#include "joyStick.h"

#include <math.h>

#include "adc_def.h"
#include "globals_def.h"
#include "stdio.h"
#include "LCD4b.h"

//---------------------------------------------------------------------------

uint16_t pontoInicX_g = 522,
		 limInfPontoInicX_g = 478,
		 limSupPontoInicX_g = 542;	
uint16_t pontoInicY_g = 498,
		 limInfPontoInicY_g = 478,
		 limSupPontoInicY_g = 542;
		 
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
	//Ajuste do zero
	int32_t eixoX = joyStick->valorEixoX;
	int32_t eixoY = joyStick->valorEixoY;
	
	//TRACEprintf("\n[%ld, %ld] \n", eixoX, eixoY);
	
	//trace
	//int32_t eixoX = ((int32_t)ValorLidoADEixoX()) - 510;
	//int32_t eixoY = ((int32_t)ValorLidoADEixoY()) - 506;
	
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
	 
	int16_t tcoeff = ((-100 + (angleDegrees*2.222222))); //(angleDegrees/90)*2*100
	int16_t turn = ((tcoeff*0.01) * (fabs(fabs(eixoY) - fabs(eixoX))));

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

uint16_t ValorLidoADEixoX(
	unsigned char adSelected
)
{
	uint16_t adcX = ADC_Read(adSelected);
		
	return adcX;	
}

//---------------------------------------------------------------------------

uint16_t ValorLidoADEixoY(
	unsigned char adSelected
)
{
	uint16_t adcY = ADC_Read(adSelected);
	
	return adcY;	
}

//---------------------------------------------------------------------------

uint8_t PontoXNaPosInic(
	unsigned char adSelected
)
{
	return ((ValorLidoADEixoX(adSelected) >= limInfPontoInicX_g ) && (ValorLidoADEixoX(adSelected) <= limSupPontoInicX_g));
}

//---------------------------------------------------------------------------

uint8_t PontoYNaPosInic(
	unsigned char adSelected
)
{
	return ((ValorLidoADEixoY(adSelected) >= limInfPontoInicY_g ) && (ValorLidoADEixoY(adSelected) <= limSupPontoInicY_g));	
}

//---------------------------------------------------------------------------

unsigned char DirecaoCarro()
{
	static TEstadoCarro estadoCarro_g = PARADO;
	unsigned char sentido = CalculaSentido();
		
	unsigned char direcao = 'P';
	switch(estadoCarro_g) {
		case PARADO:
			if((sentido == 'F') || (sentido == 'D') || (sentido == 'E')) {
				direcao = 'F';
				estadoCarro_g = ANDANDO_FRENTE;
			}				
			else if((sentido == 'T') || (sentido == 'R') || (sentido == 'L')) {
				direcao = 'T';
				estadoCarro_g = ANDANDO_TRAS;
			}else direcao = 'P';
			break;	
		
		case ANDANDO_FRENTE:
			if ((sentido == 'T') || (sentido == 'R') || (sentido == 'L') || (sentido == 'P')) {
				direcao = 'P';
				estadoCarro_g = PARADO;
			}else direcao = 'F';	
			break;			
		
		case ANDANDO_TRAS:
			if ((sentido == 'F') || (sentido == 'D') || (sentido == 'E') || (sentido == 'P')) {
				direcao = 'P';
				estadoCarro_g = PARADO;
			}else direcao = 'T';	
			break;		
			
		default:
			direcao = 'P';
	}
	
	return direcao;
}

//---------------------------------------------------------------------------

