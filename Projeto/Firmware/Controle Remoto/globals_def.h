/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Definições de Macros                         *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.0                                       *
****************************************************************/

#ifndef _GLOBALS_DEF_H_
#define _GLOBALS_DEF_H_

#include <stdint.h>

#define F_CPU 16000000UL

//----------------------------------------------------------------------------

#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))

//----------------------------------------------------------------------------

typedef enum {
	PARADO = 1, ANDANDO_FRENTE,  ANDANDO_FRENTE_DIREITA, ANDANDO_FRENTE_ESQUERDA, 
	ANDANDO_TRAS, ANDANDO_TRAS_DIREITA, ANDANDO_TRAS_ESQUERDA,
}TEstadoCarro;
	
//----------------------------------------------------------------------------

typedef struct{
	volatile uint8_t fonteAlimentacao;//'B' bateria ou 'P' painel
	volatile int qntdDadosLido;
	volatile uint8_t iniciado;//'y' yes ou 'n' no
	volatile uint8_t completo;//'y' yes ou 'n' no
	volatile uint16_t tensaoBateria;
	volatile uint16_t tensaoPainel;
	volatile int dutyLadoEsq;
	volatile int dutyLadoDir;
	volatile uint8_t direcao;
	volatile uint8_t sentido;	
	volatile TEstadoCarro estadoCarro;
	volatile uint8_t podeIniciarTransmissao;
	volatile uint8_t botaoSelFontePress;
	volatile uint8_t podeRecalcularDutyCicle;//'y' yes ou 'n' no - Caso não tenha recebido dados do carro, não envia novo valor do duty cicle
	volatile unsigned char anguloServoLeft; 
	volatile unsigned char anguloServoRight;
}BufferDados;

//----------------------------------------------------------------------------

/*!
	\param uint16_t valor - valor a ser convertido
	\param unsigned char* caracterConvertido - caracter convertido
	\param uint8_t tamanho
	\brief Converte um valor inteiro para  valores em char individuais.
*/	
void CvrtNum2CaracterIndividual(
	uint16_t valor, 
	unsigned char* caracterConvertido,
	uint8_t tamanho
);

//----------------------------------------------------------------------------

#endif 