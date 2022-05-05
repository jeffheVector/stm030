#ifndef __OLEDFONT_H
#define __OLEDFONT_H 
#include "port_sys.h"
//常用ASCII表
//偏移量32
//ASCII字符集
//偏移量32
//大小:12*6
#ifdef HDEP_GATHER
/************************************6*8的点阵************************************/
 /*  (0) A(1) B(2) M(3) N(4) 0(5) 1(6) 2(7)
 3(8) 4(9) 5(10) 6(11) 7(12) 8(13) 9(14) :(15)
 =(16) >(17)*/
//#define CHAR_SPACE 0
//#define CHAR_A 1
//#define CHAR_B 2
//#define CHAR_M 3
//#define CHAR_N 4
//#define CHAR_0 5
//#define CHAR_1 6
//#define CHAR_2 7
//#define CHAR_9 14
//#define CHAR_POINT 15
//#define CHAR_COLON 16
//#define CHAR_EQUAL 17
//#define CHAR_SEPARATOR 18
//#define CHAR_LEFTBRACKET 19
//#define CHAR_RIGHTBRACKET 20
#define CHARFONT_WIDTH 24
#define CHARFONT_HEIGHT 48
#define CHAR_MAXNUM 18
extern u8 abmnChars[];
extern u8 abmnFont[];
extern unsigned char hdep_logo[];
#endif
#endif


