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

uint16_t CalculaDutyCycleLadoDir()
{
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY();
	uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
	
	unsigned char convertido[5];
	Usart_Transmit('[');
	CvrtNum2CaracterIndividual(valorLidoADEixoX, convertido);
	Usart_Write(convertido);
	Usart_Transmit(',');
	CvrtNum2CaracterIndividual(valorLidoADEixoY, convertido);
	Usart_Write(convertido);
	Usart_Transmit(']');
	
	uint16_t duty = 0;
	/*Andando Reverse*/
	if (valorLidoADEixoY < limInfPontoInicY_g) 
		duty = 100;
		
	/*Andando reto frente*/
	else if (PontoXNaPosInic() && !PontoYNaPosInic())
		duty = 100;
			
	/*Andando para direita*/
	else if ((valorLidoADEixoX > limSupPontoInicX_g)) 
		duty = 0;
	
	/*Andando para esquerda*/
	else if ((valorLidoADEixoX < limInfPontoInicX_g)) 
		duty = 100;
		
	return duty;
}

//---------------------------------------------------------------------------

uint16_t CalculaDutyCycleLadoEsq()
{
	uint16_t valorLidoADEixoX = ValorLidoADEixoX(); 
	uint16_t valorLidoADEixoY = ValorLidoADEixoY();
	uint8_t valorPorCentoEixoY = CalculaPorcentoPosicaoEixoY(valorLidoADEixoY);
	
	uint16_t duty = 0;
	/*Andando Reverse*/
	if (valorLidoADEixoY < limInfPontoInicY_g) 
		duty = 100;
		
	/*Andando reto frente*/
	else if (PontoXNaPosInic() && !PontoYNaPosInic())
		duty = 100;
			
	/*Andando para direita*/
	else if ((valorLidoADEixoX > limSupPontoInicX_g)) 
		duty = 100;
	
	/*Andando para esquerda*/
	else if ((valorLidoADEixoX < limInfPontoInicX_g)) 
		duty = 0;
		
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