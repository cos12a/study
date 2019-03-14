/* Includes ------------------------------------------------------------------*/
#include "i2c.h"


// I2C stubs, implementations to be provided by the user.
__weak uint8_t ADS1x1x_i2c_start_write(uint8_t i2c_address)
{        
    /* Prevent unused argument(s) compilation warning */
  UNUSED(i2c_address);
  return 0; 
}
__weak uint8_t ADS1x1x_i2c_write(uint8_t x)
{
     UNUSED(x);   
     return 0;
}
__weak uint8_t ADS1x1x_i2c_start_read(uint8_t i2c_address, uint16_t bytes_to_read)
{
  UNUSED(i2c_address);
  UNUSED(bytes_to_read);
  return 0;
  
}
__weak uint8_t ADS1x1x_i2c_read(void)
{
  return 0;
}
__weak uint8_t ADS1x1x_i2c_stop(void)
{
  return 0;
}



/**************************************************************************/
/*!
    @brief  Writes 16 bits to the specified destination register.
*/
/**************************************************************************/
void ADS1x1x_write_register(uint8_t i2c_address, uint8_t reg, uint16_t value)
{
//    sendDef sData;
    uint8_t sData[3];
    sData[0] = reg;
    sData[1] = (uint8_t)(value>>8);
    sData[2] = (uint8_t)(value);
    HAL_I2C_Master_Transmit(&hi2c2, i2c_address<<1, &sData[0], 3, 100);
}


/**************************************************************************/
/*!
    @brief  Read 16 bits from the specified destination register.
*/
/**************************************************************************/
uint16_t ADS1x1x_read_register(uint8_t i2c_address, uint8_t reg)
{
    uint8_t result[2];
    HAL_StatusTypeDef sta;
    HAL_I2C_Master_Transmit(&hi2c2, i2c_address<<1, &reg, 1, 100);
    sta = HAL_I2C_Master_Receive(&hi2c2, ((i2c_address<<1)|0x01), &result[0], 2, 100);
    if (sta != HAL_OK)
    {
           return 0;
    }
    return ((uint16_t)result[0]<<8)|result[1];     
}
