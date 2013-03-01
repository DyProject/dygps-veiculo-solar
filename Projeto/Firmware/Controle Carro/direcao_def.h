#ifndef _DIRECAO_DEF_H_
#define _DIRECAO_DEF_H_

#include <avr/io.h>
#include <stdint.h>


#include "globals_def.h"
#include <util/delay.h>

//----------------------------------------------------------------------------

#define IN1			PB1
#define IN3			PB2
#define	IN2_IN4		PB3
#define ENA_ENB		PB4

//----------------------------------------------------------------------------

typedef struct{
	uint8_t direcao;
	uint8_t dutyCicleM1;
	uint8_t dutyCicleM2;
	uint8_t qntdDadosLido;
	uint8_t iniciado;
}BufferRecep;

//----------------------------------------------------------------------------

void ConfiguracoesDirecaoInit();

uint16_t CalculaDutyCicleM1(
	uint8_t porCentagem
);

uint16_t CalculaDutyCicleM2(
	uint8_t porCentagem
);

void DirecaoCarro();

void CarroParado();
void AndandoFrente();
void AndandoTras();

void RecebeProtocolo();

//----------------------------------------------------------------------------

#endif