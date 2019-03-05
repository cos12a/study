/*************************************************************************************************************
文件名称：key.c
硬件环境：LM3S9B90-C3
软件环境：keil 5.0 & 10636版本固件库
作    者：牙小狼 QQ:4764971 E-Mail:yaxiaolang@foxmail.com
功能说明：按键驱动
功能:
1.支持最大32个按键(视编译器所支持的数据类型而定)
2.支持不同的扫描方式(根据具体情况编写KeyIOread()和KeyInit())
3.支持按键消抖处理
4.支持按键环形缓冲区
5.支持短按键,短按shift键,长按连续键,长按shift键四种输出模式 
更新说明：
V0.1(2011-3-2)
1.支持矩阵键盘和IO按键两种
2.支持长按键和短按键两种触发方式。
V0.2(2011-3-5)
1.最大按键数量16个,可继续扩充至64个.
2.支持长按键和短按键并行触发.
3.增加短按键部分改为按键释放时返回.
V0.3(2013-11-4)
1.重写按键获取的逻辑,长按键算法不成熟,暂时去掉
2.矩阵按键没用到,暂时去掉
3.增加按键消抖算法
V0.4(2013-11-6)
1.增加环形buf
2.重写接口函数,便于在OS环境中调用,当然裸奔也支持.
V0.5(2013-11-9)
1.增加长按键的支持
2.增加shift键的支持
3.增加按键数量的定义,定义后根据按键数量选择合适的数据类型,从而节省ram和rom
4.处于兼容性的考虑,将最大按键支持数量由64个缩减至32个
V0.6(2013-11-24)
1.修正长按键输出后,还输出短按键的bug
2.修改的KeyScan()的逻辑,将长按键细分为长按键连续输出和长按键反码输出(仅一次)两种方式
3.shift键按下时也提供按键值输出,方便应用程序判断shift键的状态
4.由于第3点的修改,短按键和shift键不在冲突,即长按键/短按键/shift键相互独立
5.将无按键返回0xff,改成返回0
6.增加一个KeyFlush()函数,用于清空buf
V0.7（2016/06/30）(cos12a@163.com)
1.更新环行队列程序
*************************************************************************************************************/
#include "key.h"

/*************************************************************************************************************
                                            Macro Definition    宏定义
*************************************************************************************************************/
#if   KEY_MAX_NUM > 16
#define KEY_TYPE    INT32U
#elif KEY_MAX_NUM > 8
#define KEY_TYPE    INT16U
#else
#define KEY_TYPE    INT8U
#endif

/* 定义环形队列数据类型 */
typedef struct{
    KEY_TYPE KeyBuf[KEY_BUF_SIZE];
    INT8U   front;
    INT8U   rear;
}s_QUEUE_Def;

static s_QUEUE_Def key={
.front = 0,
.rear = 0,
};
/*************************************************************************************************************
                                            Global Variables    全局变量
*************************************************************************************************************/
static  KEY_TYPE PreScanKey  = 0;               //前次按键扫描值      
static  KEY_TYPE PreReadKey  = 0;               //前次按键读取值        
static  KEY_TYPE KeyShift    = 0;               //shift按键记录
static  KEY_TYPE KeyMask     = 0;               //按键掩码
#if LONG_KEY_EN > 0
static  INT8U  KeyPressTmr = KEY_PRESS_DLY;     //长按键判断周期
#endif

/* 函数申明 */
static void  KeyBufIn (KEY_TYPE code);
static char Empty(void);
static void SetNull(void);
static KEY_TYPE KeyBufOut (void);

/*************************************************************************************************************
函数名称：KeyIOread()
函数入口：无
函数出口：无
函数说明：按键扫描函数,需根据实际硬件情况进行移植
*************************************************************************************************************/
#if  (MCU_TYPE == ST_MCU)   /* 选择MCU */
static KEY_TYPE  KeyIOread( void )
{
 KEY_TYPE KeyScanCode=0;

  KeyScanCode|=(GPIOPinRead(KEY_UP_PB,    KEY_UP_PN   ))  ? 0 : KEY_UP;

  KeyScanCode|=(GPIOPinRead(KEY_DOWN_PB,  KEY_DOWN_PN ))  ? 0 : KEY_DOWN;

  KeyScanCode|=(GPIOPinRead(KEY_LIFT_PB,  KEY_LIFT_PN ))  ? 0 : KEY_LIFT;

  KeyScanCode|=(GPIOPinRead(KEY_RIGHT_PB, KEY_RIGHT_PN))  ? 0 : KEY_RIGHT;

  KeyScanCode|=(GPIOPinRead(KEY_ENTER_PB, KEY_ENTER_PN))  ? 0 : KEY_ENTER;

  KeyScanCode|=(GPIOPinRead(KEY_ESC_PB,   KEY_ESC_PN  ))  ? 0 : KEY_ESC;

  return KeyScanCode;
}
#endif

