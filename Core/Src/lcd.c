#include "lcd.h"
#include "main.h"
#include "lcdfont.h"
#include "port_sys.h"
#ifdef HDEP_GATHER
SPI_HandleTypeDef lcdspi;
static void Hardware_SPI_Init(void)
{
///////��ʼ��SPI�˿�////////////////
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI1,ENABLE);	 //ʹ��SPI2�˿�ʱ��


    lcdspi.Instance = SPI1;
    lcdspi.Init.Mode = SPI_MODE_MASTER;
    lcdspi.Init.Direction = SPI_DIRECTION_1LINE;
    lcdspi.Init.DataSize = SPI_DATASIZE_8BIT;
    lcdspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    lcdspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    lcdspi.Init.NSS = SPI_NSS_SOFT;
    lcdspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    lcdspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    lcdspi.Init.TIMode = SPI_TIMODE_DISABLE;
    lcdspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    lcdspi.Init.CRCPolynomial = 7;

    if(HAL_SPI_Init(&lcdspi) != HAL_OK)
    {
        Error_Handler(ERRORCODE_LCD_INIT);
    }

//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//	//SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
//	SPI_InitStructure.SPI_CRCPolynomial = 7;
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
//
//	SPI_Init(SPI1,&SPI_InitStructure);
//	//SPI_Init(SPI2,&amp;SPI_InitStructure);
//
//	SPI_Cmd(SPI1,ENABLE);
}

u16 BACK_COLOR;   //����ɫ


