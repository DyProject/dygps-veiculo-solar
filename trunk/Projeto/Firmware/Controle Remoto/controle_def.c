#include "controle_def.h"

#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;
unsigned char caracterConvertido_g[5];

//----------------------------------------------------------------------------

uint8_t RecebeProtocolo(
	BufferDados* bufferRecepcao
)
{	
	uint8_t dadoRecebido = UDR0;	
	bufferRecepcao->completo = 'n';
	/*Inicia recepção*/
	if(dadoRecebido == 'z') {
		bufferRecepcao->iniciado = 'y';		
		bufferRecepcao->qntdDadosLido++;
	}		
	else if(bufferRecepcao->iniciado == 'y') {
						
		if(bufferRecepcao->qntdDadosLido == 1) {
			bufferRecepcao->fonteAlimentacao = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;
		}
		else if(bufferRecepcao->qntdDadosLido == 2) {		
			bufferRecepcao->tensaoBateria = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;
		}			
		else if(bufferRecepcao->qntdDadosLido == 3) {
			bufferRecepcao->tensaoPainel = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;			
			bufferRecepcao->iniciado = 'n';
			bufferRecepcao->completo = 'y';
			bufferRecepcao->qntdDadosLido = 0;
		}
	}		
			
	return bufferRecepcao->completo;
}	

//----------------------------------------------------------------------------

void TransmitiBuffer(
	BufferDados* bufferDados
)
{
	volatile uint8_t indicaInicioTransmissao = 'S';
	volatile uint8_t direcao;
	volatile uint8_t dutyLadoEsq;
	volatile uint8_t dutyLadoDir;
	volatile uint8_t sentido;
	volatile uint8_t fonte;
	
	sentido = CalculaSentido();
	bufferDados->sentido = sentido;
	
	direcao = DirecaoCarro(sentido);
	bufferDados->direcao = direcao;
	dutyLadoEsq = CalculaDutyCycleLadoEsq();
	bufferDados->dutyLadoEsq = dutyLadoEsq;
	dutyLadoDir = CalculaDutyCycleLadoDir();
	bufferDados->dutyLadoDir = dutyLadoDir;
	
	/*Indica o inicio do protocolo*/
	Usart_Transmit(indicaInicioTransmissao);
	/*Indica a direcao do carro*/
	Usart_Transmit(direcao);
	/*Indica o duty cicle motor 1*/
	Usart_Transmit(dutyLadoEsq);
	/*Indica o duty cicle motor 1*/
	Usart_Transmit(dutyLadoDir);
	/*Indica se deve alterar a fonte de alimentacao*/
	if(bufferDados->botaoSelFontePress == 'y')
		fonte = '1';//deve inverter a fonte de alimentação
	else fonte = '0';
	bufferDados->botaoSelFontePress = 'n';	
	Usart_Transmit(fonte);//falta ajustar para enviar 1 quando o botão for pressionado
}

//----------------------------------------------------------------------------

void MostraDadosLCD(
	BufferDados* bufferDados
)
{
	/*Imprimi tudo na primeira vez*/
	static uint8_t contador = 20;
	unsigned char caracConvertido[3];
			
	LCD_setPos(1,3);
	CvrtNum2CaracterIndividual(bufferDados->dutyLadoEsq	, caracConvertido, 3);
	escreve_LCD(caracConvertido);
	
	LCD_setPos(2,3);
	CvrtNum2CaracterIndividual(bufferDados->dutyLadoDir,  caracConvertido, 3);
	escreve_LCD(caracConvertido);
	
	/*Para o display não ficar piscando*/
	if(contador > 16){				
		LCD_setPos(1,0);
		escreve_LCD("M1:");
	
		LCD_setPos(1,7);
		escreve_LCD("B:");
		CvrtNum2CaracterIndividual(bufferDados->tensaoBateria, caracConvertido, 2);
		escreve_LCD(caracConvertido);
		EscreveCaracterLCD('V');
	
		LCD_setPos(1,13);
		EscreveCaracterLCD('[');
		EscreveCaracterLCD(bufferDados->direcao);
		EscreveCaracterLCD(']');
	
		LCD_setPos(2,0);
		escreve_LCD("M2:");
		
		LCD_setPos(2,7);
		escreve_LCD("P:");
		CvrtNum2CaracterIndividual(bufferDados->tensaoPainel, caracConvertido, 2);
		escreve_LCD(caracConvertido);
		EscreveCaracterLCD('V');
	
		LCD_setPos(2,13);
		EscreveCaracterLCD('F');
		EscreveCaracterLCD(':');
		EscreveCaracterLCD(bufferDados->fonteAlimentacao);
	}
		
	contador++;
}
//----------------------------------------------------------------------------