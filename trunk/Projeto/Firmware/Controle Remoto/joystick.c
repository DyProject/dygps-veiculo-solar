#include "joyStick.h"

#include <math.h>

#include "adc_def.h"

//---------------------------------------------------------------------------

typedef enum {
	PARADO = 1, ANDANDO_FRENTE, ANDANDO_TRAS, ANDANDO_FRENTE_DIREITA, ANDANDO_FRENTE_ESQUERDA,
	ANDANDO_TRAS_DIREITA, ANDANDO_TRAS_ESQUERDA 
	}TEstadoCarro;
	
volatile TEstadoCarro estadoCarro_g = PARADO;

uint16_t pontoInicX_g = 522,
		 limInfPontoInicX_g = 502,
		 limSupPontoInicX_g = 542;	
uint16_t pontoInicY_g = 498,
		 limInfPontoInicY_g = 478,
		 limSupPontoInicY_g = 518;

//---------------------------------------------------------------------------

uint16_t ValorLidoADEixoX()
{
	uint16_t adcX = ADC_Read(AD_EIXO_X);
	
	unsigned char caracConvertido[5];
	CvrtNum2CaracterIndividual(adcX, caracConvertido);
	LCD_setPos(2,0);
	escreve_LCD("X: ");
	escreve_LCD(caracConvertido);
		
	return adcX;	
}

//---------------------------------------------------------------------------

uint16_t ValorLidoADEixoY()
{
	uint16_t adcY = ADC_Read(AD_EIXO_Y);
	
	unsigned char caracConvertido[5];
	CvrtNum2CaracterIndividual(adcY, caracConvertido);
	LCD_setPos(2,8);
	escreve_LCD("Y: ");
	escreve_LCD(caracConvertido);
	
	return adcY;	
}

//---------------------------------------------------------------------------

uint8_t PontoXNaPosInic()
{
	return ((ValorLidoADEixoX() >= limInfPontoInicX_g ) && (ValorLidoADEixoX() <= limSupPontoInicX_g));
}

//---------------------------------------------------------------------------

uint8_t PontoYNaPosInic()
{
	return ((ValorLidoADEixoY() >= limInfPontoInicY_g ) && (ValorLidoADEixoY() <= limSupPontoInicY_g));	
}

//---------------------------------------------------------------------------

unsigned char DirecaoCarro()
{
	unsigned char sentido = CalculaSentido();
	unsigned char direcao;
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
	}
}

//---------------------------------------------------------------------------

unsigned char CalculaSentido()
{
	unsigned char direcao;
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY();
	
	/*Andando Frente*/		
	if((valorLidoADEixoY > limSupPontoInicY_g) && PontoXNaPosInic())
		direcao = 'F';
	
	/*Andando Frente Direita*/
	else if((valorLidoADEixoX > limSupPontoInicX_g) && 
			((valorLidoADEixoY > limSupPontoInicY_g) || PontoYNaPosInic()))
		direcao = 'D';
		
	/*Andando Frente Esquerda*/	
	else if((valorLidoADEixoX < limInfPontoInicX_g) && 
			((valorLidoADEixoY > limSupPontoInicY_g) ||  PontoYNaPosInic()))
		direcao = 'E';	
		
	/*Andando Tras*/
	else if((valorLidoADEixoY < limInfPontoInicY_g) && PontoXNaPosInic())
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
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY();
	uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
		
	uint8_t duty = 0;
	unsigned char sentido = CalculaSentido();
	switch(sentido) {
		case 'F'://Andando Frente
		case 'T'://Andando Tras
			duty = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
			break;
				
		case 'E'://Andando Frente Esquerda
		case 'L'://Andando Tras Esquerda
			duty = 100;
			break;
		case 'R': //Andando Tras Direita
		case 'D'://Andando Frente Direita
			duty = 100 - CalculaPorcentoPosicaoEixoX(valorLidoADEixoX);
			break;
		case 'P'://Parado
			duty = 0;
			break;
	}
		
	return duty;
}

//---------------------------------------------------------------------------

uint8_t CalculaDutyCycleLadoEsq()
{
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY();
	uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
	
	uint8_t duty = 0;
	unsigned char sentido = CalculaSentido();
	switch(sentido) {
		case 'F'://Andando Frente
		case 'T'://Andando Tras
			duty = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
			break;
		case 'D'://Andando Frente Direita
		case 'R'://Andando Tras Direita
			duty = 100;
			break;
		case 'E'://Andando Frente Esquerda
		case 'L': //Andando Tras Esquerda
			duty = 100 - CalculaPorcentoPosicaoEixoX(valorLidoADEixoX);
			break;
		case 'P'://Parado
			duty = 0;
			break;
	}
	return duty;
}

//---------------------------------------------------------------------------

uint8_t CalculaPorcentoPosicaoEixoY(
	uint16_t valorLidoADEixoY
) 
{
	/*Valores Empíricos*/
	uint16_t posYFrente100PorCento = 1023,
			 posYFrente75PorCento = 900,
			 posYFrente50PorCento = 775,
			 posYFrente25PorCento = 650,
			 posYTras100PorCento = 0,
			 posYTras75PorCento = 300,
			 posYTras50PorCento = 390,
			 posYTras25PorCento = 480,
			 posY0PorCento = 522;		
		 
	uint8_t valorPorCentoEixoY; 
	
	if ((valorLidoADEixoY > posYFrente75PorCento) || (valorLidoADEixoY < posYTras75PorCento)) 
		valorPorCentoEixoY = 100;
				
	else if((valorLidoADEixoY > posYFrente50PorCento) || (valorLidoADEixoY < posYTras50PorCento)) 
		valorPorCentoEixoY = 75;
		
	else if((valorLidoADEixoY > posYFrente25PorCento) || (valorLidoADEixoY < posYTras25PorCento)) 
		valorPorCentoEixoY = 50;
	
	else if(PontoYNaPosInic())
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
	uint16_t posXFrente100PorCento = 1023,
			 posXFrente75PorCento = 900,
			 posXFrente50PorCento = 775,
			 posXFrente25PorCento = 650,
			 posXTras100PorCento = 0,
			 posXTras75PorCento = 300,
			 posXTras50PorCento = 390,
			 posXTras25PorCento = 480,
			 posX0PorCento = 498;
		 
	uint16_t valorPorCentoEixoX; 
	
	if ((valorLidoADEixoX > posXFrente75PorCento) ||  (valorLidoADEixoX < posXTras75PorCento) ) 
		valorPorCentoEixoX = 100;
				
	else if((valorLidoADEixoX > posXFrente50PorCento) || (valorLidoADEixoX < posXTras50PorCento)) 
		valorPorCentoEixoX = 75;
		
	else if((valorLidoADEixoX > posXFrente25PorCento) || (valorLidoADEixoX < posXTras25PorCento) ) 
		valorPorCentoEixoX = 50;
				
	else if(PontoXNaPosInic())
		valorPorCentoEixoX = 0;
	
	else 
		valorPorCentoEixoX = 25;
	
	return valorPorCentoEixoX;
}

//---------------------------------------------------------------------------