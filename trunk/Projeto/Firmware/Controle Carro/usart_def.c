//----------------------------------------------------------------------------- //
//		AVR e Arduino: Técnicas de Projeto, 2a ed. - 2012.						//	
//----------------------------------------------------------------------------- //
//=============================================================================	//
//		Funções para trabalhalho com a USART									//
//=============================================================================	//

#include "usart_def.h"

#include <avr/io.h> 	   
#include <avr/pgmspace.h>   

//---------------------------------------------------------------------------

void Usart_Init(unsigned int ubrr)
{
	/*UBRR - Usart Baud Rate Register[UBRR] 12 bits
	* Bit 15:12		Reservado	Para uso futuro. Deve ser escrito 0. 
	* Bit 11:0		UBRR		Baud Rate
	*/
	UBRR0H = (unsigned char) (ubrr >> 8);        
	UBRR0L = (unsigned char) ubrr;
	
	/* UCSRnA - USART Control and Status Register n A
	* bit 7		0:	  R:	RXCn:	0=Sem dado buffer RX, 1=Com dado buffer RX
	* bit 6		0:	R/W:	TXCn:	0=TX Concluido, 1=TX transmitindo, 1W=limpa TXCn
	* bit 5		1:	  R:	UDREn:	0=buffer ocupado, 1=buffer pronto para receber novo dado
	* bit 4		0:	  R:	FEn:	0=próximo caracter sem erro, 1=Com erro
	* bit 3		0:	  R:	DORn:	0=sem data OverRun, 1=Com data OverRun	
	* bit 2		0:	  R:	UPEn:	0=Sem erro, 1=Erro de paridade
	* bit 1		0:	R/W:	U2Xn:	0=desabilita, 1=Dobra velocidade TX (apenas modo assincrono)	
	* bit 0		0:	  R:	MPCMn:	0=Desabilita, 1=Modo comunicação multi processo
	*OBS: sempre zerar bit 4:2 quando escrever no UCSR0A.
	*/
	UCSR0A = 0b00000000;
	//		   ||||||||
	//		   76543210	
	
	
	/* UCSRnB - USART Control and Status Register n B
	* bit 7		0:	R/W:	RXCIEn:		0=Des. Interrupção RX, 1=Habilita
	* bit 6		0:	R/W:	TXCIEn:		0=Des. Interrupção TX, 1=Habilita
	* bit 5		0:	R/W:	UDRIEn:		0=Des. Interrupção Buffer TX vazio(pronto), 1=Habilita
	* bit 4		0:	R/W:	RXENn:		0=Des. RX, 1=Habilita
	* bit 3		0:	R/W:	TXENn:		0=Des. TX, 1=Habilita
	* bit 2		0:	R/W:	UCSZn2:		Seta bits tamanho do frame. Combinado com UCSZn1 e UCSZn0
	* bit 1		0:	  R:	RXB8n:		9° bit RX quando frame 9 bits
	* bit 0		0:	R/W:	TXB8n:		9° bit TX quando frame 9 bits
	*/				
	UCSR0B = 0b00011000;
	//		   ||||||||
	//		   76543210	
	 
	/* UCSRnC - USART Control and Status Register n C
	* bit 7-6	0:	R/W:	UMSELn1:0:	00=Modo Assincrono, 01=Sincrono, 10=Reservado, 11=MasterSPI
	* bit 5-4	0:	R/W:	UPMn1:0:	00=Paridade Des., 01=Reservado, 10=Par, 11=Ímpar	
	* bit 3		0:	R/W:	USBSn:		0=Um stop bit, 1=Dois stop bit
	* bit 2-1	0:	R/W:	UCSZn1:0:	Combinado com UCSZn2:0.	100, 101, 110= Reservados.
	*									000=Frame Cinco bits, 001=Seis, 010= Sete, 011=Oito, 111=Nove
	* bit 0		0:	R/W:	UCPOLn:		Somento modo Sincrono. 0=TX borda subido e RX descida.
															   1=TX borda descida e RX subida.
	*/
	UCSR0C = 0b00000110;
	//		   ||||||||
	//		   76543210		
}

//---------------------------------------------------------------------------

void Usart_Transmit(unsigned char dado)
{
	/*wait for empty transmit buffer*/
	while (!( UCSR0A & (1<<UDRE0)) );	
	
	/*put data into buffer, sends the data*/
	UDR0 = dado; 				
}

//---------------------------------------------------------------------------

unsigned char Usart_Receive()
{
	/*wait for data to be receive*/
	while (!(UCSR0A & (1<<RXC0)));	
	
	/*read data into buffer, receive the data*/
	return UDR0; 				
}

//---------------------------------------------------------------------------

void Usart_Write(char *c)		
{
   for (; *c!=0;c++) Usart_Transmit(*c);
}

//---------------------------------------------------------------------------

void Usart_Write_Flash(const char *c)	
{
   for (;pgm_read_byte(&(*c))!=0;c++) Usart_Transmit(pgm_read_byte(&(*c)));
}

//---------------------------------------------------------------------------