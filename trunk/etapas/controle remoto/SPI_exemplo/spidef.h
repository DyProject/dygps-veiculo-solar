#ifndef __SPI_DEF_H__
#define __SPI_DEF_H__

#include <avr/io.h>

//--------------------------------------------------------------

/* Definições pinos e portas */
#define SPI_DDR							DDRB
#define SPI_PORT						PORTB
#define SPI_DD_SS						PB2	
#define SPI_DD_MOSI						PB3
#define SPI_DD_MISO						PB4
#define SPI_DD_SCK						PB5

/* Habilita e desabilita modos */
#define SPI_HABILITA_INTERRUPCAO_SPI()		(SPCR |= (1 << SPIE))
#define SPI_DESABILITA_INTERRUPCAO_SPI()	(SPCR &= (~(1 << SPIE)))
#define SPI_HABILITA_SPI()					(SPCR |= (1 << SPE))
#define SPI_DESABILITA_SPI()				(SPCR &= (~(1 << SPE)))
#define SPI_HABILITA_SLAVE()				(SPI_PORT &= (~(1 << SPI_DD_SS)))
#define SPI_DESABILITA_SLAVE()				(SPI_PORT |= (1 << SPI_DD_SS))	

/*Define a frequência do SCK MASTER Fosc/x*/
#define SPI_SCK_FOSC_DIV_4()		SPCR &= (~(1 << SPR1)) & (~(1 << SPR0))
#define SPI_SCK_FOSC_DIV_16()		SPCR &= (~(1 << SPR1)) & (SPCR | (1 << SPR0))
#define SPI_SCK_FOSC_DIV_64()		SPCR &= ((SPCR | (1 << SPR1) & (~(1 << SPR0)))
#define SPI_SCK_FOSC_DIV_128()		SPCR &= ((~(1 << SPR1) & (~(1 << SPR0)))

/* Define o modo de envio dos bits*/
#define SPI_MSB_FIRST()				SPCR &= (~(1 << DORD))
#define SPI_LSB_FIRST()				SPCR |= (1 << DORD)

/* Polaridade do clock IDLE*/
#define SPI_CLOCK_POLARITY_HIGH_IDLE()	SPCR &= (~(1 << CPOL))
#define SPI_CLOCK_POLARITY_LOW_IDLE()	SPCR |= (1 << CPOL)

/* Amostra na subida ou descida do clock */
#define SPI_AMOSTRA_SUBIDA_SCK			SPCR &= (~(1 << CPHA))
#define SPI_AMOSTRA_DESCIDA_SCK			SPCR |= (1 << CPHA)

//--------------------------------------------------------------

void SPI_MasterInit();
void SPI_Transmit(char cData);
void SPI_SlaveInit();
char SPI_Receive();

//--------------------------------------------------------------

#endif