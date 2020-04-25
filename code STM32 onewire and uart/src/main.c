#include "stm32f10x.h"

GPIO_InitTypeDef 					gpioInit;
USART_InitTypeDef					usartInit;
TIM_TimeBaseInitTypeDef 	timerInit;
//----------------Variabe Define-----------------------------//
uint16_t u16Tim;
uint8_t u8Buff[5];
uint8_t i;

//----------------TimerConfig-----------------------------//
void TIM_Config(void);
void Delay1Ms(void);
void Delay_Ms(uint32_t T_ms);

//----------------GPIOConfig-----------------------------//
void GPIO_Config(void);

//----------------UARTConfig-----------------------------//
void TX_Config(void);
void RX_Config(void);
void Uart_Config(void);

//----------------OneWire-----------------------------//
void control_signal(void);
void respond_signal(void);
void ready_signal(void);
void read_data(uint8_t index);
void check_sum(void);
void One_Wire(void);

//----------------Main-----------------------------//
int main(void){
	GPIO_Config();
	TIM_Config();
	TX_Config();
	RX_Config();
	Uart_Config();
  TIM_Cmd(TIM2,ENABLE);
	USART_Cmd(USART3,ENABLE);
	
	//GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	
	while (1) {
		One_Wire();
		Delay_Ms(1000);
		USART_SendData(USART3,0xFE);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
		Delay_Ms(50);
		USART_SendData(USART3,u8Buff[0]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
		Delay_Ms(50);
		USART_SendData(USART3,u8Buff[1]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
		Delay_Ms(50);
		USART_SendData(USART3,u8Buff[2]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
		Delay_Ms(50);
		USART_SendData(USART3,u8Buff[3]);
  	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
	}
}
//----------------GPIOConfig-----------------------------//
void GPIO_Config(void){
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
//	gpioInit.GPIO_Pin = GPIO_Pin_8;
//	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &gpioInit);
	
	//cau hinh chan doc cam bien- luon de chan o trang thai OD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
	gpioInit.GPIO_Pin = GPIO_Pin_12;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);

}
//----------------UARTConfig-----------------------------//
void TX_Config(void){
		//PB10-UART3
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInit.GPIO_Pin = GPIO_Pin_10;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);
}
void RX_Config(void){
		//PB11-UART3
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Pin = GPIO_Pin_11;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);
}
void Uart_Config(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    usartInit.USART_BaudRate=9600;
    //khong bat tay
    usartInit.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    //vua truyen vua nha
    usartInit.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
    usartInit.USART_Parity=USART_Parity_No;
    //1 bit stop
    usartInit.USART_StopBits=USART_StopBits_1;
    usartInit.USART_WordLength= USART_WordLength_8b;
    USART_Init(USART3,&usartInit);
}
//----------------TIMERConfig-----------------------------//
void TIM_Config(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    timerInit.TIM_CounterMode= TIM_CounterMode_Up;
    timerInit.TIM_Period=0xFFFF; // mac dinh
		//tao la timer 1/10^6 s
    timerInit.TIM_Prescaler=72-1;
    //c?u hình timer
    TIM_TimeBaseInit(TIM2,&timerInit);

}
void Delay1Ms(void){
    TIM_SetCounter(TIM2,0);
    while (TIM_GetCounter(TIM2)<1000){}
}
void Delay_Ms(uint32_t T_ms){
    while(T_ms){
            Delay1Ms();
            --T_ms;
    }
}


//----------------OneWireConfig-----------------------------//

void control_signal(void){
		//tin hieu keo xuong 0 20ms va day len cao
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_Ms(20);//>18ms là oke
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
		// Kiem tra keo len muc cao hay la do timeout xay ra
		TIM_SetCounter(TIM2,0);
		while(TIM_GetCounter(TIM2)<10){
						if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)){ 
								break;
						}
				}
		//do gia tri bo dem cua timer2
		u16Tim =TIM_GetCounter(TIM2);
		//TIm out >10 la error
		// Nghia la no ko len muc 1 nen error
		if(u16Tim >=10){ 
				while(1){}
		}
		
		//=> Dang High
}
void respond_signal(void){
		//wait PB12=0 trong 5 ->45 micro s
		TIM_SetCounter(TIM2,0);
		while(TIM_GetCounter(TIM2)<45){
						if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)){ 
						break;
						}
				}
		u16Tim =TIM_GetCounter(TIM2);
		// No khong co xuong thap => error
		if((u16Tim >=45)||(u16Tim <=5)){ 
				while(1){}
		}
		//-> dang low
}
void ready_signal(void){
	//wait xung low : 70-90 mico s
	//wait xung high: 75-95 micro s
	//wait chan PB12 len cao
	TIM_SetCounter(TIM2,0);
	//KT xem trong 90 mms do co len high chua
	while(TIM_GetCounter(TIM2)<90){
					if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)){ 
					break;
					}
			}
	u16Tim =TIM_GetCounter(TIM2);
	// No khong co xuong thap => error
	if((u16Tim >=90)||(u16Tim <=70)){ 
			while(1){}
	}
	//->dang high
	
	//wait chan PB12 xuong thap 
	TIM_SetCounter(TIM2,0);
	//Kiem tra trong 95mms co xuong low chua
	while(TIM_GetCounter(TIM2)<95){
					if(!GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)){ 
					break;
					}
			}
	u16Tim =TIM_GetCounter(TIM2);
	// No khong co xuong thap => error
	if((u16Tim >=95)||(u16Tim <=75)){ 
			while(1){}
	}
	//->dang low
	//done check
}
void read_data(uint8_t index){
		for (i = 0; i < 8; ++i) {
			//wait bit 0 (45-65msm)
			//Neu 1(45-80) =>bit1
			//Neu 0 (10-45) => bit0
			/* cho chan PB12 len cao */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 65) {
				if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 65) || (u16Tim <= 45)) {
				while (1) {
				}
			}
			
			/* cho chan PB12 xuong thap */
			TIM_SetCounter(TIM2, 0);
			while (TIM_GetCounter(TIM2) < 80) {
				if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
					break;
				}
			}
			u16Tim = TIM_GetCounter(TIM2);
			if ((u16Tim >= 80) || (u16Tim <= 10)) {
				while (1) {
				}
			}
			u8Buff[index] <<= 1;
			if (u16Tim > 45) {
				/* nhan duoc bit 1 */
				u8Buff[index] |= 1;
			} else {
				/* nhan duoc bit 0 */
				u8Buff[index] &= ~1;
			}
		}
}
void check_sum(void){
		if (u8Buff[4] !=(u8Buff[0]+u8Buff[1]+u8Buff[2]+u8Buff[3])){while(1) {}}
}

void One_Wire(void){
		uint8_t c;
		control_signal();
		respond_signal();
		ready_signal();
		for(c=0;c<5;c++){
			read_data(c);
		}
		check_sum();
}