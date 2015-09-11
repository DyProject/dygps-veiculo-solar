#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "globals_def.h"

//---------------------------------------------------------------------------

#define AD_EIXO_X_DIR		4
#define AD_EIXO_Y_DIR		5
#define AD_EIXO_X_SERVO		2
#define AD_EIXO_Y_SERVO		3

//---------------------------------------------------------------------------

#define PONTO_INIC_X 524
#define LIM_INF_PONTO_INIC_X 520//478
#define LIM_SUP_PONTO_INIC_X 528//542

#define PONTO_INIC_Y 506
#define LIM_INF_PONTO_INIC_Y 502//478
#define LIM_SUP_PONTO_INIC_Y 510//542

//---------------------------------------------------------------------------
		
int16_t ValorEixoX(
	unsigned char adSelected
);
int16_t ValorEixoY(
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

unsigned char CalculaAnguloServoLeft(
	int16_t dutyLE
);

unsigned char CalculaAnguloServoRight(
	int16_t dutyLD
);
	
void TankDrive(
	JoyStick* joyStick
);

uint8_t PontoXNaPosInic(
	uint16_t valorEixoY
);
uint8_t PontoYNaPosInic(
	uint16_t valorEixoY
);

uint8_t SoftStarterLadoEsq(
	uint8_t
);
uint8_t SoftStarterLadoDir(
	uint8_t
);

unsigned char DirecaoCarro(
	JoyStick joy
);

//---------------------------------------------------------------------------

#endif