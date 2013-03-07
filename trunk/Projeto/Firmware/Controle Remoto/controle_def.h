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

typedef struct{
	unsigned char fonteAlimentacao;//painel 'P' ou bateria 'B'
	uint8_t tensaoBateria;
	uint8_t tensaoPainel;
	uint8_t qntdDadosLido;
	uint8_t iniciado;
	uint8_t completo;
}BufferRecep;

typedef struct{
	unsigned char fonteAlimentacao;//painel 'P' ou bateria 'B'
	uint8_t tensaoBateria;
	uint8_t tensaoPainel;
	unsigned char direcao;
	unsigned char sentido;		
	uint8_t dutyLadoEsq;
	uint8_t dutyLadoDir;
}DadosLCD;	
	
//----------------------------------------------------------------------------
	
unsigned char DirecaoCarro(
	unsigned char sentido
);

void TransmitiBuffer();
 
void Inicializacoes();

void MostraDadosLCD();

void Protocolo();
uint8_t RecebeProtocolo();

 //----------------------------------------------------------------------------
 
#endif 