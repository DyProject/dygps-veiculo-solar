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

//----------------------------------------------------------------------------

#define tst_bit(Y, bit_x) (Y & (1 << bit_x))

//----------------------------------------------------------------------------

void CvrtNum2CaracterIndividual(
	uint16_t valor, 
	unsigned char* caracterConvertido
);

//----------------------------------------------------------------------------

#endif 