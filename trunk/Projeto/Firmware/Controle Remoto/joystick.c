#include "joyStick.h"

#include "adc_def.h"

#include <math.h>

//---------------------------------------------------------------------------

uint16_t pontoInicX_g = 522,
		 limInfPontoInicX_g = 502,
		 limSupPontoInicX_g = 542;	
uint16_t pontoInicY_g = 498,
		 limInfPontoInicY_g = 478,
		 limSupPontoInicY_g = 518;

//---------------------------------------------------------------------------

uint16_t ValorLidoADEixoX()
{
	return ADC_Read(AD_EIXO_X);	
}

//---------------------------------------------------------------------------

uint16_t ValorLidoADEixoY()
{
	return ADC_Read(AD_EIXO_Y);	
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
		case 'E'://Andando Frente Esquerda
		case 'T'://Andando Tras
		case 'L'://Andando Tras Esquerda
			duty = 100;
			break;
		
		case 'D'://Andando Frente Direita
		case 'R': //Andando Tras Direita
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
		case 'D'://Andando Frente Direita
		case 'T'://Andando Tras
		case 'R'://Andando Tras Direita
			duty = 100;
			break;
		
		case 'E'://Andando Frente Esquerda
		case 'L': //Andando Tras Esquerda
		case 'P'://Parado
			duty = 0;
			break;
	}
	return duty;
}

//---------------------------------------------------------------------------

uint16_t CalculaPorcentoPosicaoEixoY(
	uint16_t valorLidoADEixoY
) 
{
	/*Valores Empíricos*/
	uint16_t posYFrente100PorCento = 1023,
			 posYFrente75PorCento = 900,
			 posYFrente50PorCento = 775,
			 posYFrente25PorCento = 650,
			 posYFrente0PorCento = 522;
		 
	uint16_t valorPorCentoEixoY; 
	
	if (ValorLidoADEixoY() > posYFrente75PorCento) 
		valorPorCentoEixoY = 100;
				
	else if(ValorLidoADEixoY() > posYFrente50PorCento) 
		valorPorCentoEixoY = 75;
		
	else if(ValorLidoADEixoY() > posYFrente25PorCento) 
		valorPorCentoEixoY = 50;
		
	else if(ValorLidoADEixoY() > posYFrente0PorCento) 
		valorPorCentoEixoY = 25;
	
	else 
		valorPorCentoEixoY = 0;
	
	return valorPorCentoEixoY;
}

//---------------------------------------------------------------------------