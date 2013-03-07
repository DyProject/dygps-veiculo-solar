#include "controle_def.h"

#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;
unsigned char caracterConvertido_g[5];

BufferRecep bufferRecepcao_g;
DadosLCD dadosLCD;

//----------------------------------------------------------------------------

void TransmitiBuffer()
{
	unsigned char buffer[5];
	
	buffer[0] = 'S';	
	buffer[1] = dadosLCD.direcao;
	buffer[2] = dadosLCD.dutyLadoEsq;
	buffer[3] = dadosLCD.dutyLadoDir;
	buffer[4] = '\0';
	
	Usart_Transmit(buffer[0]);
	Usart_Transmit(buffer[1]);
	Usart_Transmit(buffer[2]);
	Usart_Transmit(buffer[3]);
}

//----------------------------------------------------------------------------

void Inicializacoes()
{
	Usart_Init(MYUBRR);
	
	//Prescaler do Timer0, usado para fazer uma leitura do ADC.
	TCCR0B = (1<<CS02) | (1<<CS00);
	
	/*Configurações de inicialização d do buffer*/
	bufferRecepcao_g.qntdDadosLido = 0;
	bufferRecepcao_g.iniciado = 0;
	bufferRecepcao_g.completo = 0;
	
	inic_LCD_4bits();					
	ADC_Init();
}

//----------------------------------------------------------------------------

void MostraDadosLCD()
{	
	/*Imprimi tudo na primeira vez*/
	static uint8_t contador= 20;
	unsigned char caracConvertido[3];
	
	Limpa_matriz_LCD(1, 6, 1);
	Limpa_matriz_LCD(2, 6, 1);	
		
	LCD_setPos(1,3);
	CvrtNum2CaracterIndividual(dadosLCD.dutyLadoEsq	, caracConvertido, 3);
	escreve_LCD(caracConvertido);
	
	LCD_setPos(2,3);
	CvrtNum2CaracterIndividual(dadosLCD.dutyLadoDir, caracConvertido, 3);
	escreve_LCD(caracConvertido);
	
	/*Para o display não ficar piscando*/
	if(contador > 16){
		
		//Limpa_matriz_LCD(1, 0, 16);//teste
		//Limpa_matriz_LCD(2, 0, 16);
				
		LCD_setPos(1,0);
		escreve_LCD("M1:");
	
		LCD_setPos(1,7);
		escreve_LCD("B:");
		CvrtNum2CaracterIndividual(dadosLCD.tensaoBateria	, caracConvertido, 2);
		escreve_LCD(caracConvertido);
		EscreveCaracterLCD('V');
	
		LCD_setPos(1,13);
		EscreveCaracterLCD('[');
		EscreveCaracterLCD(dadosLCD.direcao);
		EscreveCaracterLCD(']');
	
		LCD_setPos(2,0);
		escreve_LCD("M2:");
		
		LCD_setPos(2,7);
		escreve_LCD("P:");
		CvrtNum2CaracterIndividual(dadosLCD.tensaoPainel, caracConvertido, 2);
		escreve_LCD(caracConvertido);
		EscreveCaracterLCD('V');
	
		LCD_setPos(2,13);
		EscreveCaracterLCD('F');
		EscreveCaracterLCD(':');
		EscreveCaracterLCD(dadosLCD.fonteAlimentacao);	
	}
		
	contador++;
}

//----------------------------------------------------------------------------

void Protocolo()
{
	dadosLCD.dutyLadoEsq = CalculaDutyCycleLadoEsq();
	dadosLCD.dutyLadoDir= CalculaDutyCycleLadoDir();
		
	dadosLCD.sentido = CalculaSentido();
	dadosLCD.direcao = DirecaoCarro(dadosLCD.sentido);
		
	TransmitiBuffer();
}	

//----------------------------------------------------------------------------

uint8_t RecebeProtocolo()
{	
	uint16_t valorLidoAD = UDR0;
	if(bufferRecepcao_g.iniciado) {
		bufferRecepcao_g.qntdDadosLido++;
		
		switch(bufferRecepcao_g.qntdDadosLido){
		case 1:
			bufferRecepcao_g.fonteAlimentacao = valorLidoAD;
			dadosLCD.fonteAlimentacao = bufferRecepcao_g.fonteAlimentacao;
			break;
		case 2:
			bufferRecepcao_g.tensaoBateria = valorLidoAD;
			dadosLCD.tensaoBateria = bufferRecepcao_g.tensaoBateria;
			break;
		case 3:
			bufferRecepcao_g.tensaoPainel = valorLidoAD;
			dadosLCD.tensaoPainel = bufferRecepcao_g.tensaoPainel;
			bufferRecepcao_g.iniciado = 0;
			bufferRecepcao_g.completo = 1;
			bufferRecepcao_g.qntdDadosLido = 0;
										
			break;
		}
	}		
	
	/*Inicia transmissão*/
	if(valorLidoAD == 'z') {
		bufferRecepcao_g.iniciado = 1;	
		bufferRecepcao_g.completo = 0;	
	}				
	
	return bufferRecepcao_g.completo;
}	

//----------------------------------------------------------------------------