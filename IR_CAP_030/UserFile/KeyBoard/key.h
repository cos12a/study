/*************************************************************************************************************
�ļ����ƣ�key.h
Ӳ��������LM3S9B90
���������keil 5.0 & 10636�汾�̼���
��    �ߣ���С�� QQ:4764971 E-Mail:yaxiaolang@foxmail.com
����˵������������
1.ÿ20ms~50ms����KeyScan()ɨ�谴��,����KeyHit()�ж��Ƿ��а���,����KeyGet()��ð���ֵ
2.֧�����32������(����������)
3.֧�ֲ�ͬ��ɨ�跽ʽ(���ݾ��������дKeyIOread()��KeyInit())
4.֧�ְ�����������(���˵�С��2��ɨ�����ڵ�ë��)
5.֧�ְ������λ�����(���ȿ�����)
6.֧�ֶ̰���     :�̰�ֱ�������ֵ
7.֧�ֶ̰�shift��:�����������һ�ζ̰�����,���������������������ļ�ֵ��Ϊ,shift��+����ֵ(�������̰���)
8.֧�ֳ��������� :����һ��ʱ����������������ֵ
9.֧�ֳ���shift��:����һ��ʱ����������ֵ(��һ��)
10.4�ֲ�ͬ������ʽ���໥����ʹ��,���а������ܶ��ܲ������
����˵����
V0.1(2011-3-2)
1.֧������˿ں궨��
V0.2(2013-10-31)
1.���º��������ͺ궨������
V0.3(2013-11-6)
1.֧�ְ�����������С������
V0.4(2013-11-9)
1.֧�ֳ�����������
2.֧��shift�����Ķ���
3.֧�ְ�������������,���ٰ��������ɽ�ʡram��rom�Ŀ�֧
V0.5(2013-11-24)
1.����KEY_LONG_SHIFT�ĺ궨��,֧�ֳ�������Ϊ�ڶ����ܼ����
2.���ް�������0xff,�ĳɷ���0
3.����һ��KeyFlush()����,�������buf
*************************************************************************************************************/
#ifndef __KEY__
#define __KEY__

/*************************************************************************************************************
                                            Header Files        ͷ�ļ�
*************************************************************************************************************/
//#include "bsp.h"
#include <stdint.h>
#include <stdbool.h>
#include "includes.h"
/*************************************************************************************************************
                                            Drive Config        ��������
*************************************************************************************************************/
#define KEY_MAX_NUM       8                  //������󰴼�����
#define KEY_BUF_SIZE      10                  //���尴����������С(1~256)
#define LONG_KEY_EN       1                   //��ʱ�Ƿ�֧�ֳ�����(1 or 0)
//#define KEY_SHORT_SHIFT   KEY_ESC|KEY_ENTER   //����shift��,�̰���Ч(0 or ����ɨ����)
#define KEY_SHORT_SHIFT   0                     //����shift��,�̰���Ч(0 or ����ɨ����)

#if LONG_KEY_EN > 0
#define KEY_PRESS_DLY     40                  //���峤������ʱn��ɨ�����ڿ�ʼ�ж���Чֵ(1~256)
#define KEY_PRESS_TMR     3                   //���峤����n��ɨ������Ϊ1����Чɨ��ֵ(1~256)
//#define KEY_LONG_SHIFT    KEY_ENTER|KEY_DOWN  //���峤�������������һ��(��һ��)���I,δ����ĳ���������������������(0 or ����ɨ����)
#define KEY_LONG_SHIFT    KEY_ENTER | KEY_UP | KEY_DOWN           //���峤�������������һ��(��һ��)���I,δ����ĳ���������������������(0 or ����ɨ����)

#endif

/*************************************************************************************************************
                                            Key Scancode        ����ɨ����
*************************************************************************************************************/
#define KEY_UP            0x0001              //ÿ������ռ1bit,�����ظ�
#define KEY_DOWN          0x0002
#define KEY_LIFT          0x0004
#define KEY_RIGHT         0x0008
#define KEY_ENTER         0x0010
#define KEY_ESC           0x0020
/*************************************************************************************************************
                                            Pin Mapping         �ܽ�ӳ��
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
                                            �Զ�������
*************************************************************************************************************/
/*************************************************************************************************************
�������ƣ�KeyIOread()
������ڣ���
�������ڣ���
����˵��������ɨ�躯��,�����ʵ��Ӳ�����������ֲ
*************************************************************************************************************/
/*************************************************************************************************************
                                            ����MCU���� 
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
�������Ͷ���
***************************************************** */
#define MAX_SIZE                    8u           /* ���廷�ζ�����󻺳�ߴ� */


/*************************************************************************************************************
                                            Function declaration ��������
*************************************************************************************************************/
extern void KeyInit(void);                  //������ʼ������
extern void KeyScan(void);                  //����ɨ�躯��,20ms~50ms��ʱ����
extern BOOLEAN KeyHit(void);                //�����жϺ���,�ж��Ƿ��а�������
//extern INT32U KeyGet(void);                 //������ú���,��ȡ���尴��ֵ,�ް���ʱ����0
extern INT8U KeyGet(void);                 //������ú���,��ȡ���尴��ֵ,�ް���ʱ����0
extern void KeyFlush(void);					//��հ���buf

#endif



