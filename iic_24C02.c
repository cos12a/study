

#include	"macrodriver.h"
#include 	"global.h"
#include	"iic_24C02.h"
#include    "config.h"

#if 0
void  msDelay(unsigned char  ms)
{
    u8 j;

    for(; ms>0; ms--)
    for(j=255; j>0; j--)
		{
		NOP();
		NOP();
		NOP();
		NOP();
		NOP();
		}
}

#endif
void  usDelay(u8  i)
{
	for(; i>0; i--)
	{
		NOP();
		NOP();
	}
}



    /**************************************************************************
    Comment   : I2C�����W�v,��쬰Khz.
    Option    : 1 - 250Khz
    Warning   : �зǼҦ��̰��W�v��100Khz, �`��5-10K���W�Թq��.
                �ֳt�Ҧ��̰��W�v��400Khz, �`��2-5K���W�Թq��.
                �W�v�ܧC�ɥi�H�����ϥ�AVR�������W��.
    **************************************************************************/
    //�w�qI2�׬y�ư�
#define  SCL		    4
#define  SDA			5

#define SDA_IO		P4_bit.no5
#define SCL_IO		P4_bit.no4


#define  SCL_L          P4_bit.no4=0;PM4_bit.no4=0 	//�����u��X�C
#define  SCL_H          P4_bit.no4=0;PM4_bit.no4=1

#define  SDA_L          P4_bit.no5=0;PM4_bit.no5=0    //��ƽu��X�C
#define  SDA_H          P4_bit.no5=0;PM4_bit.no5=1

#define  SCL_IF_L       0==SCL_IO               //�p�Gscl���C

#define  SDA_IF_L       0==SDA_IO               //�p�Gsda���C
#define  SDA_IF_H       1==SDA_IO               //�p�Gsda����

#define  DELAY_us       usDelay(10)              //����8.6us, �i�ھڹ�ڱ��p����
#define  DELAY_ms       msDelay(100)             //����ms,�q����g�w�s�ΡA�ھڹ�ڻݭn��


#define SDA_OUTPUT_MODE			P4_bit.no5=1;PM4_bit.no5=0

#define SDA_INPUT_MODE			P4_bit.no5=1;PM4_bit.no5=1


//------�b���]�w��������------
#define e2prom 2		// <---�b���]�w��������, 1�N��24C01; 16�N��24C16; 512�N��24C512

#if e2prom==1
	#define PAGE_SIZE 8
	#define SIZE 0x007f
#elif e2prom==2
	#define PAGE_SIZE 8
	#define SIZE 0x00ff
#elif e2prom==4
	#define PAGE_SIZE 16
	#define SIZE 0x01ff
#elif e2prom==8
	#define PAGE_SIZE 16
	#define SIZE 0x03ff
#elif e2prom==16
	#define PAGE_SIZE 16
	#define SIZE 0x07ff
#elif e2prom==32
	#define PAGE_SIZE 32
	#define SIZE 0x0fff
#elif e2prom==64
	#define PAGE_SIZE 32
	#define SIZE 0x1fff
#elif e2prom==128
	#define PAGE_SIZE 64
	#define SIZE 0x3fff
#elif e2prom==256
	#define PAGE_SIZE 64
	#define SIZE 0x7fff
#elif e2prom==512
	#define PAGE_SIZE 128
	#define SIZE 0xffff
#endif


//--------�b���]�w������}-------
#define  W_ADDR_CMD     0xa0	//�g�줸�թR�O�ξ����}(�ھڦa�}��ڱ��p����), 1010 A2 A1 A0 0
#define  R_ADDR_CMD     0xa1	//Ū�R�O�r�`�ξ����}(�ھڦa�}��ڱ��p����), 1010 A2 A1 A0 1




u8  __Gu8Ack = 0;


/*============================================================
 I2C��l��
------------------------------------------------------------*/
void  i2cInit(void)
{
    SCL_H;
    SDA_H;
}