/******************************************************************************
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    u8 i;
    LCD_CS_Clr();

    for(i = 0; i < 8; i++)
    {
        LCD_SCLK_Clr();

        if(dat & 0x80)
            LCD_SDIN_Set();
        else
            LCD_SDIN_Clr();

        LCD_SCLK_Set();
        dat <<= 1;
    }

    LCD_CS_Set();
}
void LCD_SPIWrit_Bus(u8 dat)
{
	while(HAL_SPI_Transmit(&lcdspi, &dat, 1, 1000)!=HAL_OK);
}
/******************************************************************************
      ����˵����Ӳ��SPI
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_SPIWrit_Bus1(u8 dat)
{
    //while(!SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE));
    
    //SPI_I2S_SendData(SPI2,dat);
    //while(!SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE));
	 unsigned char i;
   //CS0 = 0;
   for(i=0; i<8; i++)
   {
      //LCD_SCLK_Set();//SCL = 0;//nop_(); _nop_();_nop_();_nop_();
	   LCD_SCLK_Clr();
      if((dat&0x80)==0x80)
	  {
		  //SDA = 1;
		  LCD_SDIN_Set();
		  //LCD_SDIN_Clr();
	  }
        else
		{
			LCD_SDIN_Clr();
			//LCD_SDIN_Set();
			//SDA = 0;
		}

	  //SCL = 0;_nop_(); _nop_();_nop_();_nop_();
      //SCL = 1;//_nop_(); _nop_();_nop_();_nop_();
		LCD_SCLK_Set();
	//   LCD_SCLK_Clr();

      dat <<= 1;
   }
//	//CS0 = 1;

}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    LCD_CS_Clr();
    LCD_DC_Clr();//д����
    //LCD_Writ_Bus(dat);
    LCD_SPIWrit_Bus(dat);

    LCD_CS_Set();
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
    LCD_CS_Clr();
    LCD_DC_Set();//д����
    //LCD_Writ_Bus(dat);
    LCD_SPIWrit_Bus(dat);
    //Delay_ms(1);
    LCD_CS_Set();
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    LCD_CS_Clr();
    LCD_DC_Set();//д����
    LCD_SPIWrit_Bus(dat >> 8);
    LCD_SPIWrit_Bus(dat);
    LCD_CS_Set();
}



/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{



    if(USE_HORIZONTAL == 0)
    {
        LCD_WR_REG(0x2a);//�е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//�е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//������д
    }
    else if(USE_HORIZONTAL == 1)
    {
        y1 = y1 + 40;
        y2 = y2 + 40;

        LCD_WR_REG(0x2a);//�е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//�е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//������д
    }
    else if(USE_HORIZONTAL == 2)
    {
        LCD_WR_REG(0x2a);//�е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//�е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//������д
    }
    else
    {
        LCD_WR_REG(0x2a);//�е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//�е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//������д
    }
}


void LCD_Port_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();	 //ʹ��B�˿�ʱ��
	 __HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = LCD_RST_PIN|LCD_CS_PIN|LCD_DA_PIN|LCD_BLK_PIN;	 //PB13-SCK PB15-SDA PB11-RESET PB12-CS PB14-RS
 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 		 //�������
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;//�ٶ�50MHz
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(LCD_DA_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_SCL_PIN|LCD_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(LCD_SCL_PORT, &GPIO_InitStruct);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOB,&GPIO_InitStructure);

    LCD_CS_Set();
}
/******************************************************************************
      ����˵����LCD��ʼ������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/

void Lcd_Init(void)
{
	LCD_Port_Init();
    Hardware_SPI_Init();
    LCD_CS_Set();
    LCD_RST_Set();
    LCD_DC_Set();

    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��D�˿�ʱ��
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
    //GPIO_Init(GPIOD, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOD,GPIO_Pin_2);

    LCD_RST_Clr();
    Delay_ms(20);
    LCD_RST_Set();
    Delay_ms(100);
    LCD_BLK_Set();

    LCD_WR_REG(0x11);
    Delay_ms(120);

    LCD_WR_REG(0x36); //MX, MY, RGB mode

    if(USE_HORIZONTAL == 0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL == 1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL == 2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x55);   //   5 565  6 666

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32); //Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x19); //GVDD=4.8V

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); //VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F); //0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);


    LCD_WR_REG(0x21);

    LCD_WR_REG(0x2A); //Column Address Set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00); //0
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0xEF); //239

    LCD_WR_REG(0x2B); //Row Address Set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00); //0
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0xEF); //239

    LCD_WR_REG(0x29);


}


/******************************************************************************
      ����˵����LCD��������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_Clear(u16 Color)
{
	#ifdef DEBUG
	PRINTF("BEGIN LCD_CLEAR %d %d\r\n",LCD_W,LCD_H);
	#endif
    u16 i, j;
    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);

    for(i = 0; i < LCD_W; i++)
    {
        for (j = 0; j < LCD_H; j++)
        {
            LCD_WR_DATA(Color);
        }
    }
	#ifdef DEBUG
	PRINTF("LCD_CLEAR %d %d\r\n",LCD_W,LCD_H);
	#endif
}

/******************************************************************************
      ����˵����LCD��ʾ��������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_DispBand(void)
{
    u16 i, j, k;
    unsigned int color[8] = {0x001f, 0x07e0, 0xf800, 0x07ff, 0xf81f, 0xffe0, 0x0000, 0xffff};

    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);

    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < LCD_H / 8; j++)
        {
            for(k = 0; k < LCD_W; k++)
            {
                LCD_WR_DATA(color[i]);
            }
        }
    }

    for(j = 0; j < LCD_H % 8; j++)
    {
        for(k = 0; k < LCD_W; k++)
        {
            LCD_WR_DATA(color[7]);
        }
    }


}

/******************************************************************************
      ����˵����LCD��ʾ�ҽ׺���
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_DispGrayHor16(void)
{
    u16 i, j, k;

    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);

    for(i = 0; i < LCD_H; i++)
    {
        for(j = 0; j < LCD_W % 8; j++)
        {
            LCD_WR_DATA(0);
        }

        for(j = 0; j < 16; j++)
        {
            for(k = 0; k < LCD_W / 16; k++)
            {
                LCD_WR_DATA8(((j * 2) << 3) | ((j * 4) >> 3));
                LCD_WR_DATA8(((j * 4) << 5) | (j * 2));
            }
        }
    }


}

/******************************************************************************
      ����˵����LCD��ʾѩ������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_DispSnow(void)
{
    u16 i, j;
    u16 dat;

    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
    dat = 0;

    for(i = 0; i < LCD_H; i++)
    {
        for(j = 0; j < LCD_W / 2; j++)
        {
            LCD_WR_DATA(dat);
            LCD_WR_DATA(~dat);
        }

        dat = ~dat;
    }
}

/******************************************************************************
      ����˵����LCD��ʾ���麯��
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_DispBlock(void)
{
    u16 i, j, k;
    k = LCD_H / 4;

    LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);

    for(i = 0; i < k; i++)
    {
        for(j = 0; j < LCD_W; j++)
        {
            LCD_WR_DATA(0x7bef);
        }
    }

    for(i = 0; i < k * 2; i++)
    {
        for(j = 0; j < LCD_W / 4; j++)
        {
            LCD_WR_DATA(0x7bef);
        }

        for(j = 0; j < LCD_W / 2; j++)
        {
            LCD_WR_DATA(0);
        }

        for(j = 0; j < LCD_W / 4; j++)
        {
            LCD_WR_DATA(0x7bef);
        }
    }

    for(i = 0; i < k; i++)
    {
        for(j = 0; j < LCD_W; j++)
        {
            LCD_WR_DATA(0x7bef);
        }
    }
}


/******************************************************************************
      ����˵����LCD��ʾ����
      ������ݣ�x,y   ��ʼ����
                index ���ֵ����
                size  �ֺ�
      ����ֵ��  ��
******************************************************************************/
//void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)
//{
//	u8 i,j;
//	u8 *temp,size1;
//	if(size==16){temp=Hzk16;}//ѡ���ֺ�
//	if(size==32){temp=Hzk32;}
//  LCD_Address_Set(x,y,x+size-1,y+size-1); //����һ�����ֵ�����
//  size1=size*size/8;//һ��������ռ���ֽ�
//	temp+=index*size1;//д�����ʼλ��
//	for(j=0;j<size1;j++)
//	{
//		for(i=0;i<8;i++)
//		{
//		 	if((*temp&(1<<i))!=0)//�����ݵĵ�λ��ʼ��
//			{
//				LCD_WR_DATA(color);//����
//			}
//			else
//			{
//				LCD_WR_DATA(BACK_COLOR);//������
//			}
//		}
//		temp++;
//	 }
//}
/******************************************************************************
      ����˵����LCD��ʾ����
      ������ݣ�x,y   ��ʼ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_Address_Set(x, y, x, y); //���ù��λ��
    LCD_WR_DATA(color);
}
/******************************************************************************
      ����˵����LCD��һ����ĵ�
      ������ݣ�x,y   ��ʼ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint_big(u16 x, u16 y, u16 color)
{
    LCD_Fill(x - 1, y - 1, x + 1, y + 1, color);
}
/******************************************************************************
      ����˵������ָ�����������ɫ
      ������ݣ�xsta,ysta   ��ʼ����
                xend,yend   ��ֹ����
      ����ֵ��  ��
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
    u16 i, j;
    LCD_Address_Set(xsta, ysta, xend, yend);   //���ù��λ��

    for(i = ysta; i <= yend; i++)
    {
        for(j = xsta; j <= xend; j++)LCD_WR_DATA(color); //���ù��λ��
    }
}
/******************************************************************************
      ����˵��������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //������������
    delta_y = y2 - y1;
    uRow = x1; //�����������
    uCol = y1;

    if(delta_x > 0)incx = 1; //���õ�������
    else if (delta_x == 0)incx = 0; //��ֱ��
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if(delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //ˮƽ��
    else
    {
        incy = -1;
        delta_y = -delta_x;
    }

    if(delta_x > delta_y)distance = delta_x; //ѡȡ��������������
    else distance = delta_y;

    for(t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color); //����
        xerr += delta_x;
        yerr += delta_y;

        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}
/******************************************************************************
      ����˵����������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}
/******************************************************************************
      ����˵������Բ
      ������ݣ�x0,y0   Բ������
                r       �뾶
      ����ֵ��  ��
******************************************************************************/
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    //int di;
    a = 0;
    b = r;

    while(a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color);       //3
        LCD_DrawPoint(x0 + b, y0 - a, color);       //0
        LCD_DrawPoint(x0 - a, y0 + b, color);       //1
        LCD_DrawPoint(x0 - a, y0 - b, color);       //2
        LCD_DrawPoint(x0 + b, y0 + a, color);       //4
        LCD_DrawPoint(x0 + a, y0 - b, color);       //5
        LCD_DrawPoint(x0 + a, y0 + b, color);       //6
        LCD_DrawPoint(x0 - b, y0 + a, color);       //7
        a++;

        if((a * a + b * b) > (r * r)) //�ж�Ҫ���ĵ��Ƿ��Զ
        {
            b--;
        }
    }
}

