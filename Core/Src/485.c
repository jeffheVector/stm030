#include "main.h"
#include "485.h"
#include "systick.h"
#include "myque.h"
#include "myarray.h"
#include "utils.h"
#include "mylist.h"
//#include "lcd.h"
//#include "lcdfont.h"
UART_HandleTypeDef huart1;
uint8_t rcv485BufferLen=1;
uint8_t rcv485DataBuffer[1];
static _myque *p485Que;
//static _myarray *p485Buffer;

//#define CMD_CONNECT 0x03//链接电极
////比如0x01 0x03 0x00 0x01 0x02 0x0A CRC1 CRC2//表示A不连接，B=>1,M=>2,N=>10，电极序号1-16
//#define CMD_DISCONNECTALL 0x04
//#define CMD_ACK 0x05
#define ADDR485_DEBUG 0xFF//发送调试信息用
#define ADDR485_BROADCAST 0xFE//485广播地址

u8 localSubAddr=0x80;
static u8 Ack485Buff[8]={0x80,0x05,0x00,0x00,0x00,0x00,0x00};
//static CALLBACK_485 Callback485Array[MAX485CMD_NUM];
static _mylist *p485CmdList;
static void MX_USART1_UART_Init(u32 baudrate)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = baudrate;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler(ERRORCODE_INITUART);
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

static void Init485EN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = _485EN_IO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(_485EN_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(_485EN_PORT, _485EN_IO_PIN, GPIO_PIN_RESET);
}


void Register485Callback(u8 subAddr,u8 cmd,u8 cmdLen,CALLBACK_485 callback)
{
	if(cmd>=MAX485CMD_NUM)
	{
		Error_Handler(ERRORCODE_REGISTER485CALLBACK);
		return;
	}
	_485_func_callback p485cb;//=AppMalloc(sizeof(_485_func_callback));
	p485cb.addr=subAddr;
	p485cb.cmd=cmd;
	p485cb.cmdLen=cmdLen;
	p485cb.callback=callback;
	MyList_AddNodePureData(p485CmdList,&p485cb,sizeof(_485_func_callback));
	//Callback485Array[cmd]=callback;
}

void Make485Data(u8 *dataPackage,u8 funcCode,u8 *data)
{
	dataPackage[0]=localSubAddr;
	dataPackage[1]=funcCode;
	u8 *pData=dataPackage+2;
	AppMemCpy(pData,data,4);
	u16 crcCode=GetQuickCRC16(dataPackage,6);
	dataPackage[6]=crcCode>>8;
	dataPackage[7]=crcCode&0xFF;
}


void Callback_DealSetDebugLog(u8 *pdata)
{
	DebugLogBy485=pdata[5];
}

void Init485(u8 localAddr,u32 baudrate)
{
	localSubAddr=localAddr;
	Ack485Buff[0]=localAddr;
	u16 crcCode=GetQuickCRC16(Ack485Buff,6);
	Ack485Buff[6]=crcCode>>8;
	Ack485Buff[7]=crcCode&0xFF;
	p485Que=MyQue_Init(16,1);
	//p485Buffer=MyArray_Create(32);
	MX_USART1_UART_Init(baudrate);
	Init485EN();
	HAL_UART_Receive_IT(&huart1,rcv485DataBuffer,rcv485BufferLen);
	p485CmdList=MyList_Create();
	//Register485Callback(localAddr, CMD485_SETDEBUGLOG,Callback_DealSetDebugLog);
	#ifdef DEBUG
	#ifdef HDEP_GATHER
	PRINTF("HDEP 485ADDR:%d,BAUDRATE:%d\r\n",localAddr,baudrate);
	#endif
	#ifdef MOTOR485
	PRINTF("MOTOR 485ADDR:%d,BAUDRATE:%d\r\n",localAddr,baudrate);
	#endif
	#endif
}

