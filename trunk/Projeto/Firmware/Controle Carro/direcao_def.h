#ifndef _DIRECAO_DEF_H_
#define _DIRECAO_DEF_H_

#include <avr/io.h>
#include <stdint.h>


#include "globals_def.h"
#include <util/delay.h>

//----------------------------------------------------------------------------

#define IN1								PB1
#define IN3								PB2
#define	IN2_IN4							PB3
#define ENA_ENB							PB4

#define RELE_PORT						PORTD
#define RELE_PAINEL_MOTOR				PD3
#define RELE_TENSAO_PAINEL				PD4

#define AJUSTE_AD						0.0369
#define AD_PAINEL						0
#define AD_BATERIA						1

#define AJUSTE_AD						0.0493
//----------------------------------------------------------------------------

#define ATIVA_RELE(rele)				(RELE_PORT |= (1 << rele))
#define DESATIVA_RELE(rele)				(RELE_PORT &= ~(1 << rele))	
#define RELE_HABILITADO(rele)			(RELE_PORT & (1 << rele))

//----------------------------------------------------------------------------

typedef struct{
	uint8_t direcao;
	uint8_t dutyCicleM1;
	uint8_t dutyCicleM2;
	uint8_t qntdDadosLido;
	uint8_t iniciado;
	uint8_t completo;
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

uint8_t RecebeProtocolo();

void TrasmitiBuffer();

uint8_t TensaoBateria();
uint8_t TensaoPainel();

//----------------------------------------------------------------------------

#endif