/******************************************************************************
      ����˵������ʾ�ַ�
      ������ݣ�x,y    �������
                num    Ҫ��ʾ���ַ�
                mode   1���ӷ�ʽ  0�ǵ��ӷ�ʽ
      ����ֵ��  ��
******************************************************************************/
//void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color)
//{
//    u8 temp;
//    u8 pos,t;
//	  u16 x0=x;
//    if(x>LCD_W-16||y>LCD_H-16)return;	    //���ô���
//	num=num-' ';//�õ�ƫ�ƺ��ֵ
//	LCD_Address_Set(x,y,x+8-1,y+16-1);      //���ù��λ��
//	if(!mode) //�ǵ��ӷ�ʽ
//	{
//		for(pos=0;pos<16;pos++)
//		{
//			temp=asc2_1608[(u16)num*16+pos];		 //����1608����
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)LCD_WR_DATA(color);
//				else LCD_WR_DATA(BACK_COLOR);
//				temp>>=1;
//				x++;
//		    }
//			x=x0;
//			y++;
//		}
//	}else//���ӷ�ʽ
//	{
//		for(pos=0;pos<16;pos++)
//		{
//		    temp=asc2_1608[(u16)num*16+pos];		 //����1608����
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//��һ����
//		        temp>>=1;
//		    }
//		}
//	}
//}

