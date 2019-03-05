/*************************************************************************************************************
�ļ����ƣ�key.c
Ӳ��������LM3S9B90-C3
���������keil 5.0 & 10636�汾�̼���
��    �ߣ���С�� QQ:4764971 E-Mail:yaxiaolang@foxmail.com
����˵������������
����:
1.֧�����32������(�ӱ�������֧�ֵ��������Ͷ���)
2.֧�ֲ�ͬ��ɨ�跽ʽ(���ݾ��������дKeyIOread()��KeyInit())
3.֧�ְ�����������
4.֧�ְ������λ�����
5.֧�ֶ̰���,�̰�shift��,����������,����shift���������ģʽ 
����˵����
V0.1(2011-3-2)
1.֧�־�����̺�IO��������
2.֧�ֳ������Ͷ̰������ִ�����ʽ��
V0.2(2011-3-5)
1.��󰴼�����16��,�ɼ���������64��.
2.֧�ֳ������Ͷ̰������д���.
3.���Ӷ̰������ָ�Ϊ�����ͷ�ʱ����.
V0.3(2013-11-4)
1.��д������ȡ���߼�,�������㷨������,��ʱȥ��
2.���󰴼�û�õ�,��ʱȥ��
3.���Ӱ��������㷨
V0.4(2013-11-6)
1.���ӻ���buf
2.��д�ӿں���,������OS�����е���,��Ȼ�㱼Ҳ֧��.
V0.5(2013-11-9)
1.���ӳ�������֧��
2.����shift����֧��
3.���Ӱ��������Ķ���,�������ݰ�������ѡ����ʵ���������,�Ӷ���ʡram��rom
4.���ڼ����ԵĿ���,����󰴼�֧��������64��������32��
V0.6(2013-11-24)
1.���������������,������̰�����bug
2.�޸ĵ�KeyScan()���߼�,��������ϸ��Ϊ��������������ͳ������������(��һ��)���ַ�ʽ
3.shift������ʱҲ�ṩ����ֵ���,����Ӧ�ó����ж�shift����״̬
4.���ڵ�3����޸�,�̰�����shift�����ڳ�ͻ,��������/�̰���/shift���໥����
5.���ް�������0xff,�ĳɷ���0
6.����һ��KeyFlush()����,�������buf
V0.7��2016/06/30��(cos12a@163.com)
1.���»��ж��г���
*************************************************************************************************************/
#include "key.h"

/*************************************************************************************************************
                                            Macro Definition    �궨��
*************************************************************************************************************/
#if   KEY_MAX_NUM > 16
#define KEY_TYPE    INT32U
#elif KEY_MAX_NUM > 8
#define KEY_TYPE    INT16U
#else
#define KEY_TYPE    INT8U
#endif

/* ���廷�ζ����������� */
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
                                            Global Variables    ȫ�ֱ���
*************************************************************************************************************/
static  KEY_TYPE PreScanKey  = 0;               //ǰ�ΰ���ɨ��ֵ      
static  KEY_TYPE PreReadKey  = 0;               //ǰ�ΰ�����ȡֵ        
static  KEY_TYPE KeyShift    = 0;               //shift������¼
static  KEY_TYPE KeyMask     = 0;               //��������
#if LONG_KEY_EN > 0
static  INT8U  KeyPressTmr = KEY_PRESS_DLY;     //�������ж�����
#endif

/* �������� */
static void  KeyBufIn (KEY_TYPE code);
static char Empty(void);
static void SetNull(void);
static KEY_TYPE KeyBufOut (void);

/*************************************************************************************************************
�������ƣ�KeyIOread()
������ڣ���
�������ڣ���
����˵��������ɨ�躯��,�����ʵ��Ӳ�����������ֲ
*************************************************************************************************************/
#if  (MCU_TYPE == ST_MCU)   /* ѡ��MCU */
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
�������ƣ�KeyScan()
������ڣ���
�������ڣ���
����˵����
1.����˵��
  a.��������
  b.��׽�������Ͷ̰���
  c.�������ý������ֳɶ̰���/����������/�̰�shift��/����shift��4�ַ�ʽд��buf
2.�㷨˵��
  a.������ȡ�㷨
    1).NowKey & PreKey                                : ��ƽ���� 
    2).NowKey ^ PreKey                                : ��Ե���� 
    3).NowKey & (NowKey ^ PreKey)��(~PreKey) & NowKey : �����ش���
    4).PreKey & (NowKey ^ PreKey)��PreKey & (~NowKey) : �½��ش���
  b.�˲��㷨
    1).PreScanKey & NowScanKey                        : ��ƽ����
    2).PreReadKey & (PreScanKey ^ NowScanKey)         : ��������
    3).NowReadKey = 1) | 2)                           : ���������ֵĵ�ƽ����
