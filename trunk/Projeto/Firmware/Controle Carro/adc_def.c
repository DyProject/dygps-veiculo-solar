#include "adc_def.h"

//--------------------------------------------------------------------------------------------------

void ADC_Init () 
{
	/* ADMUX - ADC Multiplexer Selection Register
	* bit 7:6   01:	 R/W:	REFS[1:0]:	00=ARef,Vref interno desligado, 01=AVcc com capacitor externo no ARef
										10=Reservado, 11=1.1V tensão Ref interna com capacitor externo no ARef
    * bit 5		 0:	 R/W:	ADLAR:		0=Ajusta a direta valor da conversão, 1=Ajuste a esquerda
	* bit 4		 0:	 R:	    -:			Não usado deve ser sempre lido zero
	* bit 3:0 0000:	 R/W:	MUX[3:0]:	0000=ADC0, 0001=ADC1, 0010==ADC2, 0011=ADC3, 0100=ADC4, 0101=ADC5,
										0110=ADC6, 0111=ADC7, 1000=ADC8, 1001:1101=Reservado, 1110=1.1V(VBG)
										1111=0v(GND)
	*/
	ADMUX = 0b01000000; 
	//        ||||||||
	//		  76543210	
	
	/* ADCSRA - ADC Control and Status Register A
	* bit 7		0:	R/W:	ADEN:			0=Des. ADC, 1=Habilita ADC
	* bit 6		0:	R/W:	ADSC:			Modo Simples: 0=Sem efeito, 1=Inicia conversão
											Modo Livre:   0=Sem efeito, 1=Inicia primeira conversão
	* bit 5		0:	R/W:	ADATE:			0=Des, 1=Habilita Auto Trigger
	* bit 4		0:	R/W:	ADIF:			0R=, 1R=Conversão completa flag interrupção
											1W=Limpa o flag de inter. completa
	* bit 3		0:	R/W:	ADIE:			0=Des., 1=Habilita interrupção
	* bit 2:0 000:	R/W:	ADPS[2:0]:		000=Prescaler 2,  001==Prescaler 2, 010=Prescaler 4
											011=Prescaler 8,  100=Prescaler 16. 101=Prescaler 32
											110=Prescaler 64, 111=Prescaler 128
	Exemplo: prescaler de 128: 20MHz/128 = 156,250KHz
		OBS: ADC[ADCL-ADCH] Registrado que armazena o resultado. Se o ADCL é lido o registrador 
		não é atualizado até ADCH ser lido. Se ADLAR for setado(ajuste a esquerda) e a precisão
		não for maior que 8bits apenas a leitura do ADCH é o suficiente.
	*/	
	ADCSRA = 0b10000111;  
	//         ||||||||
	//		   76543210

	/* ADCSRB - ADC Control and Status Register B
	* bit 7		0:	R:		-:			Reservado. Para uso futuro. Dever ser sempre escirto zero.
	* bit 6		0:	R/W:	ACME:			
	* bit 5:3  00:	R:		-:			Reservado. Para uso futuro. Dever ser sempre escirto zero.
	* bit 2:0 000:	R/W:	ADTS[2:0]:	Modo Auto Trigger se ADEN = 1, senão não tem efeito
										000=Free Running mode,  001==Analog Comparator, 
										010=External Interrupt Request 0, 011=Timer/Counter0 Compare Match A,  
										100=Timer/Counter0 Overflow, 101=Timer/Counter1 Compare Match B
										110=Timer/Counter1 Overflow, 111=Timer/Counter1 Capture Even
	*/	
	ADCSRB = 0b00000000;  
	//         ||||||||
	//		   76543210


	/* DIDR0 - Digital Input Disable Register 0
	* bit 7:6	00:		R:		-:				Reservado. Para uso futuro. Dever ser sempre escirto zero.
	* bit 5:0	111111:	R/W:	ADCnD[5:0]:		0=Habilita, 1=Entrada digital desabilitada

	*/	
	DIDR0 = 0b00111111;  
	//        ||||||||
	//		  76543210
}

//--------------------------------------------------------------------------------------------------

uint16_t ADC_Read(
	uint8_t canal
)
{
	/*Máscara para definir o canal. Bits2:0*/
	ADMUX = (ADMUX & 0b11111000)  | (canal & 0b0000111);
	
	/*Inicia conversão*/
	ADCSRA |= (1<<ADSC);
	
	/*Aguarda conversão ser concluída*/
	while(ADCSRA & (1<<ADSC));
		
	return (ADC);
}

//--------------------------------------------------------------------------------------------------