static void LCD_ShowABMNChar(u16 x, u16 y, u8 charIndex, u8 mode, u16 color)
{
    //u8 temp;
    //u8 pos;
    //u16 x0=x;

    if(x > LCD_W - CHARFONT_WIDTH || y > LCD_H - CHARFONT_HEIGHT)return;	 //���ô���

    LCD_Address_Set(x, y, x + CHARFONT_WIDTH - 1, y + CHARFONT_HEIGHT - 1); //���ù��λ��
    u8 *ptemp = abmnFont;

    u16 fontSize = CHARFONT_HEIGHT * (CHARFONT_WIDTH >> 3);
    ptemp += charIndex * fontSize;

    for(int j = 0; j < fontSize; j++)
    {
        for(u8 i = 0; i < 8; i++)
        {
            if((*ptemp & (1 << i)) != 0) //�����ݵĵ�λ��ʼ��
            {
                LCD_WR_DATA(color);//����
            }
            else
            {
                if(!mode) //�ǵ��ӷ�ʽ
                {
                    LCD_WR_DATA(BACK_COLOR);//������
                }
            }
        }

        ptemp++;
    }

//	if(!mode) //�ǵ��ӷ�ʽ
//	{
//		for(pos=0;pos<CHAR_HEIGHT;pos++)
//		{
//			temp=abmnFont[(u16)charIndex*CHAR_HEIGHT+pos];		 //����1608����
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)LCD_WR_DATA(color);
//				else LCD_WR_DATA(BACK_COLOR);
//				temp>>=1;
//				x++;
//		    }
//			x=x0;
//			y++;
//		}
//	}else//���ӷ�ʽ
//	{
//		for(pos=0;pos<CHAR_HEIGHT;pos++)
//		{
//		    temp=abmnFont[(u16)charIndex*CHAR_HEIGHT+pos];		 //����1608����
//			for(t=0;t<8;t++)
//		    {
//		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//��һ����
//		        temp>>=1;
//		    }
//		}
//	}
}
static u8 LCD_FindABMNCharIndex(const u8 achar)
{
    u8 charIndex = 0;

    for(u8 i = 0; i < CHAR_MAXNUM; i++)
    {
        if(abmnChars[i] == achar)
        {
            charIndex = i;
            break;
        }
    }

    return charIndex;
}

