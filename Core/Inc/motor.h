#ifndef __MOTOR_H
#define __MOTOR_H
#include "port_sys.h"
#include "485.h"
#include "pwm.h"
#define MOTORSTATUS_STOPPED 1
#define MOTORSTATUS_STOPPING 2
#define MOTORSTATUS_FORWARD 3
#define MOTORSTATUS_BACKWARD 4

#define MOTORLIMIT_IDLE 1
#define MOTORLIMIT_REACH 2
#define MOTORLIMIT_NOISE 3
#define MOTORLIMIT_WAITSTABLE 4

//#define MOTORLIMIT_IDLE 3
//#define MOTORLIMIT_REACH 4

#define MOTORSTART_FORWARD 1
#define MOTORSTART_BACKWARD 2
typedef struct
{
	u8 idleNum;
	u8 reachNum;
	u8 readNum;
}_limit_statenums;
typedef struct
{
	GPIO_TypeDef *PORT_HIGH,*PORT_LOW;
	u16 PIN_HIGH,PIN_LOW;
	u8 status;
	u8 pwmIndex;
	u8 highState;
	_limit_statenums highStateNums;
	_limit_statenums lowStateNums;
	u8 lowState;
}_motor_info;
#define MOTOR485CMD_LEN 8
#define MAXMOTOR_NUM 2
#define MOTORCMD485_STARTMOTOR 0X01 //0xFE 0x01 0x00 0x01 0x01 0x02 CRC1 CRC2//启动1号闸门，正转（1，2反转），启动时长2秒钟
#define MOTORCMD485_REPORTSTARTMOTOR 0X02
#define MOTORCMD485_STOPMOTOR 0x03 //0XFE 0x02 0x00 0x01 0x00 0x00 CRC1 CRC2//停止1号闸门，立即停止（0）
#define MOTORCMD485_REPORTSTOPMOTOR 0x04
#define MOTORCMD485_QUERYMOTORINFO 0x05//0XFE 0x03 0x01 0x00 0x00 0x00 CRC1 CRC2//查询1号闸门信息
#define MOTORCMD485_REPORTMOTORINFO 0x06//0xFE 0x06 0x01 0x03 0x01 0x02 CRCX
	//上报一号闸门信息，第一个字节为电机序号（0，1），后三个字节，闸门状态(1-4)，上限位1(1-2)，下限位(1-2)表示到了
void Motor_Init(void);
void Motor_Deal(void);
void Motor_GetStatus(u8 motorIndex,char *buffer,u8 maxLen);
bool Motor_StartForward(u8 motorIndex,u32 timeOut);
bool Motor_StartBackward(u8 motorIndex,u32 timeOut);
#endif