3.����˵��
  a.���µ��õ�KeyIOread()��,��Ч��������Ϊ�ߵ�ƽ,��ÿ��bit��ʾһ������ֵ
  b.Ӧ�õ��øú����ļ��Ӧ����20ms~50ms���ڵ��ü���ڵ�ë�̾����˳���
*************************************************************************************************************/
void KeyScan(void) 
{
  KEY_TYPE NowScanKey   = 0;                                //��ǰ����ֵɨ��ֵ
  KEY_TYPE NowReadKey   = 0;                                //��ǰ����ֵ
//KEY_TYPE KeyPressDown = 0;                                //��������                                    
  KEY_TYPE KeyRelease   = 0;                                //�����ͷ�                                                           
  NowScanKey  = KeyIOread();
  NowReadKey  = (PreScanKey&NowScanKey)|                    //��ƽ����
                 PreReadKey&(PreScanKey^NowScanKey);        //��������(������) 

//KeyPressDown  = NowReadKey & (NowReadKey ^ PreReadKey);   //�����ش���  
  KeyRelease    = PreReadKey & (NowReadKey ^ PreReadKey);   //�½��ش���   

#if LONG_KEY_EN > 0                                         
  if(NowReadKey == PreReadKey && NowReadKey) {              //�õ�ƽ����������������Ч�о�
    KeyPressTmr--;
    if(!KeyPressTmr){                                       //�����ж����ڵ�,������Ӧ������ֵ
      if(NowReadKey & ~(KEY_LONG_SHIFT)){                   //������ģʽһ
        KeyBufIn(NowReadKey | KeyShift);                    //�������ظ����
      }
      else if(NowReadKey & (KEY_LONG_SHIFT) & ~KeyMask ){   //������ģʽ��        
        KeyBufIn(~(NowReadKey | KeyShift));                 //���������������Ϊ�ڶ����ܼ�      
      }
      KeyPressTmr = KEY_PRESS_TMR;                          //������������,׼����ȡ��1��������
      KeyMask = NowReadKey;
    }
  }
  else{
    KeyPressTmr = KEY_PRESS_DLY;                            //�����仯,���ð����ж�����
  }
#endif

  if(KeyRelease){                                           //�̰����ж�
      if(KeyRelease &(~KeyMask)){
        KeyShift ^= (KeyRelease & (KEY_SHORT_SHIFT));       //shift������(��Ե����)
        KeyBufIn(KeyRelease | KeyShift);
      }else{
        KeyMask = 0;
      }
  }
  
  PreScanKey = NowScanKey;
  PreReadKey = NowReadKey; 
}

/*************************************************************************************************************
�������ƣ�KeyInit()
������ڣ���
�������ڣ���
����˵����������ʼ��
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
�������ƣ�KeyHit()
������ڣ���
�������ڣ��а���ʱ����TRUE,�ް���ʱ����FALSE
����˵�����ж��Ƿ��а�������
*************************************************************************************************************/
BOOLEAN  KeyHit (void)
{
    BOOLEAN hit = FALSE;
    hit = key.front != key.rear;
    return (hit);
}

/*************************************************************************************************************
�������ƣ�KeyBufIn()
������ڣ�code: �����buf�İ���ֵ
�������ڣ���
����˵����������ֵ����buf��
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
�������ƣ�Empty()
������ڣ���
�������ڣ��ж϶����Ƿ�Ϊ��
����˵����
*************************************************************************************************************/
static char Empty(void)
{
    char temp;
    temp = key.rear == key.front;
    return temp;
}

/*************************************************************************************************************
�������ƣ�SetNull()
������ڣ���
�������ڣ����������
����˵����
*************************************************************************************************************/
static void SetNull(void)
{
    key.rear = 0;
    key.front = 0;
    return;
}
/*************************************************************************************************************
�������ƣ�KeyFlush()
������ڣ���
�������ڣ���
����˵�������buff�����а���ֵ
*************************************************************************************************************/
void KeyFlush(void)
{
    SetNull();
    return;
}
/*************************************************************************************************************
�������ƣ�KeyBufOut()
������ڣ���
�������ڣ����ذ���ֵ���ް�������0xFF
����˵������buf�л��һ������ֵ
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
�������ƣ�KeyGet()
������ڣ���
�������ڣ����ذ���ֵ���ް�������0u
����˵������buf�л��һ������ֵ
*************************************************************************************************************/
INT8U  KeyGet(void)
{                         
  return (INT8U)KeyBufOut();
}  

/* ��ѯ����ֵ */
INT8U KeyQuery(void)
{
    if(Empty()){
        return 0;
    }
    return key.KeyBuf[(key.front+1)%KEY_BUF_SIZE];
}