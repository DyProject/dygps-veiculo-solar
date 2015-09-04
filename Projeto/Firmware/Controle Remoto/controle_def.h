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
#include "globals_def.h"

//----------------------------------------------------------------------------

void TransmitiBuffer(
	unsigned char* buff
);
 
void MostraDadosLCD(
	BufferDados* bufferDados,
	unsigned char* bufferDadosTrans
);

void LCD4_MSG1(
	BufferDados* bufferDados,
	unsigned char* bufferDadosTrans
);

void LCD4_MSG_INICIAL();

void CarregaBufferTransmissao(
	BufferDados* bufferDados,
	unsigned char* buff
);

 //----------------------------------------------------------------------------
 
#endif 