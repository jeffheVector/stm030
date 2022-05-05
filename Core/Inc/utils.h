#ifndef __UTILS_H
#define __UTILS_H
#include "port_sys.h"

void getu8Str(u8 u, char *str);
bool CheckInputIPAddr(char *value);
bool CheckInputInteger(char *value);
bool CheckInputNumber(char *value);
bool CheckInputPositiveInteger(char *value);
bool CheckInputFixedReal(char *value,int fixedPoint);
bool CheckInputPositiveFixedReal(char *value,int fixedPoint);
bool CheckInputPositiveReal(char *value);
bool CheckInputReal(char *value);
bool CheckInputMobileNo(char *value);
int nsqrt(int n);
void GetLngLat(double ll,int *d,int *m,int *s);
double SetLngLat(int d,int m,int s);
u8 CRCAnd(u8 *data,u32 len);
u8 CRCXor(u8 *data,u32 len);
unsigned short GetQuickCRC16(unsigned char * pBuffer, int Length);
int strIndexOf(char *str,int strLen,char *subStr,int substrLen,int beginIndex);
void strTrim(char *str);
int hexToI(char s[],int len);
void iToHex(int i,char *pHex,int index,int maxCode);
u8 BCDByteToInt(u8 byte);
u8 IntToBCDByte(u8 bcdValue);
#endif
