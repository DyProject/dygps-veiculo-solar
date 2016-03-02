#include "joyStick.h"

#include <math.h>

#include "adc_def.h"
#include "globals_def.h"
#include "stdio.h"
#include "LCD4b.h"

//---------------------------------------------------------------------------

	 
uint8_t dutyAnteriorLadoEsq_g = 0;
uint8_t dutyAnteriorLadoDir_g = 0;

typedef struct{
	volatile uint32_t PWM_Right;
	volatile  uint32_t PWM_Left ;
}JoyStickInfo;

//---------------------------------------------------------------------------

//Envia um valor entre 0-100%. 100% equivale a 180º
unsigned char CalculaAnguloServoLeft(
	 int16_t dutyLE
)
{	
	return ((unsigned char)fabs(((dutyLE/2) + 50)-100)); //x = 100*(y+100) / 200);
}

//---------------------------------------------------------------------------

//Envia um valor entre 0-100%. 100% equivale a 180º
unsigned char CalculaAnguloServoRight(
	int16_t dutyLD
)
{
	return ((unsigned char)((dutyLD/2) + 50)); //x = 100*(y+100) / 200
}

//---------------------------------------------------------------------------
/* 
 Controlling a servo position using a potentiometer (variable resistor) 
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Knob
*/

void TankDrive(
	JoyStick* joyStick
) 
{				
	int32_t eixoX = joyStick->valorEixoX;
	int32_t eixoY = joyStick->valorEixoY;
	
	uint32_t hypotenuse = sqrt(((eixoX*eixoX) + (eixoY*eixoY)));
	uint16_t angleDegrees = (uint16_t)((acos(fabs(eixoX)/hypotenuse))*(180/M_PI)); 
	
	joyStick->hypotenuse = hypotenuse;
	joyStick->angleDegrees = angleDegrees;
			 
	// Now angle indicates the measure of turn
	// Along a straight line, with an angle o, the turn co-efficient is same
	// this applies for angles between 0-90, with angle 0 the co-eff is -1
	// with angle 45, the co-efficient is 0 and with angle 90, it is 1
	 
	// And max of y or x is the movement
	int16_t mov = fmax(fabs(eixoY),fabs(eixoX));
	 
	//int16_t tcoeff = ((-100 + (angleDegrees*2.222222))); //(angleDegrees/90)*2*100
	
	int16_t tcoeff = (100 * angleDegrees) / 90;
	int16_t turn = (tcoeff / 100) * mov;
	//int16_t tcoeff = angleDegrees/90;
	//int16_t turn = ((tcoeff*0.01) * (fabs(fabs(eixoY) - fabs(eixoX))));

	// First and third quadrant
	mov = mov * 0.196; // converter em pwm = ValorLido/510 * 100
	turn  = turn * 0.196; 
	
	int16_t left, right;
	
	if( (eixoX >= 0 && eixoY >= 0) || (eixoX < 0 &&  eixoY < 0) ) {
		left = mov;
		right = turn;
		} else {
		right = mov;
		left = turn;
	}

	// Reverse polarity - Define o sentido
	if(eixoY < 0) {
		left = 0 - left;
		right = 0 - right;
	}
	
	//TRACEprintf("PWM: [%d, %d]", left, right);
	
	joyStick->dutyLE = left;
	joyStick->dutyLD = right;		
}

//---------------------------------------------------------------------------

/*
	Retorna o Valor do Eixo X ajustado no ponto inicial.
	o AD le valores entre 0 e 1023. Esta função retorna valores entre -512 e +512
*/
int16_t ValorEixoX(
	unsigned char adSelected
)
{
	uint16_t adcX = ADC_Read(adSelected);
	
	int16_t valor;
		
	if(!PontoXNaPosInic(adcX)) 
		valor = (adcX - PONTO_INIC_X);
	else
		valor = 0;
		
	return valor;	
}

//---------------------------------------------------------------------------

int16_t ValorEixoY(
	unsigned char adSelected
)
{
	uint16_t adcY = ADC_Read(adSelected);
	
	int16_t valor;
	if(!PontoYNaPosInic(adcY)) 
		valor = (adcY - PONTO_INIC_Y);
	else
		valor = 0;
	
	return valor;
}

