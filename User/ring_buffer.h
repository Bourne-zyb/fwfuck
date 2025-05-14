#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>
#include <string.h>

#define RING_BUFFER_SUCCESS     		0x00
#define RING_BUFFER_ERROR       		0x01
#define RING_BUFFER_EMPTY_ERROR       	0x01
#define RING_BUFFER_FULL_ERROR       	0x01
//�?��缓冲区结构体
typedef struct
{
    uint16_t head ;//操作头指�?
    uint16_t tail ;//操作尾指�?
    uint16_t lenght ;//已储存的数据�?
    uint8_t *array_addr ;//缓冲区储存数组基地址
    uint16_t max_lenght ;//缓冲区最大可储存数据�?
}STRUCT_RING_BUFFER;

uint8_t  Ring_Buffer_Init( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t *buffer_addr ,uint32_t buffer_size );//初�?化新缓冲�?
uint8_t  Ring_Buffer_Write_Byte( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t data );//向缓冲区里写一�?���?
uint8_t  Ring_Buffer_Write( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t *pdata, uint16_t size );//向缓冲区里写size�?���?
uint8_t  Ring_Buffer_Write_Byte_Force( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t data);
uint8_t  Ring_Buffer_Read_Byte( STRUCT_RING_BUFFER *ring_buffer_handle );//从缓冲区读取一�?���?
uint16_t Ring_Buffer_Read( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t *pData, uint16_t size );//从缓冲区读取size�?���?
uint8_t  Ring_Buffer_Get_Lenght( STRUCT_RING_BUFFER *ring_buffer_handle ); //获取缓冲区里已储存的数据长度
uint8_t  Ring_Buffer_Find_Byte( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t data );//从缓冲区查找一�?���?
uint8_t  Ring_Buffer_Clean( STRUCT_RING_BUFFER *ring_buffer_handle );//清空缓冲�?

#endif /*__RING_BUFFER_H__*/
