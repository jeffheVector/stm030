#ifndef __485_H
#define __485_H
#include "port_sys.h"
//#ifdef C6T6
//#define _485EN_PORT GPIOB
//#define _485EN_IO_PIN GPIO_PIN_12
//#endif

#ifdef MOTOR485
#define _485EN_PORT GPIOA
#define _485EN_IO_PIN GPIO_PIN_11
#endif
#ifdef HDEP_GATHER
#define _485EN_PORT GPIOB
#define _485EN_IO_PIN GPIO_PIN_1
#endif
#ifdef START//Õâ¸öÊÇ
#define _485EN_PORT GPIOB
#define _485EN_IO_PIN GPIO_PIN_1
#endif
#define MAX485CMD_NUM 255
#define MAX485PKG_LEN 8
//#define CMD485_SETDEBUGLOG 254//0xFE 0xFF 0x00 0x00 0x00 0x01 CRC1 CRC2
#define DEFAULT485_BAUDRATE 115200U

typedef void (*CALLBACK_485)(u8 *data);
typedef struct
{
	u8 addr;
	u8 cmd;
	CALLBACK_485 callback;
}_485_func_callback;
extern uint8_t rcv485BufferLen;
extern uint8_t rcv485DataBuffer[];
extern u8 localSubAddr;
void Init485(uint8_t localAddr,u32 baudrate);
void Send485(uint8_t *buffer,uint32_t bufflen);
void Task_Deal485(void);
void LogTo485(uint8_t *buffer,uint32_t buffLen);
void Register485Callback(u8 subAddr,u8 cmd,CALLBACK_485 callback);
void Make485Data(u8 *dataPackage,u8 funcCode,u8 *data);
#endif
