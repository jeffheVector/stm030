#ifndef __ABMN_H
#define __ABMN_H
#include "port_sys.h"
#define HDEPCMD485_ABMN 0x80//0xFE 0x80 0x01 0x03 0x01 0x02 CRCX//�ĸ��ֽڱ�ʾ��A,M,N,B�ֱ���Ǹ��缫��0��ʶ�Ͽ�
void ABMN_Init(u8 localAddr);
void ABMN_ShowInfo(void);
#endif
