#include "controle_def.h"

#include "macros_def.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;

//----------------------------------------------------------------------------

void EnviaDirecaoCarro()
{
	switch (estadoCarro) {
		case PARADO:
			if(BT_FRENTE_ISPRESSED()) {
				Usart_Transmit('F');
				estadoCarro = ANDANDO_FRENTE;
			}else if(BT_RE_ISPRESSED()) {
				Usart_Transmit('R');
				estadoCarro = ANDANDO_TRAS;	
			}			
			break;	
			
		case ANDANDO_FRENTE:
			if(!BT_FRENTE_ISPRESSED()) {
				Usart_Transmit('P');
				estadoCarro = PARADO;
				
			} else if(BT_DIREITA_ISPRESSED()) {
				Usart_Transmit('D');
				estadoCarro = ANDANDO_DIREITA;
				
			} else if(BT_ESQUERDA_ISPRESSED()) {
				Usart_Transmit('E');
				estadoCarro = ANDANDO_ESQUERDA;	
			}	
			break;
			
		case ANDANDO_TRAS:
			if(!BT_RE_ISPRESSED()) {
				Usart_Transmit('P');
				estadoCarro = PARADO;
			}			
			break;
			
		case ANDANDO_DIREITA:
			if(!BT_FRENTE_ISPRESSED()) {
				Usart_Transmit('P');
				estadoCarro = PARADO;
								
			} else if(!BT_DIREITA_ISPRESSED()) {
				Usart_Transmit('C');
				estadoCarro = ANDANDO_FRENTE;
			}	
			break;
		
		case ANDANDO_ESQUERDA:
			if(!BT_FRENTE_ISPRESSED()) {
				Usart_Transmit('P');
				estadoCarro = PARADO;
								
			} else if(!BT_ESQUERDA_ISPRESSED()) {
				Usart_Transmit('C');
				estadoCarro = ANDANDO_FRENTE;
			}
			break;
	}		
}		
//----------------------------------------------------------------------------