void LogTo485(uint8_t *buffer,uint32_t buffLen)
{
//	#define MAXLOGCODE_LEN 16
//	#define MAXLOGDATA_LEN 12
//	u8 debugInfo[MAXLOGCODE_LEN];
//	debugInfo[0]=ADDR485_DEBUG;
//	debugInfo[1]=localSubAddr;
//	u8 *pBuffer=debugInfo;
//	pBuffer+=2;
//	//char test[64]={0};
//	//sprintf(test,"BUFFLEN:%d\r\n",buffLen);
//	//Send485((u8 *)test,strlen(test));
//	do//(buffLen>7)
//	{
//		AppMemSet(pBuffer,0,MAXLOGDATA_LEN);
//		uint32_t sendLen=buffLen>MAXLOGDATA_LEN?MAXLOGDATA_LEN:buffLen;
//		AppMemCpy(pBuffer,buffer,sendLen);
//		u16 crcCode=GetQuickCRC16(debugInfo,MAXLOGCODE_LEN-2);
//		debugInfo[MAXLOGCODE_LEN-2]=crcCode>>8;
//		debugInfo[MAXLOGCODE_LEN-1]=crcCode&0xFF;
//		Send485(debugInfo,MAXLOGCODE_LEN);
//		buffLen-=sendLen;
//		buffer+=sendLen;
//	}while(buffLen>0); 
//	u8 *pSend=AppMalloc(buffLen+4);
//	AppMemSet(pSend,0,buffLen+4);
//	pSend[0]=ADDR485_DEBUG;
//	pSend[1]=localSubAddr;
//	AppMemCpy(pSend+2,buffer,buffLen);
//	u16 crcCode=GetQuickCRC16(pSend,buffLen+2);
//	pSend[buffLen+2]=crcCode>>8;
//	pSend[buffLen+3]=crcCode&0xFF;
//	Send485(pSend,buffLen+4);
//	AppFree(pSend);
	Send485(buffer,buffLen);
}

void Send485(uint8_t *buffer,uint32_t bufflen)
{
	//return;
	HAL_GPIO_WritePin(_485EN_PORT, _485EN_IO_PIN, GPIO_PIN_SET);
	Delay_ms(10);
//	for(u32 i=0;i<bufflen;i++)
//	{
//		HAL_UART_Transmit(&huart1, (u8 *)(buffer+i),1,50*bufflen);// bufflen);//, 50*bufflen);
//	}
	HAL_UART_Transmit(&huart1, (u8 *)(buffer),bufflen,50*bufflen);
	Delay_ms(10);
	HAL_GPIO_WritePin(_485EN_PORT, _485EN_IO_PIN, GPIO_PIN_RESET);
}
uint32_t testcount=0;
static void Timer_Deal485Data(void)
{
	
}

static _485_func_callback *Find485DealCallback(u8 subAddr,u8 cmd)
{
	for(int i=0;i<p485CmdList->count;i++)
	{
		_485_func_callback *p485cb=MyList_GetNode(p485CmdList,i);
		if(p485cb->addr==subAddr && p485cb->cmd==cmd)
		{
			return p485cb;
		}
	}
	return NULL;
}

//static CALLBACK_485 Find485DealCallback(u8 subAddr,u8 cmd)
//{
//	for(int i=0;i<p485CmdList->count;i++)
//	{
//		_485_func_callback *p485cb=MyList_GetNode(p485CmdList,i);
//		if(p485cb->addr==subAddr && p485cb->cmd==cmd)
//		{
//			return p485cb->callback;
//		}
//	}
//	return NULL;
//}