void LCD_ShowABMNString(u16 x, u16 y, const u8 *p, u8 mode, u16 color)
{
    while(*p != '\0')
    {
        if(x > LCD_W - CHARFONT_WIDTH)
        {
            x = 0;
            y += CHARFONT_HEIGHT;
        }

        if(y > LCD_H - CHARFONT_HEIGHT)
        {
            y = x = 0;
            LCD_Clear(RED);
        }

        LCD_ShowABMNChar(x, y, LCD_FindABMNCharIndex(*p), mode, color);
        x += CHARFONT_WIDTH;
        p++;
    }
}

void ConvertLCDNum(u16 num, u8 *numBuff, u8 buffLen)
{
    char numStr[10];
    sprintf(numStr, "%d", num);
    u8 numlen = strlen(numStr);
    u8 numIndex = buffLen / 2 - numlen / 2;

    //u8 j=numIndex;
    for(u8 i = 0; i < buffLen; i++)
    {
        if(i < numIndex || i >= numIndex + numlen)
        {
            numBuff[i] = ' ';
        }
        else
        {
            numBuff[i] = numStr[i - numIndex];
        }

//		if(i+numIndex<buffLen)
//			numBuff[i+numIndex]=numStr[i];//-'0'+1;
    }
}
/******************************************************************************
      ����˵������ʾ�ַ���
      ������ݣ�x,y    �������
                *p     �ַ�����ʼ��ַ
      ����ֵ��  ��
******************************************************************************/
//void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color)
//{
//    while(*p!='\0')
//    {
//        if(x>LCD_W-16){x=0;y+=16;}
//        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
//        LCD_ShowChar(x,y,*p,0,color);
//        x+=8;
//        p++;
//    }
//}


/******************************************************************************
      ����˵������ʾ����
      ������ݣ�m������nָ��
      ����ֵ��  ��
******************************************************************************/
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}


/******************************************************************************
      ����˵������ʾ����
      ������ݣ�x,y    �������
                num    Ҫ��ʾ������
                len    Ҫ��ʾ�����ָ���
      ����ֵ��  ��
******************************************************************************/
//void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color)
//{
//	u8 t,temp;
//	u8 enshow=0;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/mypow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				LCD_ShowChar(x+8*t,y,' ',0,color);
//				continue;
//			}else enshow=1;
//
//		}
//	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
//	}
//}


/******************************************************************************
      ����˵������ʾС��
      ������ݣ�x,y    �������
                num    Ҫ��ʾ��С��
                len    Ҫ��ʾ�����ָ���
      ����ֵ��  ��
******************************************************************************/
//void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color)
//{
//	u8 t,temp;
//	u8 enshow=0;
//	u16 num1;
//	num1=num*100;
//	for(t=0;t<len;t++)
//	{
//		temp=(num1/mypow(10,len-t-1))%10;
//		if(t==(len-2))
//		{
//			LCD_ShowChar(x+8*(len-2),y,'.',0,color);
//			t++;
//			len+=1;
//		}
//	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
//	}
//}


/******************************************************************************
      ����˵������ʾ40x40ͼƬ
      ������ݣ�x,y    �������
      ����ֵ��  ��
******************************************************************************/
//void LCD_ShowPicture(u16 x1, u16 y1, u16 x2, u16 y2)
//{
//    int i;
//    LCD_Address_Set(x1, y1, x2, y2);

//    for(i = 0; i < 1600; i++)
//    {
//        LCD_WR_DATA8(image[i * 2 + 1]);
//        LCD_WR_DATA8(image[i * 2]);
//    }
//}

void LCD_ShowBmp565(u8 *bmpData,u16 x, u16 y,u16 width,u16 height)
{
	u8 *pData=bmpData+72;
    int i;
	//u16 x1,y1,x2,y2;
    LCD_Address_Set(x, y, x+width-1, y+height-1);

    for(i = 0; i < width*height; i++)
    {
        LCD_WR_DATA8(pData[i * 2 + 1]);
        LCD_WR_DATA8(pData[i * 2]);
    }	
}
#endif
