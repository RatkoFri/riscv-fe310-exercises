/*
 * hal_uart.h
 *
 *  Created on: 12 Oct 2023
 *      Author: patriciobulic
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_


#define UART0_BASEADDR     0x10013000
#define UART1_BASEADDR     0x10023000

#define UART0_RX_PIN           16
#define UART0_TX_PIN           17
#define UART0_PINS_SEL         0xfffcffff  // pins 16 and 17
#define UART0_PINS_EN          0x00030000  // pins 16 and 17
#define UART1_TX_PIN           18
#define UART1_RX_PIN           23
#define UART1_PINS_SEL         0xff7bffff  // pins 18 and 23
#define UART1_PINS_EN          0x00840000  // pins 18 and 23


#define UART_MODE_TX		   0x0
#define UART_MODE_RX		   0x1
#define UART_MODE_TX_RX        0x2


#define UART_STOPBITS_1        0xfffffffd
#define UART_STOPBITS_2        0x00000002

#define UART_BAUD_115200	   139	// this is clock divider for 115200


typedef struct
{
	volatile int UART_TXDATA; //0x0
	volatile int UART_RXDATA; //0x4
	volatile int UART_TXCTRL; //0x8
	volatile int UART_RXCTRL; //0xc
	volatile int UART_IE; //0x10
	volatile int UART_IP; //0x14
	volatile int UART_DIV; //0x18
}UART_TypeDef;

typedef struct
{
  unsigned int BaudRate;		// UART communication baud rate. Only 115200 is supported
  unsigned int StopBits;		// Specifies the number of stop bits transmitted.
  unsigned int Mode;			// Specifies whether the Receive or Transmit mode is enabled or disabled
} UART_InitTypeDef;



#define UART0 ((UART_TypeDef *)UART0_BASEADDR)
#define UART1 ((UART_TypeDef *)UART1_BASEADDR)


void HAL_UART_Init(UART_TypeDef *uart, UART_InitTypeDef* UARTInitStruct);
void HAL_UART_DeInit(UART_TypeDef *uart);
void HAL_UART_Transmit(UART_TypeDef *uart, char *pData,  unsigned int Size);




#endif /* HAL_UART_H_ */