#if  (MCU_TYPE == AVR_MCU)
static KEY_TYPE  KeyIOread( void )
{
 KEY_TYPE KeyScanCode=0;

  KeyScanCode|=(GPIOPinRead(KEY_UP_PB,    KEY_UP_PN   ))  ? 0 : KEY_UP;

  KeyScanCode|=(GPIOPinRead(KEY_DOWN_PB,  KEY_DOWN_PN ))  ? 0 : KEY_DOWN;

  KeyScanCode|=(GPIOPinRead(KEY_LIFT_PB,  KEY_LIFT_PN ))  ? 0 : KEY_LIFT;

  KeyScanCode|=(GPIOPinRead(KEY_RIGHT_PB, KEY_RIGHT_PN))  ? 0 : KEY_RIGHT;

  KeyScanCode|=(GPIOPinRead(KEY_ENTER_PB, KEY_ENTER_PN))  ? 0 : KEY_ENTER;

  KeyScanCode|=(GPIOPinRead(KEY_ESC_PB,   KEY_ESC_PN  ))  ? 0 : KEY_ESC;

  return KeyScanCode;
}

#endif

/*************************************************************************************************************
函数名称：KeyScan()
函数入口：无
函数出口：无
函数说明：
1.功能说明
  a.按键消抖
  b.捕捉长按键和短按键
  c.根据设置将按键分成短按键/长按连续建/短按shift键/长按shift键4种方式写入buf
2.算法说明
  a.按键获取算法
    1).NowKey & PreKey                                : 电平触发 
    2).NowKey ^ PreKey                                : 边缘触发 
    3).NowKey & (NowKey ^ PreKey)或(~PreKey) & NowKey : 上升沿触发
    4).PreKey & (NowKey ^ PreKey)或PreKey & (~NowKey) : 下降沿触发
  b.滤波算法
    1).PreScanKey & NowScanKey                        : 电平触发
    2).PreReadKey & (PreScanKey ^ NowScanKey)         : 采样保持
    3).NowReadKey = 1) | 2)                           : 带采样保持的电平触发
3.调用说明
  a.对下调用的KeyIOread()中,有效按键必须为高电平,且每个bit表示一个按键值
  b.应用调用该函数的间隔应该在20ms~50ms，在调用间隔内的毛刺均可滤除。
*************************************************************************************************************/
void KeyScan(void) 
{
  KEY_TYPE NowScanKey   = 0;                                //当前按键值扫描值
  KEY_TYPE NowReadKey   = 0;                                //当前按键值
//KEY_TYPE KeyPressDown = 0;                                //按键按下                                    
  KEY_TYPE KeyRelease   = 0;                                //按键释放                                                           
  NowScanKey  = KeyIOread();
  NowReadKey  = (PreScanKey&NowScanKey)|                    //电平触发
                 PreReadKey&(PreScanKey^NowScanKey);        //采样保持(即消抖) 

//KeyPressDown  = NowReadKey & (NowReadKey ^ PreReadKey);   //上升沿触发  
  KeyRelease    = PreReadKey & (NowReadKey ^ PreReadKey);   //下降沿触发   

#if LONG_KEY_EN > 0                                         
  if(NowReadKey == PreReadKey && NowReadKey) {              //用电平触发做长按键的有效判据
    KeyPressTmr--;
    if(!KeyPressTmr){                                       //长按判断周期到,保存相应长按键值
      if(NowReadKey & ~(KEY_LONG_SHIFT)){                   //长按键模式一
        KeyBufIn(NowReadKey | KeyShift);                    //长按键重复输出
      }
      else if(NowReadKey & (KEY_LONG_SHIFT) & ~KeyMask ){   //长按键模式二        
        KeyBufIn(~(NowReadKey | KeyShift));                 //长按键反码输出作为第二功能键      
      }
      KeyPressTmr = KEY_PRESS_TMR;                          //重置连按周期,准备获取下1个长按键
      KeyMask = NowReadKey;
    }
  }
  else{
    KeyPressTmr = KEY_PRESS_DLY;                            //按键变化,重置按键判断周期
  }
#endif

  if(KeyRelease){                                           //短按键判断
      if(KeyRelease &(~KeyMask)){
        KeyShift ^= (KeyRelease & (KEY_SHORT_SHIFT));       //shift按键码(边缘触发)
        KeyBufIn(KeyRelease | KeyShift);
      }else{
        KeyMask = 0;
      }
  }
  
  PreScanKey = NowScanKey;
  PreReadKey = NowReadKey; 
}