unsigned char CalculaSentido()
{
	unsigned char direcao = 'P';
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(AD_EIXO_X_DIR); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY(AD_EIXO_Y_DIR);
	
	/*Andando Frente*/		
	if((valorLidoADEixoY > limSupPontoInicY_g) && PontoXNaPosInic(AD_EIXO_X_DIR))
		direcao = 'F';
	
	/*Andando Frente Direita*/
	else if((valorLidoADEixoX > limSupPontoInicX_g) && 
			((valorLidoADEixoY > limSupPontoInicY_g) || PontoYNaPosInic(AD_EIXO_Y_DIR)))
		direcao = 'D';
		
	/*Andando Frente Esquerda*/	
	else if((valorLidoADEixoX < limInfPontoInicX_g) && 
			((valorLidoADEixoY > limSupPontoInicY_g) ||  PontoYNaPosInic(AD_EIXO_Y_DIR)))
		direcao = 'E';	
		
	/*Andando Tras*/
	else if((valorLidoADEixoY < limInfPontoInicY_g) && PontoXNaPosInic(AD_EIXO_X_DIR))
		direcao = 'T';
		
	/*Andando Tras Direita*/
	else if((valorLidoADEixoX > limSupPontoInicX_g) && (valorLidoADEixoY < limInfPontoInicY_g))
		direcao = 'R';
		
	/*Andando Tras Esquerda*/	
	else if((valorLidoADEixoX < limInfPontoInicX_g) && (valorLidoADEixoY < limInfPontoInicY_g))
		direcao = 'L';	
		
	else 
		direcao = 'P';
		
	return direcao;
}

//---------------------------------------------------------------------------

uint8_t CalculaDutyCycleLadoDir()
{
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(AD_EIXO_X_DIR); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY(AD_EIXO_Y_DIR);
	//uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
		
	uint8_t duty = 0;
	unsigned char sentido = CalculaSentido();
	switch(sentido) {
		case 'F'://Andando Frente
		case 'T'://Andando Tras
		
			if(dutyAnteriorLadoDir_g < 75)
				duty = SoftStarterLadoDir(CalculaPorcentoPosicaoEixoY(valorLidoADEixoY));
			else duty = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
		
			break;
				
		case 'E'://Andando Frente Esquerda
		case 'L'://Andando Tras Esquerda
			if(dutyAnteriorLadoDir_g < 75)
				duty = SoftStarterLadoDir(100);
			else duty = 100;
			
			break;
		case 'R': //Andando Tras Direita
		case 'D'://Andando Frente Direita
			if(dutyAnteriorLadoDir_g < 75)
				duty = SoftStarterLadoDir(100 - CalculaPorcentoPosicaoEixoX(valorLidoADEixoX));
			else duty = (100 - CalculaPorcentoPosicaoEixoX(valorLidoADEixoX));
			
			
			break;
		case 'P'://Parado
			duty = 0;
			break;
	}
	
	dutyAnteriorLadoDir_g = duty;
		
	return duty;
}

//---------------------------------------------------------------------------

uint8_t SoftStarterLadoEsq(
	uint8_t dutyAtual
)
{
	uint8_t incremento = 5;
	uint8_t incrementoInicial = 15;
	
	if(dutyAnteriorLadoEsq_g < incrementoInicial )//Abaixo desse valor o carrinho não anda
		dutyAtual = incrementoInicial; 
	else if(dutyAnteriorLadoEsq_g < dutyAtual)
		dutyAtual = dutyAnteriorLadoEsq_g + incremento;
	else if(dutyAnteriorLadoEsq_g > dutyAtual)
		dutyAtual = dutyAnteriorLadoEsq_g - incremento;
		
	return dutyAtual;
}

//---------------------------------------------------------------------------

uint8_t SoftStarterLadoDir(
uint8_t dutyAtual
)
{
	uint8_t incremento = 5;
	uint8_t incrementoInicial = 15;
		
	if((dutyAnteriorLadoDir_g < incrementoInicial))//Abaixo desse valor o carrinho não anda. 
		dutyAtual = incrementoInicial;
	else if(dutyAnteriorLadoDir_g < dutyAtual)
		dutyAtual = dutyAnteriorLadoDir_g + incremento;
	else if(dutyAnteriorLadoDir_g > dutyAtual)
		dutyAtual =dutyAnteriorLadoDir_g - incremento;
	
	return dutyAtual;
}

//---------------------------------------------------------------------------

