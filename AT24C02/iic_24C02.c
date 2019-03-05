//#include	"macrodriver.h"
//#include 	"global.h"
#include	"iic_24C02.h"
//#include    "config.h"

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
    Comment   : I2C時鐘頻率,單位為Khz.
    Option    : 1 - 250Khz
    Warning   : 標準模式最高頻率為100Khz, 常用5-10K的上拉電阻.
                快速模式最高頻率為400Khz, 常用2-5K的上拉電阻.
                頻率很低時可以直接使用AVR的內部上拉.
    **************************************************************************/
    //定義I2匯流排埠
#define  SCL		    4
#define  SDA			5

#define SDA_IO		P4_bit.no5
#define SCL_IO		P4_bit.no4


#define  SCL_L          P4_bit.no4=0;PM4_bit.no4=0 	//時鐘線輸出低
#define  SCL_H          P4_bit.no4=0;PM4_bit.no4=1

#define  SDA_L          P4_bit.no5=0;PM4_bit.no5=0    //資料線輸出低
#define  SDA_H          P4_bit.no5=0;PM4_bit.no5=1

#define  SCL_IF_L       0==SCL_IO               //如果scl為低

#define  SDA_IF_L       0==SDA_IO               //如果sda為低
#define  SDA_IF_H       1==SDA_IO               //如果sda為高

#define  DELAY_us       usDelay(10)              //延時8.6us, 可根據實際情況改變
#define  DELAY_ms       msDelay(100)             //延時ms,從器件寫緩存用，根據實際需要改


#define SDA_OUTPUT_MODE			P4_bit.no5=1;PM4_bit.no5=0

#define SDA_INPUT_MODE			P4_bit.no5=1;PM4_bit.no5=1


//------在此設定晶片型號------
#define e2prom 2		// <---在此設定晶片型號, 1代表24C01; 16代表24C16; 512代表24C512

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


//--------在此設定晶片位址-------
#define  W_ADDR_CMD     0xa0	//寫位元組命令及器件位址(根據地址實際情況改變), 1010 A2 A1 A0 0
#define  R_ADDR_CMD     0xa1	//讀命令字節及器件位址(根據地址實際情況改變), 1010 A2 A1 A0 1




u8  __Gu8Ack = 0;


/*============================================================
 I2C初始化
------------------------------------------------------------*/
void  i2cInit(void)
{
    SCL_H;
    SDA_H;
}

