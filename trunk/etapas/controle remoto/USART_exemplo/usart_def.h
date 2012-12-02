#ifndef YUSART_H_INCLUDED
#define YUSART_H_INCLUDED

//----------------------------------------------------------------------

#define 	FOSC F_CPU                
#define 	BAUD 9600       
/* Baud Rate:
* Modo Assincrono:			BAUD = (fosc) / (16*(UBRRN + 1))
* Modo Sincrono 2x speed	BAUD = (fosc) / (8 * (UBRRN + 1))
* Modo Sincrono MAster		BAUD = (fosc) / (2 * (UBRRN + 1))
*/                
#define 	MYUBRR FOSC/16/BAUD-1

//----------------------------------------------------------------------

void Usart_Init(unsigned int);
void Usart_transmit(unsigned char*);
unsigned char Usart_Receive();

//----------------------------------------------------------------------

ISR(USART_RX_vect)							
{
	unsigned char dataReceived;
	unsigned char echo[2];

	dataReceived = Usart_Receive();	
	echo[0] = dataReceived;
	echo[1] = '\0';							
	
	Usart_transmit(echo);
				
}//ISR(USART_RX_vect)

#endif