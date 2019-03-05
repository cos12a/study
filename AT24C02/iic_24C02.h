

#ifndef	__iic_xx_h__
#define	__iic_xx_h__


#define u8		unsigned char
#define u16		unsigned int


//-------------以下函数供其它程序调用-------------------------------------------

//void  msDelay(u8  ms);
//void  usDelay(u8  i);
//void  ReadStr24c_02(u16 addr, u8 *device, u16 n);      //从24cxx读出数据
//void  WriteAnyStr24c_02(u16 addr, u8 *device, u16 n);  //把数据写入24cxx

//void Write24c02(uchar8_t addr,uchar8_t *device,uchar8_t n);
//void Read24c02(uchar8_t addr,uchar8_t *device,uchar8_t n);

//void Read24c02(u8 addr,u8 *device,u8 n);

void Read24c02(u8 addr,u8 *device,u8 n);
void Write24c02(uchar8_t addr,u8 *device,uchar8_t n);


#endif




