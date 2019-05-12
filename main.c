/**
  ******************************************************************************
  * @file     STM32F4 GSM SIM900/main.c 
  * @author   Hssairi Mohamed
  * @version V1.0.0
  * @date    12/05/2019
  * @brief   Main program body.
  ******************************************************************************
  */ 

//******************************************************************************
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "discoveryf4utils.h"
#include "uart_to_printf.h"
//******************************************************************************

//******************************************************************************
/* Private typedef -----------------------------------------------------------*/
	EXTI_InitTypeDef   EXTI_InitStructure;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char t1;
int received=0,OK=0;
int res=0;
int len=0;
int nb;
char * msg;

/* Private function prototypes -----------------------------------------------*/
//void EXTILine0_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
	void USART1_GpioConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
  USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
// NVIC_InitTypeDef NVIC_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 6 (TX) and 7 (RX) are used
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // the pins are configured as alternate function so the USART peripheral has access to them
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;// this defines the IO speed and has nothing to do with the baudrate!
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;// this defines the output type as push pull mode (as opposed to open drain)
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;// this activates the pullup resistors on the IO pins
  GPIO_Init(GPIOB, &GPIO_InitStruct);// now all the values are passed to the GPIO_Init() function which sets the GPIO registers

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

  USART_InitStruct.USART_BaudRate = 9600;// the baudrate is set to the value we passed into this init function
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
  USART_InitStruct.USART_StopBits = USART_StopBits_1;// we want 1 stop bit (standard)
  USART_InitStruct.USART_Parity = USART_Parity_No;// we don't want a parity bit (standard)
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
  USART_Init(USART1, &USART_InitStruct);// again all the properties are passed to the USART_Init function which takes care of all the bit setting

// finally this enables the complete USART1 peripheral
  USART_Cmd(USART1, ENABLE);
	
	


}
void Init_Exti(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect EXTI Line0 to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

void LED_Config(void)
{
  /* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15|
  GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

}



void USART_puts(USART_TypeDef* USARTx, volatile char *s)      //send a msg(chaine of chars)
{
  while(*s)
{
// wait until data register is empty
  while( !(USARTx->SR & 0x00000040) );
  USART_SendData(USARTx, *s);
  *s++;
}
}
void USART_putc(USART_TypeDef* USARTx, volatile char s)      //send a char
{
  while( !(USARTx->SR & 0x00000040) );
  USART_SendData(USARTx, s);
}

int SEND_AT(void)
{                             //AT to check gsm connection
  USART_puts(USART1,"AT\r");
  USART_putc(USART1,(char)13);
  USART_putc(USART1,(char)10);
  t1='w';
  while (t1 !='K')
{  
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
  t1=USART_ReceiveData(USART1);	      //receive the OK if cnx is establshied
  len++;
  if ( t1=='O' )
{
  nb=len;  
}
  if ( (t1=='K') && (len==nb+1) )
{
  res=1;
}
}
  return res;
}

void Send_Msg1(char * msg) 
{
  USART_puts(USART1,"AT+cmgs=");//sending sms to the numberphone
  USART_putc(USART1,(char)'"');// " before number
  USART_puts(USART1,"+21626957665");//num makni
  USART_putc(USART1,(char)'"');// " after number
  USART_putc(USART1,(char)13);
  USART_putc(USART1,(char)10);
  Delay(0xFFFFFF);
  Delay(0xFFFFFF);
  USART_puts(USART1,msg);
  USART_putc(USART1,(char)26); //ascii code of ctrl+z pour dire la fin du sms
	 
  Delay(0xFFFFFF);
  Delay(0xFFFFFF);
}
int SEND_Text_mode()
{
  int len=0,nb,res=0;
  USART_puts(USART1,"AT+cmgf=1\r");  // sending sms mode is on
  t1='w';
  while (t1 !='K')
{  
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
  t1=USART_ReceiveData(USART1);	
  len++;
  if ( t1=='O' ){ nb=len;}
  if ( (t1=='K') && (len==nb+1) )
{ 
  res=1;
}
}
  return res;
}
extern volatile int Intrusion_Detected;
int msgReceive(){
	int len=0,nb=0,ok=0;
	int locate=0;
	char * texto ;
	texto= "Alarme is on";
	USART_puts(USART1,"AT+cnmi=2,2,0,0,0\r");
	 t1='w';
  while (t1 !='K')
{  
	  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET);
  t1=USART_ReceiveData(USART1);	
  len++;
  if ( t1=='O' ){ nb=len;}
  if ( (t1=='K') && (len==nb+1) )
{ 
  locate=1;
}
}
  Delay(0xFFFFFF);
  Delay(0xFFFFFF);
t1='w';
while (t1!='L')
{
	while((USART_GetFlagStatus(USART1, USART_FLAG_RXNE)== RESET)&&(Intrusion_Detected==0));
 	
	if (Intrusion_Detected)
			{
					Send_Msg1(texto);       
					Intrusion_Detected=0;
			}
	t1=USART_ReceiveData(USART1);		
	}
return 1 ;
}


void deleteMsg(void){
//puts AT+CMGD="4"pour effacer tous les msg

USART_puts(USART1,"AT+cmgd=1\r");


}


//recieve msg : AT+CMTI:"ch po quoi",id
//AT+CMGR=id read msg received
//AT+CMGD=id delete msg
int main(void)
{

	LED_Config();
char * phoneNumber;
	
	//phoneNumber= "+21653587324";
	
	Intrusion_Detected=0;
USART1_GpioConfig() ;
Init_Exti();
while (OK==0)
{
  OK=SEND_AT();       //ready ?
}
  OK=0;
  GPIO_SetBits(GPIOD,GPIO_Pin_12);//led blink if ready
while (OK==0)
{
  OK=SEND_Text_mode(); //send mode on ?
}
  GPIO_SetBits(GPIOD,GPIO_Pin_13);//led blink if send mode is on

  while (1)
  {
		int i=0;
			if (Intrusion_Detected)
			{
					Send_Msg1("Alarme is on");       
					Intrusion_Detected=0;
			}
		i=msgReceive();
		if (i==1)
		{
			
			GPIO_SetBits(GPIOD,GPIO_Pin_14);
			Send_Msg1("https://www.google.com/maps/place/36.807124,10.106196"); 
			i=0;
		}
		
  }
}
//******************************************************************************

