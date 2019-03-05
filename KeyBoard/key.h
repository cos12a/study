/*************************************************************************************************************
文件名称：key.h
硬件环境：LM3S9B90
软件环境：keil 5.0 & 10636版本固件库
作    者：牙小狼 QQ:4764971 E-Mail:yaxiaolang@foxmail.com
功能说明：按键驱动
1.每20ms~50ms调用KeyScan()扫描按键,调用KeyHit()判断是否有按键,调用KeyGet()获得按键值
2.支持最大32个按键(数量可配置)
3.支持不同的扫描方式(根据具体情况编写KeyIOread()和KeyInit())
4.支持按键消抖处理(可滤掉小于2个扫描周期的毛刺)
5.支持按键环形缓冲区(长度可配置)
6.支持短按键     :短按直接输出键值
7.支持短按shift键:点击后除了输出一次短按键外,还将后续所有其他按键的键值改为,shift键+按键值(包括长短按键)
8.支持长按连续键 :长按一定时间后连续输出按键键值
9.支持长按shift键:长按一定时间后反码输出键值(仅一次)
10.4种不同按键方式可相互叠加使用,所有按键功能都能并发输出
更新说明：
V0.1(2011-3-2)
1.支持物理端口宏定义
V0.2(2013-10-31)
1.更新函数声明和宏定义内容
V0.3(2013-11-6)
1.支持按键缓冲区大小的配置
V0.4(2013-11-9)
1.支持长按键的配置
2.支持shift按键的定义
3.支持按键数量的配置,减少按键数量可节省ram和rom的开支
V0.5(2013-11-24)
1.增加KEY_LONG_SHIFT的宏定义,支持长按键做为第二功能键输出
2.将无按键返回0xff,改成返回0
3.增加一个KeyFlush()函数,用于清空buf
*************************************************************************************************************/
#ifndef __KEY__
#define __KEY__

/*************************************************************************************************************
                                            Header Files        头文件
*************************************************************************************************************/
//#include "bsp.h"
#include <stdint.h>
#include <stdbool.h>
#include "includes.h"
/*************************************************************************************************************
                                            Drive Config        驱动配置
*************************************************************************************************************/
#define KEY_MAX_NUM       8                  //定义最大按键数量
#define KEY_BUF_SIZE      10                  //定义按键缓冲区大小(1~256)
#define LONG_KEY_EN       1                   //定时是否支持长按键(1 or 0)
//#define KEY_SHORT_SHIFT   KEY_ESC|KEY_ENTER   //定义shift键,短按有效(0 or 按键扫描码)
#define KEY_SHORT_SHIFT   0                     //定义shift键,短按有效(0 or 按键扫描码)

#if LONG_KEY_EN > 0
#define KEY_PRESS_DLY     40                  //定义长按键延时n个扫描周期开始判定有效值(1~256)
#define KEY_PRESS_TMR     3                   //定义长按键n个扫描周期为1次有效扫描值(1~256)
//#define KEY_LONG_SHIFT    KEY_ENTER|KEY_DOWN  //定义长按键，反码输出一次(仅一次)按鍵,未定义的长按连续输出、不输出反码(0 or 按键扫描码)
#define KEY_LONG_SHIFT    KEY_ENTER | KEY_UP | KEY_DOWN           //定义长按键，反码输出一次(仅一次)按鍵,未定义的长按连续输出、不输出反码(0 or 按键扫描码)

#endif