/*============================================================
�_�l�H���G   �����]SCL�^�����q���ɡA SDA �q���q���ܬ��C�q��
------------------------------------------------------------*/
void  i2cStart(void)
{
    unsigned int i=0;
    SCL_H;
    SDA_H;
	DELAY_us;
    while((SDA_IF_L) || (SCL_IF_L)) {if(++i>10000){i=0;break;}}  //�׬y�ƪŶ�
    DELAY_us;
    DELAY_us;
    while((SDA_IF_L) || (SCL_IF_L)) {if(++i>10000){i=0;break;}}
    DELAY_us;
    SDA_L;
	DELAY_us;
    SCL_L;
	DELAY_us;
}
/*============================================================
����H��:   �����]SCL�^�����q���ɡA SDA �q�C�q���ܬ����q��
------------------------------------------------------------*/
void  i2cStop(void)
{
    SDA_L;
	DELAY_us;
    SCL_H;
	DELAY_us;
    SDA_H;
	DELAY_us;
    DELAY_us;
}

/*============================================================
�g���u8a, �ê�^�����H���A
�g�J���\��^1,���Ѫ�^0
------------------------------------------------------------*/
u8  i2cWrite(u8 a)
{
	u8 i;

    for(i=0;i<8;i++)     // �o�e�@�Ӧ줸�ժ����
    {
	    if(a & 0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
    	SCL_H;
        DELAY_us;
        SCL_L;
        a = a<<1;
        DELAY_us;
    }
    DELAY_us;
    SDA_H;               //�ǳƱ�������
    DELAY_us;
    SCL_H;
    DELAY_us;
    if(SDA_IF_L)		//��������,������
    {
		__Gu8Ack = TRUE;
    }
    else                //�L����
    {
		__Gu8Ack = FALSE;
    }
    SCL_L;               // �X������u
    DELAY_us;
    return __Gu8Ack;
}


/*============================================================
    Ū�ƾ�
    ��J�G�O�_����ack�A0--�������A1--����
    ��^Ū�����줸�ո��
------------------------------------------------------------*/
u8 i2cRead(u8  ack)
{
	u8  i;
    u8  temp=0;


    SDA_H;		 		     //AVR��������]�m����XŪ�����
    for(i=0;i<8;i++)
    {
		DELAY_us;
        SCL_L;
		DELAY_us;
        SCL_H;
		DELAY_us;
        temp<<=1;
        if(SDA_IF_H)
		{
			++temp;
		}
    }
    SCL_L;
	DELAY_us;
    if(ack)                   // �D�]�������H��
    {
        SDA_L;
    }
    else
    {
        SDA_H;
    }
    DELAY_us;
    SCL_H;
	DELAY_us;
    SCL_L;                     // �X������u
    DELAY_us;
    return(temp);
}





/*============================================================
�q24C_xxŪ�ƾ�
�Ѽ�   addr   : �n��X��ƪ������};
       *device: �nŪ�J��ƪ��D���O�����}����;
       n      : �nŪ�J���ƾڭӼ�;
�ѼƱ���:  addr+(n-1)����j�󾹥󪺳̰���}; n����>0;
-------------------------------------------------------------*/
//void  ReadStr24c_02(u16 addr, u8 *device, u16 n)
//void Read24c02(u8 addr,u8 *device,u8 n)
void Read24c02(u8 addr,u8 *device,u8 n)

{
	u8   i=0;
    u8  j;

    while(1)
    {
        if(++i > 10)  {return;}
        i2cStart();
        i2cWrite(W_ADDR_CMD);		            //�g����a�}
        if(FALSE == __Gu8Ack)   {continue;}
        #if e2prom>16                           //�g����l�a�}
		    i2cWrite(addr>>8);
		    if(FALSE == __Gu8Ack)   {continue;}
        #endif
		i2cWrite(addr);
            if(FALSE == __Gu8Ack)   {continue;}
		i2cStart();
		i2cWrite(R_ADDR_CMD);		            //�o�����}�A�ǳ�Ū�ƾ�
        if(FALSE == __Gu8Ack)   {continue;}
        for(j=0; j<(n-1); j++)
		{
            *device = i2cRead(1);	            //�q����Ū�@�Ӧ줸�աA������
			device++;
		}
        *device = i2cRead(0);	                //�q����Ū�̫�@�Ӧ줸�աA������
        i2cStop();
        DELAY_us;
        DELAY_us;
        return;
    }
}


/*============================================================
�V24Cxx�g�ƾ�
�Ѽ�   addr   : �q���l�a�};
       *device: �D���O�����}����;
       n      : �g�J��ƪ��Ӽ�;
�ѼƱ���:  addr+(n-1)����j��q�����󪺳̰���}; n����>0;
�����void   WriteAnyStr24c_xx(u16 addr, u8 *device, u16 n)�ե�
�`�N�G
    �D����o�e���O�h�ܤ@�㭶����Ʀ줸�ա]1�^�A�Ӥ��O�������A
    �o�Ǹ�Ʀ줸���{�ɦs�x�b�������w�ľ����C
    �b�D����o�e������󤧫�A�o�Ǹ�ƱN�Q�g�J�O����A�ҥH�b
    �����H������n�[�@�q���ɵ��q����g�w�s���
------------------------------------------------------------*/
//void  WriteStr24c_xx(u16 addr, u8 *device, u16 n)
//void Write24c02(uchar8_t addr,uchar8_t *device,uchar8_t n)
void Write24c02(uchar8_t addr,u8 *device,uchar8_t n)

{
    u8   i=0;
    u8  j;

	while(1)
    {
        if(++i > 10)  {break;}
        i2cStart();
        i2cWrite(W_ADDR_CMD);	                //����a�}
        if(FALSE == __Gu8Ack)   {continue;}
        #if e2prom>16                           //�g����a�}�l�a�}
		    i2cWrite(addr>>8);
		    if(FALSE == __Gu8Ack)   {continue;}
        #endif
		i2cWrite(addr);
        if(FALSE == __Gu8Ack)   {continue;}
		for(j=0; j<n; j++)			            //�o�e�n�g�J�����
		{
            i2cWrite(*device);
			device++;
        }
        i2cStop();
        DELAY_ms;                               //�q����g�w�ĸ��
        return;
    }
}

#if 0
/*============================================================
�V24Cxx�g����Ŷ����\�����N�j�p�����

�Ѽ�: addr   :  �q���l�a�};
      *device:  �D���O�����}����;
       n     :  �g�J����ƭӼ�
�ѼƱ���addr :  addr+(n-1)����j��q�����󪺳̰���}; n����>0;
-------------------------------------------------------------*/
void   WriteAnyStr24c_xx(u16 addr, u8 *device, u16 n)

{
	u16  i;

    //�T�w��}�P����}���t,�p�⵲���ɡGi=�C���Ѿl���Ŷ�
    i = addr/PAGE_SIZE;
	if(i)
    {
        i = (u16)(PAGE_SIZE*(i+1)-addr);
    }
    else
    {
        i = PAGE_SIZE-addr;
    }
    if(i >= n)  //�p�Gaddr�Ҧb�����Ѿl�Ŷ� >= n, �N�����g�Jn�Ӹ��
    {
        WriteStr24c_xx(addr, device, n);
    }
    else //�p�Gaddr�Ҧb�����Ѿl�Ŷ� < n, �N���gaddr�Ҧb�����Ѿl�Ŷ�i
    {
        WriteStr24c_xx(addr, device, i);
        n      = n-i;                              //��s�ѤU��ƭӼ�
        addr   = addr+i;	                       //��s�ѤU��ƪ��_�l��}
        device = device+i;                         //���w�a�}�H���W�[

        while(n >= PAGE_SIZE)	//��ѤU��Ƽg�J����,����PAGE_SIZE�����פ@���@�����g�J
        {
            WriteStr24c_xx(addr, device, PAGE_SIZE);
            n      = n-PAGE_SIZE;		            //��s�Ѿl��ƭӼ�
            addr   = addr+PAGE_SIZE;	            //��s�ѤU��ƪ��_�l��}
            device = device+PAGE_SIZE;              //���w�a�}�H���W�[
        }
        if(n)			                        //��̫�ѤU���p��@��PAGE_SIZE���ת���Ƽg�J����
        {
            WriteStr24c_xx(addr, device, n);
        }
    }
}

#endif