/*============================================================
起始信號：   時鐘（SCL）為高電平時， SDA 從高電平變為低電平
------------------------------------------------------------*/
void  i2cStart(void)
{
    unsigned int i=0;
    SCL_H;
    SDA_H;
	DELAY_us;
    while((SDA_IF_L) || (SCL_IF_L)) {if(++i>10000){i=0;break;}}  //匯流排空閒
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
停止信號:   時鐘（SCL）為高電平時， SDA 從低電平變為高電平
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
寫資料u8a, 並返回應答信號，
寫入成功返回1,失敗返回0
------------------------------------------------------------*/
u8  i2cWrite(u8 a)
{
	u8 i;

    for(i=0;i<8;i++)     // 發送一個位元組的資料
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
    SDA_H;               //準備接受應答
    DELAY_us;
    SCL_H;
    DELAY_us;
    if(SDA_IF_L)		//測試應答,有應答
    {
		__Gu8Ack = TRUE;
    }
    else                //無應答
    {
		__Gu8Ack = FALSE;
    }
    SCL_L;               // 鉗住時鐘線
    DELAY_us;
    return __Gu8Ack;
}


/*============================================================
    讀數據
    輸入：是否應答ack，0--不應答，1--應答
    返回讀取的位元組資料
------------------------------------------------------------*/
u8 i2cRead(u8  ack)
{
	u8  i;
    u8  temp=0;


    SDA_H;		 		     //AVR類單片機設置為輸出讀取資料
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
    if(ack)                   // 主設備應答信號
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
    SCL_L;                     // 鉗住時鐘線
    DELAY_us;
    return(temp);
}





/*============================================================
從24C_xx讀數據
參數   addr   : 要輸出資料的器件位址;
       *device: 要讀入資料的主機記憶體位址指標;
       n      : 要讀入的數據個數;
參數條件:  addr+(n-1)不能大於器件的最高位址; n必須>0;
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
        i2cWrite(W_ADDR_CMD);		            //寫器件地址
        if(FALSE == __Gu8Ack)   {continue;}
        #if e2prom>16                           //寫器件子地址
		    i2cWrite(addr>>8);
		    if(FALSE == __Gu8Ack)   {continue;}
        #endif
		i2cWrite(addr);
            if(FALSE == __Gu8Ack)   {continue;}
		i2cStart();
		i2cWrite(R_ADDR_CMD);		            //發器件位址，準備讀數據
        if(FALSE == __Gu8Ack)   {continue;}
        for(j=0; j<(n-1); j++)
		{
            *device = i2cRead(1);	            //從器件讀一個位元組，並應答
			device++;
		}
        *device = i2cRead(0);	                //從器件讀最後一個位元組，不應答
        i2cStop();
        DELAY_us;
        DELAY_us;
        return;
    }
}


/*============================================================
向24Cxx寫數據
參數   addr   : 從機子地址;
       *device: 主機記憶體位址指標;
       n      : 寫入資料的個數;
參數條件:  addr+(n-1)不能大於從機器件的最高位址; n必須>0;
給函數void   WriteAnyStr24c_xx(u16 addr, u8 *device, u16 n)調用
注意：
    主器件發送的是多至一整頁的資料位元組（1），而不是停止條件，
    這些資料位元組臨時存儲在片內頁緩衝器中。
    在主器件發送停止條件之後，這些資料將被寫入記憶體，所以在
    結束信號之後要加一段延時給從器件寫緩存資料
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
        i2cWrite(W_ADDR_CMD);	                //器件地址
        if(FALSE == __Gu8Ack)   {continue;}
        #if e2prom>16                           //寫器件地址子地址
		    i2cWrite(addr>>8);
		    if(FALSE == __Gu8Ack)   {continue;}
        #endif
		i2cWrite(addr);
        if(FALSE == __Gu8Ack)   {continue;}
		for(j=0; j<n; j++)			            //發送要寫入的資料
		{
            i2cWrite(*device);
			device++;
        }
        i2cStop();
        DELAY_ms;                               //從器件寫緩衝資料
        return;
    }
}

#if 0
/*============================================================
向24Cxx寫器件空間允許的任意大小的資料

參數: addr   :  從機子地址;
      *device:  主機記憶體位址指標;
       n     :  寫入的資料個數
參數條件addr :  addr+(n-1)不能大於從機器件的最高位址; n必須>0;
-------------------------------------------------------------*/
void   WriteAnyStr24c_xx(u16 addr, u8 *device, u16 n)

{
	u16  i;

    //確定位址與頁位址的差,計算結束時：i=每頁剩餘的空間
    i = addr/PAGE_SIZE;
	if(i)
    {
        i = (u16)(PAGE_SIZE*(i+1)-addr);
    }
    else
    {
        i = PAGE_SIZE-addr;
    }
    if(i >= n)  //如果addr所在頁的剩餘空間 >= n, 就直接寫入n個資料
    {
        WriteStr24c_xx(addr, device, n);
    }
    else //如果addr所在頁的剩餘空間 < n, 就先寫addr所在頁的剩餘空間i
    {
        WriteStr24c_xx(addr, device, i);
        n      = n-i;                              //更新剩下資料個數
        addr   = addr+i;	                       //更新剩下資料的起始位址
        device = device+i;                         //指針地址隨之增加

        while(n >= PAGE_SIZE)	//把剩下資料寫入器件,先按PAGE_SIZE為長度一頁一頁的寫入
        {
            WriteStr24c_xx(addr, device, PAGE_SIZE);
            n      = n-PAGE_SIZE;		            //更新剩餘資料個數
            addr   = addr+PAGE_SIZE;	            //更新剩下資料的起始位址
            device = device+PAGE_SIZE;              //指針地址隨之增加
        }
        if(n)			                        //把最後剩下的小於一個PAGE_SIZE長度的資料寫入器件
        {
            WriteStr24c_xx(addr, device, n);
        }
    }
}

#endif