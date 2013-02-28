#include "controle_def.h"

#include "globals_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;
unsigned char caracterConvertido_g[5];

//----------------------------------------------------------------------------

void TransmitiBuffer(
	uint8_t dutyLadoEsq, 
	uint8_t dutyLadoDir, 
	unsigned char direcao
)
{
	unsigned char buffer[5];
	
	buffer[0] = 'S';	
	buffer[1] = direcao;
	buffer[2] = dutyLadoEsq;
	buffer[3] = dutyLadoDir;
	buffer[4] = '\0';
	Usart_Write(buffer);
}

//----------------------------------------------------------------------------

