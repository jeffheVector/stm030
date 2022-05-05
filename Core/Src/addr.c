#include "addr.h"
#include "systick.h"
#include "485.h"
#ifdef HDEP_GATHER
#define ADDR_LOAD_PIN GPIO_PIN_4
#define ADDR_LOAD_PORT GPIOA

#define ADDR_READ_PIN GPIO_PIN_0
#define ADDR_READ_PORT GPIOB

#define ADDR_CLK_PIN GPIO_PIN_6
#define ADDR_CLK_PORT GPIOA
#endif
void Addr_Init(void)
{
	#ifdef HDEP_GATHER
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = ADDR_LOAD_PIN | ADDR_CLK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADDR_LOAD_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ADDR_READ_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADDR_READ_PORT, &GPIO_InitStruct);
    Set0(ADDR_READ_PORT, ADDR_READ_PIN);
    Set0(ADDR_LOAD_PORT, ADDR_LOAD_PIN);
    Set0(ADDR_CLK_PORT, ADDR_CLK_PIN);
	#endif
}

u8 Addr_Read(void)
{
	#ifdef HDEP_GATHER
	Delay_ms(1);
    Set1(ADDR_LOAD_PORT, ADDR_LOAD_PIN);
    u8 addr = 0;
    u8 state = 1;
	//u8 readBuff[10];
	//readBuff[8]=0;
//	Set0(ADDR_CLK_PORT, ADDR_CLK_PIN);
//    Delay_ms(1);
//    Set1(ADDR_CLK_PORT, ADDR_CLK_PIN);
//	Delay_ms(1);
    for(u8 i = 0; i < 8; i++)
    {
        
        if(HAL_GPIO_ReadPin(ADDR_READ_PORT, ADDR_READ_PIN) == GPIO_PIN_RESET)
        {
            addr = addr | state;
			//readBuff[7-i]='1';
        }
		else
		{
			//readBuff[7-i]='0';
		}
		Set0(ADDR_CLK_PORT, ADDR_CLK_PIN);
        Delay_ms(1);
        Set1(ADDR_CLK_PORT, ADDR_CLK_PIN);
		Delay_ms(1);
		//Send485(readBuff,2);

        state = state << 1;
		Delay_ms(1);
    }
	Set0(ADDR_LOAD_PORT, ADDR_LOAD_PIN);
	//char tx_buf[10];
	//Send485(readBuff,9);
    //sprintf(tx_buf, "  485ADDR:%X\r\n", addr);
    //Send485((u8 *)tx_buf, strlen(tx_buf));
    return addr;
	#endif
	#ifdef MOTOR485
	return 0xFD;
	#endif
}

u32 baudrate_read(void)
{
	return DEFAULT485_BAUDRATE;
}
