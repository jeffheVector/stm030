#ifndef __ABMN_H
#define __ABMN_H
#include "port_sys.h"
#define HDEPCMD485_ABMN 0x80//0xFE 0x80 0x01 0x03 0x01 0x02 CRCX//四个字节表示，A,M,N,B分别接那个电极，0标识断开
void ABMN_Init(u8 localAddr);
void ABMN_ShowInfo(void);
#endif
