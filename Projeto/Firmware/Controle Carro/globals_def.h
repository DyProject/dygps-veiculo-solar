/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Definições de Macros                         *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.0                                       *
****************************************************************/

#define F_CPU 16000000UL

#ifndef _GLOBALS_DEF_H_
#define _GLOBALS_DEF_H_

#include <stdint.h>

//----------------------------------------------------------------------------

#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))

//----------------------------------------------------------------------------

typedef enum {
	PARADO = 1, ANDANDO_FRENTE, ANDANDO_TRAS
}TEstadoCarro;

//----------------------------------------------------------------------------

typedef struct{
	volatile uint8_t fonteAlimentacao;//'B' bateria ou 'P' painel
	volatile uint8_t direcao;
	volatile int dutyCicleM1;
	volatile int dutyCicleM2;
	volatile int qntdDadosLido;
	volatile uint8_t iniciado;//'y' yes ou 'n' no
	volatile uint8_t completo;//'y' yes ou 'n' no
	volatile TEstadoCarro estadoCarro;
}BufferRecep;

//----------------------------------------------------------------------------
	
void CvrtNum2CaracterIndividual(
	uint16_t valor, 
	unsigned char* caracterConvertido,
	uint8_t tamanho
);

//----------------------------------------------------------------------------

#endif 