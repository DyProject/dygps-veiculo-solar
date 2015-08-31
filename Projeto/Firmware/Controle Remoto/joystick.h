#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "globals_def.h"

//---------------------------------------------------------------------------

#define AD_EIXO_X_DIR		4
#define AD_EIXO_Y_DIR		5
#define AD_EIXO_X_SERVO		2
#define AD_EIXO_Y_SERVO		3

//---------------------------------------------------------------------------
		
uint16_t ValorLidoADEixoX(
	unsigned char adSelected
);
uint16_t ValorLidoADEixoY(
	unsigned char adSelected
);

//---------------------------------------------------------------------------

typedef struct{
	volatile int32_t valorEixoX;
	volatile int32_t valorEixoY;
	volatile uint32_t hypotenuse;
	volatile uint16_t angleDegrees;
	volatile int16_t dutyLD;
	volatile int16_t dutyLE;
}JoyStick;

//---------------------------------------------------------------------------

void CalculaAngulosServo(
	JoyStick* joyStick,
	BufferDados* bufferDados
);
	
void TankDrive(
	JoyStick* joyStick
);

uint8_t PontoXNaPosInic(
	unsigned char adSelected
);
uint8_t PontoYNaPosInic(
	unsigned char adSelected
);

unsigned char CalculaSentido();

uint8_t SoftStarterLadoEsq(
	uint8_t
);
uint8_t SoftStarterLadoDir(
	uint8_t
);

uint8_t CalculaDutyCycleLadoEsq();
uint8_t CalculaDutyCycleLadoDir();

uint8_t CalculaPorcentoPosicaoEixoY(
	uint16_t valorLidoADEixoY
);

uint8_t CalculaPorcentoPosicaoEixoX(
	uint16_t valorLidoADEixoY
);

unsigned char DirecaoCarro();

//---------------------------------------------------------------------------

#endif