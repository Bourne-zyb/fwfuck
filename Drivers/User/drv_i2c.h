#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__



void I2C_Init( void );
uint8_t I2C_Read( uint16_t DevAddress, uint16_t MemAddress, void *pData, uint32_t Size );
uint8_t I2C_Write( uint16_t DevAddress, uint16_t MemAddress, void *pData, uint32_t Size );
uint8_t IsDeviceReady( uint16_t DevAddress );


#endif
