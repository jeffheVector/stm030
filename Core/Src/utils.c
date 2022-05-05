#include "utils.h"
u8 BCDByteToInt(u8 byte)
{
	u8 bcdHigh=(byte&0xF0)>>4;
	return bcdHigh*10+(byte &0x0F);
}

u8 IntToBCDByte(u8 bcdValue)
{
	u8 byte=bcdValue/10;
	byte=(byte<<4)+bcdValue%10;
	return byte;
}

static char HexTable[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void iToHex(int i,char *pHex,int index,int maxCode)
{
	//uint32_t ui=(uint32_t)i;
	int lastCharIndex=index+maxCode-1;
	
	
	while(lastCharIndex>=index)
	{
		uint8_t j=i&0x000F;
		pHex[lastCharIndex]=HexTable[j];
		i=i>>4;
		lastCharIndex--;
	}
}

int toLower(int c)  
{  
    if (c >= 'A' && c <= 'Z')  
    {  
        return c + 'a' - 'A';  
    }  
    else  
    {  
        return c;  
    }  
} 
int hexToI(char s[],int len)  
{  
    int i=0;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    //for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >='A' && s[i] <= 'F');++i)  
	for(;i<len;i++)
    {  
        if (s[i] >= 'a' && s[i] <= 'z')  
        {  
            n = 16 * n + (10 +s[i] - 'a');  
        }  
        else  if (s[i] >='A' && s[i] <= 'F')  
        {  
            n = 16 * n + (10 + s[i] - 'A');  
        }
		else if(s[i] >= '0' && s[i] <= '9')
        {  
            n = 16 * n + s[i] - '0';  
        }  
    }  
    return n;  
}  

void strTrim(char *str)
{
	int i=0;
	int len=strlen(str);
	while(i<len)
	{
		if(str[i]==' ')
		{
			for(int j=i;j<len;j++)
			{
				str[j]=str[j+1];
			}
			len--;
		}
		else
		{
			i++;
		}
	}
}

int strIndexOf(char *str,int strLen,char *subStr,int substrLen,int beginIndex)
{
	char *pBuff=str+beginIndex;
	while(beginIndex+substrLen<=strLen)
	{
		if(strncmp(pBuff,subStr,substrLen)==0)
		{
			return beginIndex;
		}
		beginIndex++;
		pBuff=str+beginIndex;
	}
	return -1;
}
// 逆序CRC表

unsigned char aucCRCHi[]={

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

       0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

       0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

       0x00, 0xC1, 0x81, 0x40

};

unsigned char aucCRCLo[]={

       0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,

       0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,

       0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,

       0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,

       0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,

       0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,

       0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,

       0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,

       0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,

       0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,

       0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,

       0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,

       0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,

       0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,

       0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,

       0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,

       0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,

       0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,

       0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,

       0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,

       0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,

       0x41, 0x81, 0x80, 0x40
};

unsigned short GetQuickCRC16(unsigned char * pBuffer, int Length) 
{

       unsigned char CRCHi = 0xFF;

       unsigned char CRCLo = 0xFF;

       unsigned char iIndex = 0;

       for (int i = 0; i < Length; i++)
       {
              iIndex = CRCHi ^ pBuffer[i];

              CRCHi = CRCLo ^ aucCRCHi[iIndex];

              CRCLo = aucCRCLo[iIndex];
       }

       return (unsigned int)( CRCHi << 8 | CRCLo);// CRC校验返回值 
}

u8 CRCAnd(u8 *data, u32 len)
{
    if(len == 1)
        return data[0];

    len = len - 1;
    u8 d = data[len--];

    while(len--)
    {
        d = d & data[len];
    }

    return d;
//	u8 d=data[0];
//	for(int i=1;i<len;i++)
//	{
//		d=d & data[i];
//	}
//	return d;
}

u8 CRCXor(u8 *data, u32 len)
{
    if(len == 1)
        return data[0];

    len = len - 1;
    u8 d = data[len--];

    while(len--)
    {
        d = d ^ data[len];
    }
	return d;
//	u8 d=data[0];
//	for(int i=1;i<len;i++)
//	{
//		d=d ^ data[i];
//	}
//    return d;
}

void GetLngLat(double ll, int *d, int *m, int *s)
{
    *d = (int)ll;
    ll = ll - *d;
    ll = ll * 60;
    *m = (int)ll;
    ll = ll - *m;
    ll = ll * 60;
    *s = (int)ll;
}
double SetLngLat(int d, int m, int s)
{
    return d * 1.0 + m / 60.0 + s / 3600.0;
}
int nsqrt(int n)
{

    if(n < 0)
    {
        return 0;
    }

    if(n == 0)
    {
        return 0;
    }

    if(n == 1)
    {
        return 1;
    }

    int i;

    for(i = n / 2; i >= 1; i--)
    {
        if(i * i <= n)
        {
            return i;
        }
        else
        {
            i--;
        }
    }

    return 1;
}

void getu8Str(u8 u, char *str)
{
    u8 i = 0;
    u8 bits[8];
    char bitc[10];

    for(i = 0; i < 8; i++)
    {
        if(u & (1 << i))
        {
            bits[i] = 1;
        }
        else
        {
            bits[i] = 0;
        }

    }

    sprintf(str, "");

    for(i = 0; i < 8; i++)
    {
        sprintf(bitc, "%d", bits[i]);
        strcat(str, bitc);
    }
}

bool CheckInputIPAddr(char *value)
{
    char ch;
    int pnum = 0;
    int slen = strlen(value);
    char temp[10];
    AppMemSet(temp, 0, 10);
    int tempi = 0;
    int ipa;

    for(int i = 0; i < slen; i++)
    {
        ch = value[i];

        if(ch == '.')
        {
            ipa = atoi(temp);

            if(ipa < 0 || ipa > 255)
            {
                return false;
            }

            AppMemSet(temp, 0, 10);
            tempi = 0;
            pnum++;
        }
        else if(ch >= '0' && ch <= '9')
        {
            if(tempi >= 3)
            {
                return false;
            }

            temp[tempi] = ch;
            tempi++;
        }
        else
        {
            return false;
        }
    }

    ipa = atoi(temp);

    if(ipa < 0 || ipa > 255)
    {
        return false;
    }

    if(pnum != 3)
    {
        return false;
    }

    return true;
}

bool CheckInputPositiveInteger(char *value)
{
    int slen = strlen(value);

    if(slen == 0)
    {
        return false;
    }

    for(int i = 0; i < slen; i++)
    {
        if(value[i] < '0' || value[i] > '9')
        {
            return false;
        }
    }

    if(value[0] == '0')
    {
        if(slen > 1)
        {
            return false;//0111这样也算错误
        }
    }

    return true;
}
bool CheckInputNumber(char *value)
{
    int slen = strlen(value);

    if(slen == 0)
    {
        return false;
    }

    for(int i = 1; i < slen; i++)
    {
        if(value[i] < '0' || value[i] > '9')
        {
            return false;
        }
    }

    return true;
}
bool CheckInputInteger(char *value)
{
    int slen = strlen(value);

    if(slen == 0)
    {
        return false;
    }

    if(value[0] == '-')
    {
        if(slen < 2)
        {
            return false;
        }

        if(value[1] == '0' && slen == 2)
        {
            return false;
        }

        char temp[20];
        AppMemSet(temp, 0, 20);

        for(int i = 1; i < slen; i++)
        {
            temp[i - 1] = value[i];
        }

        return CheckInputPositiveInteger(temp);
    }
    else
    {
        return CheckInputPositiveInteger(value);
    }
}
//输入fixPoint位的实数
bool CheckInputPositiveFixedReal(char *value, int fixedPoint)
{
    //sprintf(errInfo,"请输入%d位小数的正实数",fixedPoint);
    int slen = strlen(value);

    if(slen < fixedPoint + 2)
    {
        return false;
    }

    if(value[0] == '0')
    {
        if(value[1] != '.')
        {
            return false;
        }
    }

    int pointIndex = -1;

    for(int i = 0; i < slen; i++)
    {
        if(value[i] == '.')
        {
            if(pointIndex >= 0)
            {
                return false;//多个点
            }

            pointIndex = i;
            //break;
        }
        else if(value[i] < '0' || value[i] > '9')
        {
            return false;
        }
    }

    if(pointIndex < 1)
    {
        return false;
    }

    if(slen - pointIndex - 1 != fixedPoint)
    {
        return false;
    }

    return false;
}
//检查输入是否是实数
bool CheckInputReal(char *value)
{
	int slen = strlen(value);
	if(slen==0)
		return false;
	int pointNum=0;
	for(int i=0;i<slen;i++)
	{
		if(!((value[i] >='0' &&  value[i] <= '9') || (value[i] == '.')))
		{
			return false;
		}
		if(value[i]=='.')
		{
			pointNum++;
		}
	}
	if(pointNum>1)
	{
		return false;
	}
//	float fvalue=atof(value);
//	//PRINTF("%s %f",value,fvalue);
//	if(fvalue==0)
//	{
//		//判断是不是输入的只有0和0.
//		for(int i=0;i<slen;i++)
//		{
//			if(value[i] !='0' || value[i] != '.')
//			{
//				return false;
//			}
//		}
//	}
	return true;
}

bool CheckInputPositiveReal(char *value)
{
	int slen = strlen(value);
	if(slen==0)
		return false;
	if(value[0] == '-')
		return false;
	return CheckInputReal(value);
}

bool CheckInputFixedReal(char *value, int fixedPoint)
{
    //sprintf(errInfo,"请输入%d位小数的正实数",fixedPoint);
    int slen = strlen(value);

    //char ninfo[30];
    if(slen < fixedPoint + 2)
    {
        return false;
    }

    if(value[0] == '-')
    {
        if(slen < fixedPoint + 3)
        {
            return false;
        }

        if(value[1] == '0' && slen == 2)
        {
            return false;
        }

        char temp[64];
        AppMemSet(temp, 0, 64);

        for(int i = 1; i < slen; i++)
        {
            temp[i - 1] = value[i];
        }

        return CheckInputPositiveFixedReal(temp, fixedPoint);
    }
    else
    {
        return CheckInputPositiveFixedReal(value, fixedPoint);
    }
}

bool CheckInputMobileNo(char *value)
{
    if(strlen(value) != 11)
    {
        return false;
    }

    if(value[0] != '1')
    {
        return false;
    }

    switch(value[1])
    {
        case '1':
        case '2':
            return false;
    }

    return true;
}
