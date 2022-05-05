#include "abmn.h"
#include "lcd.h"
#include "lcdfont.h"
#include "gwtask_timer.h"
#include "485.h"
#ifdef HDEP_GATHER
#define MAXRELAY_NUM 16
#define MAXABMNBUFF_LEN 8
#define E1_9_PORT GPIOA
#define E1_9_PIN GPIO_PIN_8
#define E2_10_PORT GPIOA
#define E2_10_PIN GPIO_PIN_11
#define E3_11_PORT GPIOA
#define E3_11_PIN GPIO_PIN_12
#define E4_12_PORT GPIOA
#define E4_12_PIN GPIO_PIN_15
#define E5_13_PORT GPIOB
#define E5_13_PIN GPIO_PIN_3
#define E6_14_PORT GPIOB
#define E6_14_PIN GPIO_PIN_4
#define E7_15_PORT GPIOB
#define E7_15_PIN GPIO_PIN_5
#define E8_16_PORT GPIOB
#define E8_16_PIN GPIO_PIN_6
#define ABMN_CLR_PORT GPIOF
#define ABMN_CLR_PIN GPIO_PIN_1
#define ABMN_CLK_PORT GPIOF
#define ABMN_CLK_PIN GPIO_PIN_0
#define ABMN_D_PORT GPIOB
#define ABMN_D_PIN GPIO_PIN_7
static u8 abmnStates[MAXABMNBUFF_LEN];//1_9;2_10
#define ALOW_BITMASK 0X80//1-8
#define NLOW_BITMASK 0X40
#define BLOW_BITMASK 0X20
#define MLOW_BITMASK 0X10
#define AHIGH_BITMASK 0X01//9-16
#define NHIGH_BITMASK 0X02
#define BHIGH_BITMASK 0X04
#define MHIGH_BITMASK 0X08
//#define ABMNINDEX8_16 7U
//#define ABMNINDEX7_15 6U
//#define ABMNINDEX6_14 5U
//#define ABMNINDEX5_13 4U
//#define ABMNINDEX4_12 3U
//#define ABMNINDEX3_11 2U
//#define ABMNINDEX2_10 1U
//#define ABMNINDEX1_9 0U
//static u16 abmnPortPins[MAXABMNBUFF_LEN];
//static GPIO_TypeDef * abmnPorts[MAXABMNBUFF_LEN];
static u16 relayPortPins[MAXABMNBUFF_LEN];
static GPIO_TypeDef * relayPorts[MAXABMNBUFF_LEN];
static u8 abmnRelayNos[4];//保存当前ABMN链接对象,0表示没有链接，1-16表示有

#define A_TYPE 3
#define B_TYPE 1
#define M_TYPE 0
#define N_TYPE 2
static void TestABMN(void);
static u16 beginRelayNo,endRelayNo;
static void SetABMNPortSate(u8 abmnIndex,u8 abmnState);
static void ConnectABMNRelayNos(u16 ano,u16 bno,u16 mno,u16 nno);
static void DisAllConnectRelay(void);
static void SetABMNPortSate(u8 abmnIndex,u8 abmnState);
void ABMN_ShowInfo(void)
{
	LCD_ShowBmp565(hdep_logo,0,0,48,48);
	uint8_t tx_buf[10];
	memset(tx_buf,0,11);
	ConvertLCDNum(beginRelayNo,tx_buf,4);
    LCD_ShowABMNString(2*CHARFONT_WIDTH, 0,tx_buf, 0, WHITE);
//	memset(tx_buf,0,11);
//	tx_buf[0]='=';
//	tx_buf[1]='>';
//	LCD_ShowABMNString(4*CHARFONT_WIDTH,0,tx_buf,0,BLUE);
	memset(tx_buf,0,11);
	ConvertLCDNum(endRelayNo,tx_buf,4);
    LCD_ShowABMNString(6*CHARFONT_WIDTH, 0, tx_buf, 0, YELLOW);
	memset(tx_buf,0,11);
	tx_buf[0]=' ';
	tx_buf[1]='A';
	tx_buf[2]='>';
	tx_buf[3]=' ';
	for(int i=4;i<10;i++)
	{
		tx_buf[i]=' ';
	}
	LCD_ShowABMNString(0, CHARFONT_HEIGHT, tx_buf, 0, RED);
	memset(tx_buf,0,11);
	tx_buf[0]=' ';
	tx_buf[1]='M';
	tx_buf[2]='>';
	tx_buf[3]=' ';
	for(int i=4;i<10;i++)
	{
		tx_buf[i]=' ';
	}
	LCD_ShowABMNString(0, 2*CHARFONT_HEIGHT, tx_buf, 0, GREEN);
	memset(tx_buf,0,11);
	tx_buf[0]=' ';
	tx_buf[1]='N';
	tx_buf[2]='>';
	tx_buf[3]=' ';
	for(int i=4;i<10;i++)
	{
		tx_buf[i]=' ';
	}
	LCD_ShowABMNString(0, 3*CHARFONT_HEIGHT, tx_buf, 0, GREEN);
	memset(tx_buf,0,11);
	tx_buf[0]=' ';
	tx_buf[1]='B';
	tx_buf[2]='>';
	tx_buf[3]=' ';
	for(int i=4;i<10;i++)
	{
		tx_buf[i]=' ';
	}
	LCD_ShowABMNString(0, 4*CHARFONT_HEIGHT, tx_buf, 0, GRAY);
}