/*************************************************************************************************************
                                            Key Scancode        按键扫描码
*************************************************************************************************************/
#define KEY_UP            0x0001              //每个按键占1bit,不得重复
#define KEY_DOWN          0x0002
#define KEY_LIFT          0x0004
#define KEY_RIGHT         0x0008
#define KEY_ENTER         0x0010
#define KEY_ESC           0x0020
/*************************************************************************************************************
                                            Pin Mapping         管脚映射
*************************************************************************************************************/
//Up
#define KEY_UP_PN                 GPIO_PIN_0
#define KEY_UP_PB                 GPIO_PORTB_BASE
#define KEY_UP_PG                 SYSCTL_PERIPH_GPIOB
//Down
#define KEY_DOWN_PN               GPIO_PIN_4
#define KEY_DOWN_PB               GPIO_PORTB_BASE
#define KEY_DOWN_PG               SYSCTL_PERIPH_GPIOD
//Life
#define KEY_LIFT_PN               GPIO_PIN_3
#define KEY_LIFT_PB               GPIO_PORTB_BASE
#define KEY_LIFT_PG               SYSCTL_PERIPH_GPIOB
//Righte
#define KEY_RIGHT_PN              GPIO_PIN_2
#define KEY_RIGHT_PB              GPIO_PORTB_BASE
#define KEY_RIGHT_PG              SYSCTL_PERIPH_GPIOB
//Enter
#define KEY_ENTER_PN              GPIO_PIN_1
#define KEY_ENTER_PB              GPIO_PORTB_BASE
#define KEY_ENTER_PG              SYSCTL_PERIPH_GPIOG
//Esc
#define KEY_ESC_PN                GPIO_PIN_5
#define KEY_ESC_PB                GPIO_PORTB_BASE
#define KEY_ESC_PG                SYSCTL_PERIPH_GPIOF


/*************************************************************************************************************
                                            自定义区域
*************************************************************************************************************/
/*************************************************************************************************************
函数名称：KeyIOread()
函数入口：无
函数出口：无
函数说明：按键扫描函数,需根据实际硬件情况进行移植
*************************************************************************************************************/
/*************************************************************************************************************
                                            定义MCU类型 
*************************************************************************************************************/
#define INT32U      uint32_t
#define INT8U       uint8_t
#define INT16U      uint16_t
#define BOOLEAN     bool
#define FALSE           false
#define TRUE            true
#define KEY_NULL        0u

#define AVR_MCU         1
#define ST_MCU          2

#define MCU_TYPE        AVR_MCU

#define GPIO_PORTB_BASE         1
#define GPIO_PORT                       PINC


#define GPIOPinRead(a ,b)    (GPIO_PORT &((a)<<(b)))  

#define GPIO_PIN_0              0
#define GPIO_PIN_1              1
#define GPIO_PIN_2              2
#define GPIO_PIN_3              3
#define GPIO_PIN_4              4
#define GPIO_PIN_5              5
#define GPIO_PIN_6              6
#define GPIO_PIN_7              7



#define KEY_ENTER_LONG          ((char)~KEY_ENTER)
#define KEY_UP_LONG             ((char)~KEY_UP)
#define KEY_DOWN_LONG           ((char)~KEY_DOWN)

#define KEY_ADD                 KEY_LIFT
#define KEY_DEC                 KEY_RIGHT
#define KEY_EUME_ADD            KEY_UP
#define KEY_EUME_DEC            KEY_DOWN
#define TEST_DTAT_KEY           KEY_ESC
#define KEY_EUME_ENTER          KEY_ENTER
#define KEY_SET2                KEY_DOWN
#define KEY_INIT_VALUE          KEY_LIFT
#define KEY_RUN                 (~KEY_ENTER)
#define KEY_SET1                KEY_UP
#define KEY_SET2_SAVE           KEY_ENTER

/* **************************************************
数据类型定义
***************************************************** */
#define MAX_SIZE                    8u           /* 定义环形队列最大缓冲尺寸 */


/*************************************************************************************************************
                                            Function declaration 函数声明
*************************************************************************************************************/
extern void KeyInit(void);                  //按键初始化函数
extern void KeyScan(void);                  //按键扫描函数,20ms~50ms定时调用
extern BOOLEAN KeyHit(void);                //按键判断函数,判断是否有按键按下
//extern INT32U KeyGet(void);                 //按键获得函数,获取具体按键值,无按键时返回0
extern INT8U KeyGet(void);                 //按键获得函数,获取具体按键值,无按键时返回0
extern void KeyFlush(void);					//清空按键buf

#endif