uint8_t CalculaDutyCycleLadoEsq()
{
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(AD_EIXO_X_DIR); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY(AD_EIXO_Y_DIR);
	//uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
	
	uint8_t duty = 0;
	unsigned char sentido = CalculaSentido();
	
	switch(sentido) {
		case 'F'://Andando Frente
		case 'T'://Andando Tras
			//trace
			
			if(dutyAnteriorLadoEsq_g < 75)
				duty = SoftStarterLadoEsq(CalculaPorcentoPosicaoEixoY(valorLidoADEixoY));
			else duty = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
					
			//EndTrace
			//duty = SoftStarterLadoEsq(CalculaPorcentoPosicaoEixoY(valorLidoADEixoY));
			break;
		case 'D'://Andando Frente Direita
		case 'R'://Andando Tras Direita
			if(dutyAnteriorLadoEsq_g < 75)
				duty = SoftStarterLadoEsq(100);
			else duty = 100;
			break;
		case 'E'://Andando Frente Esquerda
		case 'L': //Andando Tras Esquerda
			if(dutyAnteriorLadoEsq_g < 75)
				duty = SoftStarterLadoEsq(100 - CalculaPorcentoPosicaoEixoX(valorLidoADEixoX));
			else duty = (100 -CalculaPorcentoPosicaoEixoX(valorLidoADEixoX));
			break;
		case 'P'://Parado
			duty = 0;
			break;
	}
	
	dutyAnteriorLadoEsq_g = duty;
	
	return duty;
}

//---------------------------------------------------------------------------

uint8_t CalculaPorcentoPosicaoEixoY(
	uint16_t valorLidoADEixoY
) 
{
	/*Valores Empíricos*/
	uint16_t //posYFrente100PorCento = 1023,
			 posYFrente75PorCento = 900,
			 posYFrente50PorCento = 775,
			 posYFrente25PorCento = 650,
			 //posYTras100PorCento = 0,
			 posYTras75PorCento = 300,
			 posYTras50PorCento = 390,
			 posYTras25PorCento = 480;
			 //posY0PorCento = 522;		
		 
	uint8_t valorPorCentoEixoY; 
	
	if ((valorLidoADEixoY > posYFrente75PorCento) || (valorLidoADEixoY < posYTras75PorCento)) 
		valorPorCentoEixoY = 100;
				
	else if((valorLidoADEixoY > posYFrente50PorCento) || (valorLidoADEixoY < posYTras50PorCento)) 
		valorPorCentoEixoY = 75;
		
	else if((valorLidoADEixoY > posYFrente25PorCento) || (valorLidoADEixoY < posYTras25PorCento)) 
		valorPorCentoEixoY = 50;
	
	else if(PontoYNaPosInic(AD_EIXO_Y_DIR))
		valorPorCentoEixoY = 0;
			
	else  
		valorPorCentoEixoY = 25;
				
	return valorPorCentoEixoY;
}

//---------------------------------------------------------------------------

uint8_t CalculaPorcentoPosicaoEixoX(
	uint16_t valorLidoADEixoX
) 
{
	/*Valores Empíricos*/
	uint16_t //posXFrente100PorCento = 1023,
			 posXFrente75PorCento = 900,
			 posXFrente50PorCento = 775,
			 posXFrente25PorCento = 650,
			 //posXTras100PorCento = 0,
			 posXTras75PorCento = 300,
			 posXTras50PorCento = 390,
			 posXTras25PorCento = 480;
			 //posX0PorCento = 498;
		 
	uint16_t valorPorCentoEixoX; 
	
	if ((valorLidoADEixoX > posXFrente75PorCento) ||  (valorLidoADEixoX < posXTras75PorCento) ) 
		valorPorCentoEixoX = 100;
				
	else if((valorLidoADEixoX > posXFrente50PorCento) || (valorLidoADEixoX < posXTras50PorCento)) 
		valorPorCentoEixoX = 75;
		
	else if((valorLidoADEixoX > posXFrente25PorCento) || (valorLidoADEixoX < posXTras25PorCento) ) 
		valorPorCentoEixoX = 50;
				
	else if(PontoXNaPosInic(AD_EIXO_X_DIR))
		valorPorCentoEixoX = 0;
	
	else 
		valorPorCentoEixoX = 25;
	
	return valorPorCentoEixoX;
}

//---------------------------------------------------------------------------