static void DisplayABMNConnInfo(u8 abmnType,u8 relayNo)
{
	u16 y=0;
	u16 color=BLACK;
	switch(abmnType)
	{
		case A_TYPE:
			y=CHARFONT_HEIGHT;
			color=RED;
			break;
		case B_TYPE:
			y=4*CHARFONT_HEIGHT;
			color=GRAY;
			break;
		case M_TYPE:
			y=2*CHARFONT_HEIGHT;
			color=GREEN;
			break;
		case N_TYPE:
			y=3*CHARFONT_HEIGHT;
			color=GREEN;
			break;
	}
	u16 x=5*CHARFONT_WIDTH;
	relayNo=relayNo+beginRelayNo-1;
	char txtbuf[10]={0};
	sprintf(txtbuf,"%d",relayNo);
	LCD_ShowABMNString(x, y, (u8 *)txtbuf, 0, color);
}

static void ClearABMNConnInfo(u8 abmnType)
{
	u16 y=0;
	u16 color=BLACK;
	switch(abmnType)
	{
		case A_TYPE:
			y=CHARFONT_HEIGHT;
			color=RED;
			break;
		case B_TYPE:
			y=4*CHARFONT_HEIGHT;
			color=GREEN;
			break;
		case M_TYPE:
			y=2*CHARFONT_HEIGHT;
			color=GREEN;
			break;
		case N_TYPE:
			y=3*CHARFONT_HEIGHT;
			color=GRAY;
			break;
	}
	u16 x=4*CHARFONT_WIDTH;
	//relayNo=relayNo+beginRelayNo-1;
	char txtbuf[5]={0};
	sprintf(txtbuf,"    ");
	LCD_ShowABMNString(x, y, (u8 *)txtbuf, 0, color);
}

static void Callback_SetABMN(u8 *data)
{
	u8 ano,bno,mno,nno;
	ano=data[2];
	bno=data[3];
	mno=data[4];
	nno=data[5];
	ConnectABMNRelayNos(ano,bno,mno,nno);
}
static void AMBN_Port_Init(void)
{
	Register485Callback(HDEPCMD485_ABMN,Callback_SetABMN);
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();	 //使能B端口时钟
	 __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	for(int i=0;i<MAXRELAY_NUM/2;i++)
	{
		GPIO_InitStruct.Pin = relayPortPins[i];
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;//速度50MHz
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(relayPorts[i], &GPIO_InitStruct);
	}
	for(int i=0;i<MAXRELAY_NUM/2;i++)
	{
		Set0(relayPorts[i],relayPortPins[i]);
	}
	GPIO_InitStruct.Pin = ABMN_CLK_PIN|ABMN_CLR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;//速度50MHz
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ABMN_CLK_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ABMN_D_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;//速度50MHz
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ABMN_D_PORT, &GPIO_InitStruct);

}

