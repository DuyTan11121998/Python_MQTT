#include "stm32f10x.h"

#define START 0xF1
#define STOP 0xF2

#define SENSOR1	0XE1
#define SENSOR2 0XE2
#define RASPBERRY 0xE3


GPIO_InitTypeDef 					gpioInit;
USART_InitTypeDef					usartInit;
TIM_TimeBaseInitTypeDef 	timerInit;

//----------------Variabe Define-----------------------------//
uint8_t DATA;
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

//----------------------RS485----------------------------//
int Check_Address(uint8_t dt_addr,uint8_t addr);
void Send_Data(uint8_t data);
void Receive_Enable(void);
void Send_Enable(void);
uint8_t Received_Data(void);
//----------------Main-----------------------------//
int main(void){
	GPIO_Config();
	TIM_Config();
	TX_Config();
	RX_Config();
	Uart_Config();
  TIM_Cmd(TIM2,ENABLE);
	USART_Cmd(USART3,ENABLE);
	
	//Slave Sesor 1
	while (1) {
			Receive_Enable();
			if(Received_Data()==START){
						if(Check_Address(Received_Data(),SENSOR1)){
											Send_Enable();
											//Send data
											Send_Data(START);
											Send_Data(SENSOR1);
											Send_Data(0x00);
											Send_Data(0x00);
											//....
											Send_Data(STOP);
						}
			}
	}
	
	//Slave Sersor 2
	while(1){
			Receive_Enable();
			if(Received_Data()==START){
							if(Check_Address(Received_Data(),SENSOR2)){
												Send_Enable();
												//Send data
												Send_Data(START);
												Send_Data(SENSOR2);
												Send_Data(0x00);
												Send_Data(0x00);
												//....
												Send_Data(STOP);
							}
				}
	}
	
	//Master
	
	while(1){
			Delay_Ms(10000);// Chu ki lay data ve
			
			//Get data from sensor1
			Send_Enable();
			Send_Data(START);
			Send_Data(SENSOR1);
			Receive_Enable();
			if(Received_Data()==START){
					if(Received_Data()==SENSOR1){
							while(1){
									DATA=Received_Data();
									// SAVE in ARRAY BUFFDATA
									if(DATA==STOP) break;
							}
					}
			}
			
			//GET data from sensor2
			Send_Enable();
			Send_Data(START);
			Send_Data(SENSOR2);
			Receive_Enable();
			if(Received_Data()==START){
					if(Received_Data()==SENSOR2){
							while(1){
									DATA=Received_Data();
									// SAVE in ARRAY BUFFDATA
									if(DATA==STOP) break;
							}
					}
			}
			
			//SEND data to Raspberry
			Send_Enable();
			Send_Data(START);
			Send_Data(RASPBERRY);
			//-------send frame data
			Send_Data(STOP)
	}
}
//----------------GPIOConfig-----------------------------//
void GPIO_Config(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInit.GPIO_Pin = GPIO_Pin_10;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);

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
    //c?u h�nh timer
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

//----------------------RS485----------------------------//
int Check_Address(uint8_t dt_addr,uint8_t addr){
	if (dt_addr == addr) return 1;
	else return -1;
}
void Send_Data(uint8_t data){
	USART_SendData(USART3,data);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
	Delay_Ms(50);
}
void Receive_Enable(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}
void Send_Enable(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

uint8_t Received_Data(void){
		while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)==RESET){};
		return USART_ReceiveData(USART3);
}