void Task_Deal485(void)
{
//	testcount++;
//	u8 recvBuff[16];
//	if(p485Que->size>=5)
//	{
//		u16 recvLen;
//		MyQue_Fetch(p485Que,recvBuff,&recvLen);
//		
//		if(recvLen>0)
//		{
////			char buffinfo[10]={0};
////			sprintf(buffinfo,"REV:%d\r\n",recvLen);
////		Send485((uint8_t *)buffinfo,strlen(buffinfo));
////			 uint8_t tx_buf[10];
////    tx_buf[0] = 'K';
////    tx_buf[1] = '6';
////    tx_buf[2] = 'T';
////    tx_buf[3] = '6';
////    tx_buf[4] = 'O';
////    tx_buf[5] = 0;
////	Send485(tx_buf,5);
//			Send485(recvBuff,recvLen);
//		}
//	}
//	else
//	{
//		if(testcount%1000==0)
//		{
//			//uint8_t test[3]={'a','b','c'};
//			//Send485(test,3);
//		}
//	}
	while(!MyQue_IsEmpty(p485Que))
	{
		if(MyQue_Size(p485Que)<=4)
		{
			continue;
		}
		
		//PRINTF("485SIZE:%d\r\n",p485Que->size);
		u8 *pFirstByte=MyQue_DataAt(p485Que,0);
		u8 *pFunCode=MyQue_DataAt(p485Que,1);
		_485_func_callback *p485cb=Find485DealCallback(*pFirstByte,*pFunCode);
		//if(pFirstByte
		if(p485cb!=NULL)//IsDealSubAddr(*pFirstByte))//==localSubAddr)
		{
			//如果第一个是本机地址，尝试处理
			if(MyQue_Size(p485Que)>=p485cb->cmdLen)
			{
				u8 recvBuff[16];
				if(MyQue_TryGet(p485Que,recvBuff,p485cb->cmdLen))
				{
					//尝试处理这几个字节
					u16 crcCode=GetQuickCRC16(recvBuff,p485cb->cmdLen-2);
					u8 crcCode1=crcCode>>8;
					u8 crcCode2=crcCode&0xFF;
					if(crcCode1==recvBuff[p485cb->cmdLen-2] && crcCode2==recvBuff[p485cb->cmdLen-1])
					{
						CALLBACK_485 callback=p485cb->callback;// Find485DealCallback(*pFirstByte,recvBuff[1]);
						if(callback!=NULL)
						{
							callback(recvBuff);
						}
						//已经处理一个完整包，从缓冲区删除
						MyQue_Remove(p485Que,p485cb->cmdLen,NULL);
					}
					else
					{
						//删除第一个字节，继续处理
						MyQue_Remove(p485Que,1,NULL);
					}
				}
				else
				{
					//严重错误
					MyQue_Remove(p485Que,MyQue_Size(p485Que),NULL);
				}
			}
			else
			{
				//还不够数据数量，继续等待
				break;
			}
		}
		else
		{
			//舍弃第一个字节数据
			MyQue_Remove(p485Que,1,NULL);
		}
	}
//	if(p485Que->size>=MAX485PKG_LEN)
//	{
//		u8 recvBuff[16];
//		uint16_t recvLen=p485Que->size;
//		MyQue_Fetch(p485Que,recvBuff,&recvLen);
//		
//		#ifdef DEBUG
//		PRINTF("rcv:%d;%d: %02X\r\n",p485Que->size,recvLen,recvBuff[0]);//,recvBuff[6],recvBuff[7]);
////		char info485[64]={0};
////		char *pInfo=info485;
////		sprintf(pInfo,"RECV:");
////		pInfo+=5;
////		for(int i=0;i<recvLen;i++)
////		{
////			sprintf(pInfo,"%02X ",recvBuff[i]);
////			//PRINTF("%02X ",recvBuff[i]);
////			pInfo+=3;
////		}
////		PRINTF("%s\r\n",info485);
//		#endif
//		u16 crcCode=GetQuickCRC16(recvBuff,recvLen-2);
//		u8 crcCode1=crcCode>>8;
//		u8 crcCode2=crcCode&0xFF;
//		//PRINTF("CRC:%X %X\r\n",crcCode,crcCode1,crcCode2);
//		if(crcCode1==recvBuff[recvLen-2] && crcCode2==recvBuff[recvLen-1])
//		{
//			//PRINTF("OK\r\n");
//			u8 addr=recvBuff[0];
//			//PRINTF("OK:%X\r\n",addr);
//			if(addr<ADDR485_BROADCAST)
//			{
//				if(addr==localSubAddr)
//				{
//					if(Callback485Array[recvBuff[1]]!=NULL)
//					{
//						Callback485Array[recvBuff[1]](recvBuff);
//					}
//				}
//			}
//		}
//	}
	//Delay_ms(1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	//uint8_t ch;
	for(int i=0;i<rcv485BufferLen;i++)
		MyQue_AddByte(p485Que,rcv485DataBuffer[i]);
	while(HAL_UART_Receive_IT(&huart1, rcv485DataBuffer, rcv485BufferLen) != HAL_OK);
}