//---------------------------------------------------------------------------

uint8_t PontoXNaPosInic(
	uint16_t valorEixoX
)
{
	return ((valorEixoX >= LIM_INF_PONTO_INIC_X ) && (valorEixoX <= LIM_SUP_PONTO_INIC_X));
}

//---------------------------------------------------------------------------

uint8_t PontoYNaPosInic(
	uint16_t valorEixoY
)
{
	
	return ((valorEixoY >= LIM_INF_PONTO_INIC_Y ) && (valorEixoY <= LIM_SUP_PONTO_INIC_Y));	
}

//---------------------------------------------------------------------------

unsigned char DirecaoCarro(
	JoyStick joy
)
{
	static TEstadoCarro estadoCarro_g = PARADO;
		
	unsigned char direcao;
	
	volatile uint8_t estaAndandoParaFrente = (((joy.valorEixoX != 0) && (joy.valorEixoY >= 0)) || (joy.valorEixoY > 0));
	volatile uint8_t estaAndandoParaTras = (joy.valorEixoY < 0);
	volatile uint8_t estaParado = (!estaAndandoParaFrente && !estaAndandoParaTras);
	
	switch(estadoCarro_g) {
		case PARADO:
			if (estaAndandoParaFrente) {
				direcao = 'F';
				estadoCarro_g = ANDANDO_FRENTE;			
			}
			
			else if (estaAndandoParaTras){
				direcao = 'T';
				estadoCarro_g = ANDANDO_TRAS;
			}
			
			else 
				direcao = 'P';
		break;	
		
		case ANDANDO_FRENTE:
			if (estaParado || estaAndandoParaTras) {
				direcao = 'P';
				estadoCarro_g = PARADO;
			}	
			else
				direcao = 'F';
		break;			
		
		case ANDANDO_TRAS:
			if (estaParado || estaAndandoParaFrente) {
				direcao = 'P';
				estadoCarro_g = PARADO;
			}
			
			else
				direcao = 'T';
		break;		
			
		default:
			direcao = 'P';
	}
	
	return direcao;
}

//---------------------------------------------------------------------------

uint8_t SoftStarterLadoEsq(
	uint8_t dutyAtual
)
{
	uint8_t incremento = 5;
	uint8_t incrementoInicial = 15;
	
	if((dutyAnteriorLadoEsq_g < incrementoInicial) && (dutyAtual > 0))//Abaixo desse valor o carrinho não anda
		dutyAtual = incrementoInicial; 
	else if(dutyAnteriorLadoEsq_g < dutyAtual)
		dutyAtual = dutyAnteriorLadoEsq_g + incremento;
	else if((dutyAnteriorLadoEsq_g > dutyAtual) && (dutyAtual != 0))
		dutyAtual = dutyAnteriorLadoEsq_g - incremento;
	
	dutyAnteriorLadoEsq_g = dutyAtual;
	
	if(dutyAtual > 100)
		dutyAtual = 100;
	
	return dutyAtual;
}

//---------------------------------------------------------------------------

uint8_t SoftStarterLadoDir(
	uint8_t dutyAtual
)
{
	uint8_t incremento = 5;
	uint8_t incrementoInicial = 15;
		
	if((dutyAnteriorLadoDir_g < incrementoInicial) && (dutyAtual > 0))//Abaixo desse valor o carrinho não anda. 
		dutyAtual = incrementoInicial;
	else if(dutyAnteriorLadoDir_g < dutyAtual)
		dutyAtual = dutyAnteriorLadoDir_g + incremento;
	else if((dutyAnteriorLadoDir_g > dutyAtual) && (dutyAtual != 0))
		dutyAtual =dutyAnteriorLadoDir_g - incremento;
	
	dutyAnteriorLadoDir_g = dutyAtual;
	
	if(dutyAtual > 100)
		dutyAtual = 100;
	
	return dutyAtual;
}


//---------------------------------------------------------------------------