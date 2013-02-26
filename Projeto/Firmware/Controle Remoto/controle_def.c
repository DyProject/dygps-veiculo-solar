#include "controle_def.h"

#include "macros_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;

unsigned char caracterConvertido_g[5];

//----------------------------------------------------------------------------

unsigned char DirecaoCarro(
	unsigned char sentido
)
{
	Usart_Transmit('{');
	Usart_Transmit(estadoCarro + 0x30);
	Usart_Transmit(sentido);
	Usart_Transmit('}');
	/*caso deve manter mesma direção envia 'z'*/
	unsigned char direcao = 'z';
	switch (estadoCarro) {
		case PARADO:
					if(sentido == 'F') {
						estadoCarro = ANDANDO_FRENTE;
						direcao = 'F';
					}					
					else if(sentido == 'R') {
						estadoCarro = ANDANDO_TRAS;		
						direcao = 'R';
					}								
					break;	
			
		case ANDANDO_FRENTE:		
					if(sentido == 'D') {
						estadoCarro = ANDANDO_DIREITA;
						direcao = 'D';
					}					
				
					else if(sentido == 'E') {
						estadoCarro = ANDANDO_ESQUERDA;		
						direcao = 'E';
					}
					
					else if(sentido != 'F') {
						estadoCarro = PARADO;
						direcao = 'P';
					}						
					break;
			
		case ANDANDO_TRAS:
					if(sentido != 'R') {
						estadoCarro = PARADO;
						direcao = 'P';	
					}						
					break;
			
		case ANDANDO_DIREITA:							
					if(sentido == 'F') {
						estadoCarro = ANDANDO_FRENTE;
						direcao = 'C';
					}
					
					if(sentido != 'D') {
						estadoCarro = PARADO;
						direcao = 'P';	
					}			
					break;
		
		case ANDANDO_ESQUERDA:
					if(sentido == 'F') {
						estadoCarro = ANDANDO_FRENTE;
						direcao = 'C';
					}
					
					if(sentido != 'E') {
						estadoCarro = PARADO;
						direcao = 'P';
					}			
							
					break;
		
		default:
					estadoCarro = PARADO;
					direcao = 'P';
	}		
	
	Usart_Transmit('[');
	Usart_Transmit(direcao);
	Usart_Transmit(']');
	
	return direcao;
}		

//----------------------------------------------------------------------------

void TransmitiBuffer(
	uint16_t dutyLadoEsq, 
	uint16_t dutyLadoDir, 
	unsigned char direcao
)
{
	unsigned char buffer[12];
		
	Usart_Transmit('[');
	buffer[0] = direcao;
	
	CvrtNum2CaracterIndividual(dutyLadoEsq);
	buffer[1] = 'M';
	buffer[2] = caracterConvertido_g[0];
	buffer[3] = caracterConvertido_g[1];
	buffer[4] = caracterConvertido_g[2];
	buffer[5] = caracterConvertido_g[3];
	
	CvrtNum2CaracterIndividual(dutyLadoDir);
	buffer[6] = 'M';
	buffer[7] = caracterConvertido_g[0];
	buffer[8] = caracterConvertido_g[1];
	buffer[9] = caracterConvertido_g[2];
	buffer[10] = caracterConvertido_g[3];
		
	buffer[11] = '\0';
	Usart_Write(buffer);
	
	Usart_Transmit(']');
		
	Usart_Transmit(' ');
	Usart_Transmit(' ');
}

//----------------------------------------------------------------------------

void CvrtNum2CaracterIndividual(
	uint16_t valor
)		
{											
	unsigned char cont = 3;
  
	caracterConvertido_g[0] = 0x30; 
	caracterConvertido_g[1] = 0x30; 
	caracterConvertido_g[2] = 0x30; 
	caracterConvertido_g[3] = 0x30;

	do{
		/*+0x30 para converter de caracter para número*/
		caracterConvertido_g[cont--] = (valor % 10) + 0x30;											
		valor /= 10;								
	}while (valor != 0);
}

//----------------------------------------------------------------------------
	