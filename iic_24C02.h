

#ifndef	__iic_xx_h__
#define	__iic_xx_h__


#define u8		unsigned char
#define u16		unsigned int


//-------------���º����������������-------------------------------------------

//void  msDelay(u8  ms);
//void  usDelay(u8  i);
//void  ReadStr24c_02(u16 addr, u8 *device, u16 n);      //��24cxx��������
//void  WriteAnyStr24c_02(u16 addr, u8 *device, u16 n);  //������д��24cxx

//void Write24c02(uchar8_t addr,uchar8_t *device,uchar8_t n);
//void Read24c02(uchar8_t addr,uchar8_t *device,uchar8_t n);

//void Read24c02(u8 addr,u8 *device,u8 n);

void Read24c02(u8 addr,u8 *device,u8 n);
void Write24c02(uchar8_t addr,u8 *device,uchar8_t n);


#endif