void ABMN_Init(u8 localAddr)
{
	if(localAddr==0)
	{
		beginRelayNo=0;
		endRelayNo=0;
	}
	else
	{
		beginRelayNo=(localAddr)*16+1;
		endRelayNo=(localAddr)*16+16;
	}
	AppMemSet(abmnStates,0,MAXABMNBUFF_LEN);
	relayPorts[0]=E1_9_PORT;
	relayPortPins[0]=E1_9_PIN;
	relayPorts[1]=E2_10_PORT;
	relayPortPins[1]=E2_10_PIN;
	relayPorts[2]=E3_11_PORT;
	relayPortPins[2]=E3_11_PIN;
	relayPorts[3]=E4_12_PORT;
	relayPortPins[3]=E4_12_PIN;
	relayPorts[4]=E5_13_PORT;
	relayPortPins[4]=E5_13_PIN;
	relayPorts[5]=E6_14_PORT;
	relayPortPins[5]=E6_14_PIN;
	relayPorts[6]=E7_15_PORT;
	relayPortPins[6]=E7_15_PIN;
	relayPorts[7]=E8_16_PORT;
	relayPortPins[7]=E8_16_PIN;
	abmnRelayNos[A_TYPE]=0;
	abmnRelayNos[B_TYPE]=0;
	abmnRelayNos[M_TYPE]=0;
	abmnRelayNos[N_TYPE]=0;
	AMBN_Port_Init();
	//DisAllConnectRelay();
	for(int i=0;i<8;i++)
	{
		SetABMNPortSate(i,0);
	}
	TestABMN();	
}

static void SetABMNState(u8 abmnState)
{
	Set0(ABMN_CLR_PORT,ABMN_CLR_PIN);
	Set0(ABMN_CLK_PORT,ABMN_CLK_PIN);
	Delay_ms(1);
	Set1(ABMN_CLR_PORT,ABMN_CLR_PIN);
	
	u8 setState=0x01;
	for(u8 i=0;i<8;i++)
	{
		
		if(abmnState & setState)
		{
			//PRINTF("D%d 1\r\n",i);
			Set1(ABMN_D_PORT,ABMN_D_PIN);
		}
		else
		{
			//PRINTF("D%d 0\r\n",i);
			Set0(ABMN_D_PORT,ABMN_D_PIN);
		}
		setState<<=1;
		Delay_ms(1);
		Set1(ABMN_CLK_PORT,ABMN_CLK_PIN);
		Delay_ms(1);
		Set0(ABMN_CLK_PORT,ABMN_CLK_PIN);
		Delay_ms(1);
	}
}

static void SetABMNPortSate(u8 abmnIndex,u8 abmnState)
{
	PRINTF("SetABMNPortSate %d %X\r\n",abmnIndex,abmnState);
	SetABMNState(abmnState);
	Delay_ms(1);
	Set1(relayPorts[abmnIndex],relayPortPins[abmnIndex]);//拉高传输数据
	Delay_ms(1);
	Set0(relayPorts[abmnIndex],relayPortPins[abmnIndex]);//锁定数据
	Delay_ms(1);
}

/*
	序号从1开始
*/
static void CalcRelayToABMN(u8 relayNo,u8 abmnType,u8 *pABMNIndex,u8 *pABMNState)
{
	relayNo--;
	u8 abmnIndex=relayNo>=8?relayNo-8:relayNo;
	u8 abmnState=1U;//<<abmnType;
	if(relayNo<8)
	{
		//abmnState=abmnState<<4;
		switch(abmnType)
		{
			case A_TYPE:
				abmnState=ALOW_BITMASK;
			break;
			case B_TYPE:
				abmnState=BLOW_BITMASK;
			break;
			case M_TYPE:
				abmnState=MLOW_BITMASK;
			break;
			case N_TYPE:
				abmnState=NLOW_BITMASK;
			break;
		}
	}
	else
	{
		switch(abmnType)
		{
			case A_TYPE:
				abmnState=AHIGH_BITMASK;
			break;
			case B_TYPE:
				abmnState=BHIGH_BITMASK;
			break;
			case M_TYPE:
				abmnState=MHIGH_BITMASK;
			break;
			case N_TYPE:
				abmnState=NHIGH_BITMASK;
			break;
		}
	}
	*pABMNIndex=abmnIndex;
	*pABMNState=abmnState;
}


