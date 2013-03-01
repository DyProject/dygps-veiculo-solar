#include "globals_def.h"

//----------------------------------------------------------------------------

void CvrtNum2CaracterIndividual(
	uint16_t valor, 
	unsigned char* caracterConvertido,
	uint8_t tamanho
)		
{											
	uint8_t contIni;
	for(contIni = 0; contIni < (tamanho - 1); contIni++)
		*(caracterConvertido + contIni) = 0x30; 
	*(caracterConvertido + contIni + 1) = '\0';
	
	unsigned char cont = tamanho - 1;
	do{
		/*+0x30 para converter de caracter para número*/
		caracterConvertido[cont--] = (valor % 10) + 0x30;											
		valor /= 10;								
	}while (valor != 0);
}

//----------------------------------------------------------------------------