/*************************************************************************************************************
函数名称：KeyInit()
函数入口：无
函数出口：无
函数说明：按键初始化
*************************************************************************************************************/
#if (MCU_TYPE == ST_MCU)
void KeyInit( void )
{
    MAP_SysCtlPeripheralEnable(KEY_UP_PG);
    MAP_GPIOPinTypeGPIOInput(KEY_UP_PB,KEY_UP_PN);

    MAP_SysCtlPeripheralEnable(KEY_DOWN_PG);
    MAP_GPIOPinTypeGPIOInput(KEY_DOWN_PB,KEY_DOWN_PN);

    MAP_SysCtlPeripheralEnable(KEY_LIFT_PG);
    MAP_GPIOPinTypeGPIOInput(KEY_LIFT_PB,KEY_LIFT_PN);

    MAP_SysCtlPeripheralEnable(KEY_RIGHT_PG);
    MAP_GPIOPinTypeGPIOInput(KEY_RIGHT_PB,KEY_RIGHT_PN);

    MAP_SysCtlPeripheralEnable(KEY_ENTER_PG);
    MAP_GPIOPinTypeGPIOInput(KEY_ENTER_PB,KEY_ENTER_PN);

    MAP_SysCtlPeripheralEnable(KEY_ESC_PG);
    MAP_GPIOPinTypeGPIOInput(KEY_ESC_PB,KEY_ESC_PN);
}
 
#endif

#if (MCU_TYPE == AVR_MCU)
void KeyInit(void)
{
    DDRC &= 0x3F;
    PORTC |= 0x3F;    
}
#endif



/*************************************************************************************************************
函数名称：KeyHit()
函数入口：无
函数出口：有按键时返回TRUE,无按键时返回FALSE
函数说明：判断是否有按键按下
*************************************************************************************************************/
BOOLEAN  KeyHit (void)
{
    BOOLEAN hit = FALSE;
    hit = key.front != key.rear;
    return (hit);
}

/*************************************************************************************************************
函数名称：KeyBufIn()
函数入口：code: 需放入buf的按键值
函数出口：无
函数说明：将按键值放入buf中
*************************************************************************************************************/
static void  KeyBufIn (KEY_TYPE code)
{
    key.rear = (key.rear+2)%KEY_BUF_SIZE;
    if(key.rear==key.front){
        KeyBufOut();
    }
    key.rear = (key.rear+1)%KEY_BUF_SIZE;
    key.KeyBuf[key.rear]= code;
}

/*************************************************************************************************************
函数名称：Empty()
函数入口：无
函数出口：判断队列是否为空
函数说明：
*************************************************************************************************************/
static char Empty(void)
{
    char temp;
    temp = key.rear == key.front;
    return temp;
}

/*************************************************************************************************************
函数名称：SetNull()
函数入口：无
函数出口：将队列清空
函数说明：
*************************************************************************************************************/
static void SetNull(void)
{
    key.rear = 0;
    key.front = 0;
    return;
}
/*************************************************************************************************************
函数名称：KeyFlush()
函数入口：无
函数出口：无
函数说明：清空buff中所有按键值
*************************************************************************************************************/
void KeyFlush(void)
{
    SetNull();
    return;
}
/*************************************************************************************************************
函数名称：KeyBufOut()
函数入口：无
函数出口：返回按键值，无按键返回0xFF
函数说明：从buf中获得一个按键值
*************************************************************************************************************/
static KEY_TYPE KeyBufOut (void)
{
    if(Empty()){
        return 0;
    }
    key.front = (key.front+1)%KEY_BUF_SIZE;
    return key.KeyBuf[key.front];
}
/*************************************************************************************************************
函数名称：KeyGet()
函数入口：无
函数出口：返回按键值，无按键返回0u
函数说明：从buf中获得一个按键值
*************************************************************************************************************/
INT8U  KeyGet(void)
{                         
  return (INT8U)KeyBufOut();
}  

/* 查询按键值 */
INT8U KeyQuery(void)
{
    if(Empty()){
        return 0;
    }
    return key.KeyBuf[(key.front+1)%KEY_BUF_SIZE];
}