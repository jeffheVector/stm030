#include "start.h"
#include "485.h"
#ifdef START
#define SUB485ADDR_1 0X01//第一个485设备地址，是要查询的那个485
#define SUB485ADDR_2 0X02//要发送给这个485数据
#define FUNC485_1 0x03
//#define FUNC485_1 0x03

static void Callback_DealTest1(u8 *pData)
{
	PRINTF("Callback_DealTest1\r\n");
}

void Start_Init(void)
{
	//初始化
	PRINTF("INIT START\r\n");
	Register485Callback(SUB485ADDR_1,FUNC485_1,Callback_DealTest1);
}

#endif
