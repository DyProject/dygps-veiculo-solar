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
	BufferDados* bufferDados
);
 
void MostraDadosLCD(
	BufferDados* bufferDados
);

uint8_t RecebeProtocolo(
	BufferDados* bufferRecepcao
);

 //----------------------------------------------------------------------------
 
#endif 