#include "ring_buffer.h"

/**
 * \brief 初�?化新缓冲�?
 * \param[out] ring_buffer_handle: 待初始化的缓冲区结构体句�?
 * \param[in] buffer_addr: 外部定义的缓冲区数组，类型必须为 uint8_t
 * \param[in] buffer_size: 外部定义的缓冲区数组空间
 * \return 返回缓冲区初始化的结�?
 *      \arg RING_BUFFER_SUCCESS: 初�?化成�?
 *      \arg RING_BUFFER_ERROR: 初�?化失�?
*/
uint8_t Ring_Buffer_Init( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t *buffer_addr ,uint32_t buffer_size)
{
    ring_buffer_handle->head = 0 ;//复位头指�?
    ring_buffer_handle->tail = 0 ;//复位尾指�?
    ring_buffer_handle->lenght = 0 ;//复位已存储数�?���?
    ring_buffer_handle->array_addr = buffer_addr ;//缓冲区储存数组基地址
    ring_buffer_handle->max_lenght = buffer_size ;//缓冲区最大可储存数据�?
    if(ring_buffer_handle->max_lenght < 2)//缓冲区数组必须有两个元素以上
        return RING_BUFFER_ERROR ;//缓冲区数组过小，队列初�?化失�?
    else
        return RING_BUFFER_SUCCESS ;//缓冲区初始化成功
}

/**
 * \brief 向缓冲区尾部写一�?���?
 * \param[out] ring_buffer_handle: 缓冲区结构体句柄
 * \param[in] data: 要写入的字节
 * \return 返回缓冲区写字节的结�?
 *      \arg RING_BUFFER_SUCCESS: 写入成功
 *      \arg RING_BUFFER_ERROR: 写入失败
*/
uint8_t Ring_Buffer_Write_Byte( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t data)
{
    //缓冲区数组已满，产生覆盖错�?
    if( ring_buffer_handle->lenght >= ring_buffer_handle->max_lenght )
    {
        return RING_BUFFER_ERROR ;
    }
    else
    {
        *(ring_buffer_handle->array_addr + ring_buffer_handle->tail) = data;//基地址+偏移量，存放数据
        ring_buffer_handle->lenght ++ ;//数据量�?�?+1
        ring_buffer_handle->tail ++ ;//尾指针后�?
    }
    //如果尾指针超越了数组�?��，尾指针指向缓冲区数组开头，形成�?��
    if( ring_buffer_handle->tail >= ring_buffer_handle->max_lenght )
        ring_buffer_handle->tail = 0 ;
	return RING_BUFFER_SUCCESS ;
}
/**
 * \brief 向缓冲区尾部写一�?���?
 * \param[out] ring_buffer_handle: 缓冲区结构体句柄
 * \param[in] data: 要写入的字节
 * \return 返回缓冲区写字节的结�?
 *      \arg RING_BUFFER_SUCCESS: 写入成功
 *      \arg RING_BUFFER_ERROR: 写入失败
*/
uint8_t Ring_Buffer_Write( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t *pdata, uint16_t size )
{
    uint16_t Temp;
    //缓冲区数组已满，产生覆盖错�?
    if( ring_buffer_handle->lenght+size > ring_buffer_handle->max_lenght )
    {
        return RING_BUFFER_ERROR;
    }

    Temp = ring_buffer_handle->tail+size;
    if( Temp < ring_buffer_handle->max_lenght )
    {
        memcpy( (ring_buffer_handle->array_addr + ring_buffer_handle->tail), pdata, size );
        ring_buffer_handle->tail  += size;
    }
    else if( Temp == ring_buffer_handle->max_lenght )
    {
        memcpy( (ring_buffer_handle->array_addr + ring_buffer_handle->tail), pdata, size );
        ring_buffer_handle->tail  = 0;
    }
    else
    {
        uint16_t size1 = ring_buffer_handle->max_lenght-ring_buffer_handle->tail;
        uint16_t size2 = size - size1;
        memcpy( (ring_buffer_handle->array_addr + ring_buffer_handle->tail), pdata, size1 );
        memcpy( ring_buffer_handle->array_addr, &pdata[size1], size2 );
        ring_buffer_handle->tail = size2;
    }
    ring_buffer_handle->lenght += size;
	return RING_BUFFER_SUCCESS ;
}


uint8_t Ring_Buffer_Write_Byte_Force( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t data)
{

    *(ring_buffer_handle->array_addr + ring_buffer_handle->tail) = data;//基地址+偏移量，存放数据
    ring_buffer_handle->lenght ++ ;//数据量�?�?+1
    ring_buffer_handle->tail ++ ;//尾指针后�?

    //如果尾指针超越了数组�?��，尾指针指向缓冲区数组开头，形成�?��
    if(ring_buffer_handle->tail > (ring_buffer_handle->max_lenght - 1))
        ring_buffer_handle->tail = 0 ;
	return RING_BUFFER_SUCCESS ;
}