static void ConnectRelay(u8 relayNo,u8 abmnType)
{
	u8 abmnIndex,abmnState;
	CalcRelayToABMN(relayNo,abmnType,&abmnIndex,&abmnState);
	abmnStates[abmnIndex]=abmnStates[abmnIndex] | abmnState;
	SetABMNPortSate(abmnIndex,abmnStates[abmnIndex]);
	//abmnRelayNos[abmnType]=relayNo;
	PRINTF("ABMNINDEX %d STATE %02X\r\n",abmnIndex,abmnState);
}

static void DisConnectRelay(u8 relayNo,u8 abmnType)
{
	u8 abmnIndex,abmnState;
	CalcRelayToABMN(relayNo,abmnType,&abmnIndex,&abmnState);
	abmnStates[abmnIndex]=abmnStates[abmnIndex] & (~abmnState);
	SetABMNPortSate(abmnIndex,abmnStates[abmnIndex]);
	//abmnRelayNos[abmnType]=0;
}

static void DisAllConnectRelay(void)
{
	for(u8 abmnIndex=0;abmnIndex<8;abmnIndex++)
	{
		
		if(abmnStates[abmnIndex]!=0)
		{
			abmnStates[abmnIndex]=0;
			SetABMNPortSate(abmnIndex,0);
		}
	}
}

static u8 testRelayNo=1;
static u8 testAbmnType;
static void TestConnABMN(void);
static void TestConnectSingle(void);
static void TestDisConnectSingle(void)
{
//	u8 abmnIndex,abmnState;
//	CalcRelayToABMN(testRelayNo,testAbmnType,&abmnIndex,&abmnState);
//	//PRINTF("DISCONNECT %d<=>%d ABMN:%d<=>%d\r\n",testRelayNo,testAbmnType,abmnIndex,abmnState);
//	SetABMNPortSate(abmnIndex,0);
	DisConnectRelay(testRelayNo,testAbmnType);
	ClearABMNConnInfo(testAbmnType);
	testRelayNo++;
	if(testRelayNo>16)
	{
		testAbmnType++;
		testRelayNo=1;
	}
	if(testAbmnType>3)// && testAbmnType>=4)
	{
		testRelayNo=1;
		GWTaskTimer_SetTimer(1000,true,TestConnABMN);
		return;
	}
	GWTaskTimer_SetTimer(500,true,TestConnectSingle);
}

