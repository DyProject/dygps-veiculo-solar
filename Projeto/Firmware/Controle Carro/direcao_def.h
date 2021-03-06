#ifndef _DIRECAO_DEF_H_
#define _DIRECAO_DEF_H_

#include <avr/io.h>
#include <stdint.h>


#include "globals_def.h"
#include <util/delay.h>

//----------------------------------------------------------------------------

#define IN1								PB2
#define IN3								PB1
#define	IN2_IN4							PB3
#define ENA_ENB							PB4

#define RELE_DDR						DDRD
#define RELE_PIN						PIND
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

extern FILE stdoutUART;

//----------------------------------------------------------------------------

void SetaFonteAlimentacao(
	volatile uint8_t* fonteAlimentacao
);

void ConfiguracoesDirecaoInit();
void ConfigInitServo();

uint16_t CalculaDutyCicleM1(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
);

uint16_t CalculaDutyCicleM2(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
);

void ParadaLenta(
	Buffer* bufferRecepcao
);

void DirecaoCarro(
	Buffer* bufferRecepcao
);

void AnguloServo(
	Buffer* bufferRecepcao
);

void CarroParado();
void AndandoFrente(
	Buffer* bufferRecepcao
);
void AndandoTras(
	Buffer* bufferRecepcao
);

uint8_t RecebeProtocolo(
	Buffer* bufferRecepcao
);

void TransmitiBuffer(
	Buffer* bufferRecepcao
);

void EnviaConfirmacaoParaEstabelecerConexao(
	Buffer* buffer
);

uint8_t TensaoBateria();
uint8_t TensaoPainel();

//----------------------------------------------------------------------------

#endif