/**
 * \brief 从缓冲区头指针�?取一�?���?
 * \param[in] ring_buffer_handle: 缓冲区结构体句柄
 * \return 返回读取的字�?
*/
uint8_t Ring_Buffer_Read_Byte( STRUCT_RING_BUFFER *ring_buffer_handle)
{
    uint8_t data = 0; ;
    if (ring_buffer_handle->lenght != 0)//有数�?��读出
    {
        data = *(ring_buffer_handle->array_addr + ring_buffer_handle->head);//读取数据
        ring_buffer_handle->head ++ ;
        ring_buffer_handle->lenght -- ;//数据量�?�?-1
        //如果头指针超越了数组�?��，头指针指向数组开头，形成�?��
        if(ring_buffer_handle->head > (ring_buffer_handle->max_lenght - 1))
            ring_buffer_handle->head = 0 ;
    }
    return data ;
}
/**
 * \brief 从缓冲区头指针�?取一�?���?
 * \param[in] ring_buffer_handle: 缓冲区结构体句柄
 * \return 返回读取的字节长�?
*/
uint16_t Ring_Buffer_Read( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t *pData, uint16_t size )
{
    uint16_t Temp;
    if( !ring_buffer_handle->lenght )
    {
        return 0;
    }

    if( ring_buffer_handle->lenght < size )
    {
        size = ring_buffer_handle->lenght;
    }
    ring_buffer_handle->lenght -= size;

    Temp = ring_buffer_handle->head+size;
    if( Temp < ring_buffer_handle->max_lenght )
    {
        memcpy( pData, (ring_buffer_handle->array_addr + ring_buffer_handle->head), size );
        ring_buffer_handle->head += size;
    }
    else if( Temp == ring_buffer_handle->max_lenght )
    {
        memcpy( pData, (ring_buffer_handle->array_addr + ring_buffer_handle->head), size );
        ring_buffer_handle->head = 0;
    }
    else
    {
        uint16_t size1 = ring_buffer_handle->max_lenght - ring_buffer_handle->head;
        uint16_t size2 = size - size1;
        memcpy( pData, (ring_buffer_handle->array_addr + ring_buffer_handle->head), size1 );
        memcpy( &pData[size1], ring_buffer_handle->array_addr, size2 );
        ring_buffer_handle->head = size2;
    }
    return size;
}
/**
 * \brief 获取缓冲区里已储存的数据长度
 * \param[in] ring_buffer_handle: 缓冲区结构体句柄
 * \return 返回缓冲区里已储存的数据长度
*/
uint8_t Ring_Buffer_Get_Lenght( STRUCT_RING_BUFFER *ring_buffer_handle )
{
    return ring_buffer_handle->lenght ;
}
/**
 * \brief 向缓冲区查找一�?���?
 * \param[out] ring_buffer_handle: 缓冲区结构体句柄
 * \param[in] data: 要查找的字节
 * \return 查找字节的结�?
 *      \arg RING_BUFFER_SUCCESS: 查找成功
 *      \arg RING_BUFFER_ERROR: 查找失败
*/
uint8_t Ring_Buffer_Find_Byte( STRUCT_RING_BUFFER *ring_buffer_handle, uint8_t data )
{
    //缓冲区数组已满，产生覆盖错�?
    if( ring_buffer_handle->lenght == 0 )
	{
		return RING_BUFFER_ERROR;
	}
    else
    {
		uint8_t index = ring_buffer_handle->head;
		while( index != ring_buffer_handle->tail )
		{
			if( data == ring_buffer_handle->array_addr[index++] )
			{
				return RING_BUFFER_SUCCESS ;
			}
			if( index >= ring_buffer_handle->max_lenght )
			{
				index = 0;
			}
		}
    }
	return RING_BUFFER_ERROR;
}
/**
* \brief 清空缓冲区数�?
 * \param[out]
 * \param[in]
 * \return
 *      \arg RING_BUFFER_SUCCESS: 清空成功
 *      \arg RING_BUFFER_ERROR: 清空失败
*/
uint8_t Ring_Buffer_Clean( STRUCT_RING_BUFFER *ring_buffer_handle )
{
    ring_buffer_handle->lenght = 0;
    ring_buffer_handle->head = 0;
    ring_buffer_handle->tail = 0;

    return RING_BUFFER_SUCCESS ;
}