static void TestConnectSingle(void)
{
//	u8 abmnIndex,abmnState;
//	CalcRelayToABMN(testRelayNo,testAbmnType,&abmnIndex,&abmnState);
	//PRINTF("CONNECT %d<=>%d ABMN:%d<=>%d\r\n",testRelayNo,testAbmnType,abmnIndex,abmnState);
	DisplayABMNConnInfo(testAbmnType,testRelayNo);
	//SetABMNPortSate(abmnIndex,abmnState);
	ConnectRelay(testRelayNo,testAbmnType);
	GWTaskTimer_SetTimer(500,true,TestDisConnectSingle);
}
#define INDEX_UNKWON 0xFF
static void AddExistsIndex(u8 relayIndexs[],u8 relayIndex)
{
	for(int i=0;i<4;i++)
	{
		if(relayIndexs[i]==relayIndex)
			return;
	}
	for(int i=0;i<4;i++)
	{
		if(relayIndexs[i]==INDEX_UNKWON)
		{
			relayIndexs[i]=relayIndex;
			return;
		}
	}
}
static void ConnectABMNRelayNos(u16 ano,u16 bno,u16 mno,u16 nno)
{
	PRINTF("begin ConnectABMNRelayNos:\r\n");
	u8 relayIndexs[4];
	for(int i=0;i<4;i++)
	{
		relayIndexs[i]=INDEX_UNKWON;
	}
	if(ano==0)
	{
		if(abmnRelayNos[A_TYPE]!=0)
		{
			ClearABMNConnInfo(A_TYPE);
			DisConnectRelay(abmnRelayNos[A_TYPE],A_TYPE);
			abmnRelayNos[A_TYPE]=0;
		}
	}
	else
	{
		if(abmnRelayNos[A_TYPE]!=0)
		{
			Error_Handler(ERRORCODE_HDEP_CONNECTRELAY);
		}
		else
		{
			DisplayABMNConnInfo(A_TYPE,ano);
			u8 rindex,rstate;
			CalcRelayToABMN(ano,A_TYPE,&rindex,&rstate);
			abmnStates[rindex]=abmnStates[rindex]|rstate;
			AddExistsIndex(relayIndexs,rindex);
		}
	}
	if(bno==0)
	{
		if(abmnRelayNos[B_TYPE]!=0)
		{
			ClearABMNConnInfo(B_TYPE);
			DisConnectRelay(abmnRelayNos[B_TYPE],B_TYPE);
			abmnRelayNos[B_TYPE]=0;
		}
	}
	else
	{
		if(abmnRelayNos[B_TYPE]!=0)
		{
			Error_Handler(ERRORCODE_HDEP_CONNECTRELAY);
		}
		else
		{
			DisplayABMNConnInfo(B_TYPE,bno);
			u8 rindex,rstate;
			CalcRelayToABMN(bno,B_TYPE,&rindex,&rstate);
			abmnStates[rindex]=abmnStates[rindex]|rstate;
			AddExistsIndex(relayIndexs,rindex);
		}
	}
	if(mno==0)
	{
		if(abmnRelayNos[M_TYPE]!=0)
		{
			ClearABMNConnInfo(M_TYPE);
			DisConnectRelay(abmnRelayNos[M_TYPE],M_TYPE);
			abmnRelayNos[M_TYPE]=0;
		}
	}
	else
	{
		if(abmnRelayNos[M_TYPE]!=0)
		{
			Error_Handler(ERRORCODE_HDEP_CONNECTRELAY);
		}
		else
		{
			DisplayABMNConnInfo(M_TYPE,mno);
			u8 rindex,rstate;
			CalcRelayToABMN(mno,M_TYPE,&rindex,&rstate);
			abmnStates[rindex]=abmnStates[rindex]|rstate;
			AddExistsIndex(relayIndexs,rindex);
		}
	}
	if(nno==0)
	{
		if(abmnRelayNos[N_TYPE]!=0)
		{
			ClearABMNConnInfo(N_TYPE);
			DisConnectRelay(abmnRelayNos[N_TYPE],N_TYPE);
			abmnRelayNos[N_TYPE]=0;
		}
	}
	else
	{
		if(abmnRelayNos[N_TYPE]!=0)
		{
			Error_Handler(ERRORCODE_HDEP_CONNECTRELAY);
		}
		else
		{
			DisplayABMNConnInfo(N_TYPE,nno);
			u8 rindex,rstate;
			CalcRelayToABMN(nno,N_TYPE,&rindex,&rstate);
			abmnStates[rindex]=abmnStates[rindex]|rstate;
			AddExistsIndex(relayIndexs,rindex);
		}
	}
	for(int i=0;i<4;i++)
	{
		if(relayIndexs[i]!=INDEX_UNKWON)
		{
			
			u8 abmnIndex=relayIndexs[i];
			SetABMNPortSate(abmnIndex,abmnStates[abmnIndex]);
			PRINTF("SET INDEX:%d,STATE:%X\r\n",abmnIndex,abmnStates[abmnIndex]);
			Delay_ms(1000);
		}
	}
	PRINTF("\r\nEND CONN ABMN\r\n");
//	u8 aindex,astate,bindex,bstate,mindex,mstate,nindex,nstate;
//	CalcRelayToABMN(testRelayNo,testAbmnType,&aindex,&astate);
}
static void TestDisConnectABMN(void)
{
	ConnectABMNRelayNos(0,0,0,0);
	testRelayNo++;
	GWTaskTimer_SetTimer(1000,true,TestConnABMN);
}

