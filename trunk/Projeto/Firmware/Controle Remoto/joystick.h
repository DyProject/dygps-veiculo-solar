#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <stdint.h>

//---------------------------------------------------------------------------

#define AD_EIXO_X		0
#define AD_EIXO_Y		1

//---------------------------------------------------------------------------
		
uint16_t ValorLidoADEixoX();
uint16_t ValorLidoADEixoY();

uint8_t PontoXNaPosInic();
uint8_t PontoYNaPosInic();

unsigned char CalculaSentido();

uint8_t SoftStarterLadoEsq(
	uint8_t
);
uint8_t SoftStarterLadoDir(
	uint8_t
);

uint8_t CalculaDutyCycleLadoEsq();
uint8_t CalculaDutyCycleLadoDir();

uint8_t CalculaPorcentoPosicaoEixoY(
	uint16_t valorLidoADEixoY
);

uint8_t CalculaPorcentoPosicaoEixoX(
	uint16_t valorLidoADEixoY
);

unsigned char DirecaoCarro();

//---------------------------------------------------------------------------

#endif