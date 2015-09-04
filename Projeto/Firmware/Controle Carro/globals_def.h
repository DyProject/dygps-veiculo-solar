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
#include "stdio.h"

//----------------------------------------------------------------------------

#define	set_bit(Y, bit_x) (Y |= (1 << bit_x))
#define	clr_bit(Y, bit_x) (Y &=~ (1 << bit_x))
#define tst_bit(Y, bit_x) (Y & (1 << bit_x))
#define cpl_bit(Y, bit_x) (Y ^= (1 << bit_x))

//----------------------------------------------------------------------------

#define START_TRANSMISSION 'x'
#define ACK_RECEPTOR 'z'
#define ERRO_REPORTADO 'y'
#define QUANT_DADOS_PACOTE_TRANS 6

//----------------------------------------------------------------------------

typedef enum {
	DIRECAO = 0, DUTY_MOTOR_E, DUTY_MOTOR_D, ANGULO_SERVO_E, ANGULO_SERVO_D, FONTE_ALIMENTACAO
}TDadosTransmissao;

typedef enum {
	PARADO = 1, ANDANDO_FRENTE, ANDANDO_TRAS
}TEstadoCarro;

//Buffer Recepção e Transmissão
typedef enum {
	IDLE=0, RECEBENDO_DADOS, CHECKSUM, INICIADO, CONCLUIDO, ERRO_COMUNICACAO, COD_ERRO
}TBuffer;

/* CÓDIGOS DE ERRO RECEPÇÃO
(48) SEM_ERRO - NÃO APRESENTA ERRO,
(49) NAO_INIC - RECEBEU DADOS NO rx PORÉM NÃO RECEBEU O BYTE 'S' INDICANDO O INICIO DA TRANSMISSÃO
*/
typedef enum {
	SEM_ERRO_RECEP = 48, NAO_INIC_CORRET_RECEP
}TErroCode;

//----------------------------------------------------------------------------

typedef struct{
	volatile uint8_t dadosRecebidosComSucesso;
	volatile uint8_t qntDadosAReceber;
	volatile uint8_t iniciado;//'y' yes ou 'n' no
	volatile uint8_t completo;//'y' yes ou 'n' no
	volatile TBuffer estadoBufferRecep;
	volatile TBuffer estadoBufferTrans;
	volatile TErroCode erroCodeRecep;
	volatile uint8_t fonteAlimentacao;//'B' bateria ou 'P' painel
	volatile uint8_t direcao;
	volatile int dutyCicleM1;
	volatile int dutyCicleM2;
	volatile int qntdDadosLido;
	volatile TEstadoCarro estadoCarro;
	volatile unsigned char anguloServoLeft;
	volatile unsigned char anguloServoRight;
}Buffer;

//----------------------------------------------------------------------------

#endif 