static void TestConnABMN(void)
{
//	if(testRelayNo>13)
//	{
//		return;
//	}
//	u16 ano=testRelayNo;
//	u16 bno=ano+3;
//	u16 mno=ano+1;
//	u16 nno=ano+2;
//	ConnectABMNRelayNos(ano,bno,mno,nno);
//	GWTaskTimer_SetTimer(1000,true,TestDisConnectABMN);
	//ConnectABMNRelayNos(1,2,3,4);
	//ConnectRelay(1,A_TYPE);
	//ConnectRelay(2,B_TYPE);
//	SetABMNPortSate(0,0x80);
//	SetABMNPortSate(1,0x20);
//	SetABMNPortSate(2,0x10);
//	SetABMNPortSate(3,0x40);
}

static void TestABMN(void)
{
//	u8 abmnIndex,abmnState;
//	for(int rn=15;rn<=16;rn++)
//	{
//		CalcRelayToABMN(rn,A_TYPE,&abmnIndex,&abmnState);
//		//PRINTF(" INDEX:%d ASTATE:%02X ",abmnIndex,abmnState);
//		SetABMNPortSate(abmnIndex,abmnState);//(7,0x80);//8-A
//		Delay_ms(500);
//		SetABMNPortSate(abmnIndex,0);
//		Delay_ms(500);
//		CalcRelayToABMN(rn,M_TYPE,&abmnIndex,&abmnState);
//		//PRINTF(" INDEX:%d ASTATE:%02X ",abmnIndex,abmnState);
//		SetABMNPortSate(abmnIndex,abmnState);//(7,0x80);//8-A
//		Delay_ms(500);
//		SetABMNPortSate(abmnIndex,0);
//		Delay_ms(500);
//		CalcRelayToABMN(rn,N_TYPE,&abmnIndex,&abmnState);
//		//PRINTF(" INDEX:%d ASTATE:%02X ",abmnIndex,abmnState);
//		SetABMNPortSate(abmnIndex,abmnState);//(7,0x80);//8-A
//		Delay_ms(500);
//		SetABMNPortSate(abmnIndex,0);
//		Delay_ms(500);
//		CalcRelayToABMN(rn,B_TYPE,&abmnIndex,&abmnState);
//		//PRINTF(" INDEX:%d ASTATE:%02X ",abmnIndex,abmnState);
//		SetABMNPortSate(abmnIndex,abmnState);//(7,0x80);//8-A
//		Delay_ms(500);
//		SetABMNPortSate(abmnIndex,0);
//		Delay_ms(500);
//	}
//	Delay_ms(1000);
//	SetABMNPortSate(abmnIndex,0);//(7,0x00);//断开
//	Delay_ms(1000);
//	SetABMNPortSate(7,0x40);//断开
//	Delay_ms(1000);
//	SetABMNPortSate(7,0x00);//断开

	//DisplayABMNConnInfo(A_TYPE,16);
	//ConnectRelay(16,A_TYPE);
	//ConnectABMNRelayNos(1,2,3,4);
	testRelayNo=1;
	testAbmnType=M_TYPE;//0
	GWTaskTimer_SetTimer(1000,true,TestConnABMN);//TestConnectSingle);
	//GWTaskTimer_SetTimer(1000,true,TestConnectSingle);
//	u8 abmnIndex,abmnState;
//	u8 relayNo=8;
//	for(relayNo=1;relayNo<=16;relayNo++)
//	{
//		PRINTF("NO %d:",relayNo);
//		CalcRelayToABMN(relayNo,A_TYPE,&abmnIndex,&abmnState);
//		PRINTF(" INDEX:%d ASTATE:%02X ",abmnIndex,abmnState);
//		CalcRelayToABMN(relayNo,B_TYPE,&abmnIndex,&abmnState);
//		PRINTF(" INDEX:%d BSTATE:%02X ",abmnIndex,abmnState);
//		CalcRelayToABMN(relayNo,M_TYPE,&abmnIndex,&abmnState);
//		PRINTF(" INDEX:%d MSTATE:%02X ",abmnIndex,abmnState);
//		CalcRelayToABMN(relayNo,N_TYPE,&abmnIndex,&abmnState);
//		PRINTF(" INDEX:%d NSTATE:%02X ",abmnIndex,abmnState);
//		PRINTF("\r\n");
//	}
}
#endif
