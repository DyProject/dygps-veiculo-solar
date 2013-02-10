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

#define BT_FRENTE			PB0
#define BT_RE				PB1
#define BT_DIREITA          PB2
#define BT_ESQUERDA			PB3

#define PINO_BOTOES			PINB

#define BT_FRENTE_ISPRESSED()		(!tst_bit(PINO_BOTOES, BT_FRENTE))
#define BT_RE_ISPRESSED()			(!tst_bit(PINO_BOTOES, BT_RE))
#define BT_DIREITA_ISPRESSED()		(!tst_bit(PINO_BOTOES, BT_DIREITA))
#define BT_ESQUERDA_ISPRESSED()		(!tst_bit(PINO_BOTOES, BT_ESQUERDA))
 
//----------------------------------------------------------------------------

typedef enum {
	PARADO = 1, ANDANDO_FRENTE = 2, ANDANDO_TRAS = 3, ANDANDO_DIREITA = 4, ANDANDO_ESQUERDA = 5
	}TEstadoCarro;
	
//----------------------------------------------------------------------------
	
void EnviaDirecaoCarro();
 
 //----------------------------------------------------------------------------
 
#endif 