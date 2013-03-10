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

#define RELE_DDR						DDRB
#define RELE_PIN						PINB
#define RELE_PORT						PORTD
#define RELE_CHAVE_PAINEL_BATERIA		PD3
#define RELE_TENSAO_PAINEL				PD4

#define AD_PAINEL						0
#define AD_BATERIA						1

#define AJUSTE_AD						0.0493
//----------------------------------------------------------------------------

#define ATIVA_RELE(rele)				(RELE_PORT |= (1 << rele))
#define DESATIVA_RELE(rele)				(RELE_PORT &= ~(1 << rele))	
#define RELE_HABILITADO(rele)			(RELE_PORT & (1 << rele))

//----------------------------------------------------------------------------

void SetaFonteAlimentacao(
	volatile uint8_t* fonteAlimentacao
);

void ConfiguracoesDirecaoInit();

uint16_t CalculaDutyCicleM1(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
);

uint16_t CalculaDutyCicleM2(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
);

void DirecaoCarro(
	BufferRecep* bufferRecepcao
);

void CarroParado();
void AndandoFrente(
	BufferRecep* bufferRecepcao
);
void AndandoTras(
	BufferRecep* bufferRecepcao
);

uint8_t RecebeProtocolo(
	BufferRecep* bufferRecepcao
);

void TransmitiBuffer(
	volatile uint8_t* fonteAlimentacao
);

uint8_t TensaoBateria();
uint8_t TensaoPainel();

//----------------------------------------------------------------------------

#endif