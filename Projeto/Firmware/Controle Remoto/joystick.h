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

void CalculaDutyCycle(
	uint16_t* dutyLadoEsq, 
	uint16_t* dutyLadoDir
);

uint8_t CalculaPorcentoPosicaoEixoY(
	uint16_t valorLidoADEixoY
);

//---------------------------------------------------------------------------

#endif