#include "joyStick.h"

#include "adc_def.h"

#include <math.h>

//---------------------------------------------------------------------------

uint16_t pontoInicX_g = 522,
		 limInfPontoInicX_g = 512,
		 limSupPontoInicX_g = 532;	
uint16_t pontoInicY_g = 498,
		 limInfPontoInicY_g = 488,
		 limSupPontoInicY_g = 508;

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
	
	if(valorLidoADEixoY > limSupPontoInicY_g)
		direcao = 'F';
		
	else if(valorLidoADEixoY < limInfPontoInicY_g)
		direcao = 'R';
	
	else if(valorLidoADEixoX > limSupPontoInicX_g)
		direcao = 'D';
		
	else if(valorLidoADEixoX < limInfPontoInicX_g)
		direcao = 'E';	
		
	else 
		direcao = 'P';
			
	return direcao;
}

//---------------------------------------------------------------------------

void CalculaDutyCycle(
	uint16_t* dutyLadoEsq, 
	uint16_t* dutyLadoDir
)
{
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY();
	uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
	
	/*Andando Reverse*/
	if (valorLidoADEixoY < limInfPontoInicY_g) {
		*dutyLadoDir = valorPorCentoEixoY;
		*dutyLadoEsq = valorPorCentoEixoY;	
		Usart_Transmit('x');
	}		
		
	/*Andando reto frente*/
	else if (PontoXNaPosInic() && !PontoYNaPosInic()) {
		*dutyLadoDir = valorPorCentoEixoY;
		*dutyLadoEsq = valorPorCentoEixoY;
	}	
			
	/*Andando para direita*/
	else if ((valorLidoADEixoX > limSupPontoInicX_g)) {
		*dutyLadoDir = ceil(valorPorCentoEixoY * (100 - valorPorCentoEixoY)  / 100);
		*dutyLadoEsq = valorPorCentoEixoY;	
	}
	
	/*Andando para esquerda*/
	else if ((valorLidoADEixoX < limInfPontoInicX_g)) {
		*dutyLadoDir = valorPorCentoEixoY;
		*dutyLadoEsq = ceil(valorPorCentoEixoY * (100 - valorPorCentoEixoY)  / 100);
	}		
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
			 posYFrente0PorCento = 522;
		 
	uint8_t valorPorCentoEixoY; 
	
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