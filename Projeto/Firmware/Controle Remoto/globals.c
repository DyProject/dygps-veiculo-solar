#include "globals_def.h"

void CvrtNum2CaracterIndividual(
	uint16_t valor, 
	unsigned char* caracterConvertido
)		
{											
	unsigned char cont = 3;
  
	caracterConvertido[0] = 0x30; 
	caracterConvertido[1] = 0x30; 
	caracterConvertido[2] = 0x30; 
	caracterConvertido[3] = 0x30;
	caracterConvertido[4] = '\0';

	do{
		/*+0x30 para converter de caracter para número*/
		caracterConvertido[cont--] = (valor % 10) + 0x30;											
		valor /= 10;								
	}while (valor != 0);
}