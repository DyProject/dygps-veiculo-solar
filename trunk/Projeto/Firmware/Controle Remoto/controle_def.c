#include "controle_def.h"

#include "globals_def.h"
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
	unsigned char direcao ;
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
					else direcao = 'P';				
					
					break;	
			
		case ANDANDO_FRENTE:		
					if(sentido != 'F') {
						estadoCarro = PARADO;
						direcao = 'P';
					}		
					else direcao = 'F';				
					
					break;
			
		case ANDANDO_TRAS:
					if(sentido != 'R') {
						estadoCarro = PARADO;
						direcao = 'P';	
					}	
					else direcao = 'R';		
					
					break;
		
		default:
					estadoCarro = PARADO;
					direcao = 'P';
	}		
	
	return direcao;
}		

//----------------------------------------------------------------------------

void TransmitiBuffer(
	uint16_t dutyLadoEsq, 
	uint16_t dutyLadoDir, 
	unsigned char direcao
)
{
	unsigned char buffer[10];
		
	Usart_Transmit('[');
	buffer[0] = direcao;
	
	CvrtNum2CaracterIndividual(dutyLadoEsq, caracterConvertido_g);
	buffer[1] = 'M';
	//tracebuffer[2] = caracterConvertido_g[0];
	buffer[2] = caracterConvertido_g[1];
	buffer[3] = caracterConvertido_g[2];
	buffer[4] = caracterConvertido_g[3];
	
	CvrtNum2CaracterIndividual(dutyLadoDir, caracterConvertido_g);
	buffer[5] = 'M';
	//tracebuffer[6] = caracterConvertido_g[0];
	buffer[6] = caracterConvertido_g[1];
	buffer[7] = caracterConvertido_g[2];
	buffer[8] = caracterConvertido_g[3];
		
	buffer[9] = '\0';
	Usart_Write(buffer);
	
	Usart_Transmit(']');	
}

//----------------------------------------------------------------------------
