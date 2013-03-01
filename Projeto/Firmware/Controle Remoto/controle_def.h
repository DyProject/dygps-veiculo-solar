/****************************************************************
*                  PROJECT INTEGRATOR II                        *
*           Title: Definições do Controle Remoto                *
*           Students:   CAMPOS, Dyego de                        *
*                       SOUZA, Gustavo Pereira de               *
*           Date:  10/02/2013                                   *
*           Version:  1.1                                       *
****************************************************************/

#ifndef _CONTROLE_DEF_H_
#define _CONTROLE_DEF_H_

#include <avr/io.h>

//----------------------------------------------------------------------------
	
unsigned char DirecaoCarro(
	unsigned char sentido
);

void TransmitiBuffer(
	uint8_t dutyLadoEsq, 
	uint8_t dutyLadoDir, 
	unsigned char direcao
);
 
void Inicializacoes();

void MostraDadosLCD(
	uint16_t dutyLadoEsq, 
	uint16_t dutyLadoDir, 
	unsigned char direcao
);

void Protocolo();

 //----------------------------------------------------------------------------
 
#endif 