#include "direcao_def.h"

#include "adc_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------

volatile TEstadoCarro estadoCarro_g;

//----------------------------------------------------------------------------
	
/*Irá receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM1(
	uint16_t porCentagem
)
{
	uint16_t valor = 0;
	if(porCentagem == 100)
		valor = 35000;
	else if (porCentagem == 75)
		valor = 26250;
	else if (porCentagem == 50)
		valor = 17500;
	else if (porCentagem == 25)
		valor = 8750;
		
	if(estadoCarro_g == ANDANDO_TRAS)
		valor = 35000 - valor;
		
	return valor;
}

//----------------------------------------------------------------------------

/*Irá receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM2(
	uint16_t porCentagem
)
{
	uint16_t valor = 0;
	if (porCentagem == 100)
		valor = 35000;
	else if (porCentagem == 75)
		valor = 26250;
	else if (porCentagem == 50)
		valor = 17500;
	else if (porCentagem == 25)
		valor = 8750;
		
	if(estadoCarro_g == ANDANDO_TRAS)
		valor = 35000 - valor;
			
	return valor;
}

//----------------------------------------------------------------------------

void ConfiguracoesDirecaoInit(
	BufferRecep* bufferRecepcao
)
{	
	/*pinos OC1B e OC1A como saída*/
	DDRB |= (1 << IN2_IN4) | (1 << ENA_ENB) | (1 << IN1) | (1 << IN3);	
	PORTB |= (1 << IN2_IN4) | (1 << ENA_ENB);		
	PORTB &= (~(1 << IN1)) & (~(1 << IN3)); 
	
	/*Pino relé como saída*/	
	RELE_DDR |= (1 << RELE_CHAVE_PAINEL_BATERIA) | (1 << RELE_TENSAO_PAINEL);
	
	/*Seleciona a fonte de alimentação da bateria*/
	DESATIVA_RELE(RELE_CHAVE_PAINEL_BATERIA);
	
	/*Desliga o painel*/
	DESATIVA_RELE(RELE_TENSAO_PAINEL);
		
	TCCR1A = 0b10100010;		//PWM não invertido nos pinos OC1A e OC1B
	TCCR1B = 0b00011001;		//liga TC1, prescaler = 1
	ICR1 = 35000;				//valor máximo para contagem
	OCR1A = 0;				//controle do ciclo ativo do PWM 0C1A
	OCR1B = 0;
	
	/*Configurações de inicialização d do buffer*/
	bufferRecepcao->dutyCicleM1 = 0;
	bufferRecepcao->dutyCicleM2 = 0;
	bufferRecepcao->qntdDadosLido = 0;
	bufferRecepcao->iniciado = 'n';
	bufferRecepcao->completo = 'n';
	bufferRecepcao->fonteAlimentacao = 'B';
	bufferRecepcao->direcao = 'P';
		
	//Prescaler do Timer0, usado para fazer uma leitura do ADC.
	TCCR0B = (1<<CS02) | (1<<CS00);
					
	CarroParado();
};

//----------------------------------------------------------------------------

void SetaFonteAlimentacao(
	BufferRecep* bufferRecepcao
)
{	
	if(bufferRecepcao->fonteAlimentacao == 'P') {
		DESATIVA_RELE(RELE_CHAVE_PAINEL_BATERIA);
		bufferRecepcao->fonteAlimentacao = 'B';
	}			
	else  if(bufferRecepcao->fonteAlimentacao == 'B'){
		ATIVA_RELE(RELE_CHAVE_PAINEL_BATERIA);
		bufferRecepcao->fonteAlimentacao = 'P';
	}				
}	
	
//----------------------------------------------------------------------------

void DirecaoCarro(
	BufferRecep* bufferRecepcao
)
{		
	switch (estadoCarro_g) {
		case PARADO:
			if ( bufferRecepcao->direcao == 'F') {
				estadoCarro_g = ANDANDO_FRENTE;
				AndandoFrente(bufferRecepcao);
			}				
			else if ( bufferRecepcao->direcao == 'T') {
				estadoCarro_g = ANDANDO_TRAS;
				AndandoTras(bufferRecepcao);		
			}			
			else bufferRecepcao->direcao = 'P';
			break;
		
		case ANDANDO_FRENTE:	
			if(bufferRecepcao->direcao == 'P') {
				estadoCarro_g = PARADO;
				CarroParado();
			}				
			else if(bufferRecepcao->direcao == 'F'){
				OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2);
			}			
			break;
			
		case ANDANDO_TRAS:
			if(bufferRecepcao->direcao == 'P') {
				estadoCarro_g = PARADO;
				CarroParado();
			}				
			else if(bufferRecepcao->direcao == 'T'){
				OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2);
			}			
			break;			
	}		
}		

//----------------------------------------------------------------------------

void AndandoFrente(
	BufferRecep* bufferRecepcao
)
{
	_delay_us(10);
	set_bit(PORTB,ENA_ENB);	
	clr_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1);
	OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2);
	estadoCarro_g = ANDANDO_FRENTE;
}

//----------------------------------------------------------------------------

void AndandoTras(
	BufferRecep* bufferRecepcao
)
{
	_delay_us(10);
	set_bit(PORTB,ENA_ENB);	
	set_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1);
	OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2);
	estadoCarro_g = ANDANDO_TRAS;
}

//----------------------------------------------------------------------------

void CarroParado()
{
	_delay_us(10);
	clr_bit(PORTB,ENA_ENB);	
	set_bit(PORTB,IN1);	
	set_bit(PORTB,IN3);	
	set_bit(PORTB,IN2_IN4);	
	estadoCarro_g = PARADO;
}

//----------------------------------------------------------------------------

uint8_t RecebeProtocolo(
	BufferRecep* bufferRecepcao
)
{	
	uint8_t dadoRecebido = UDR0;
		
	bufferRecepcao->completo = 0;
	/*Inicia transmissão*/
	if(dadoRecebido == 'S') 
		bufferRecepcao->iniciado = 'y';		
	else if(bufferRecepcao->iniciado == 'y') {
		bufferRecepcao->qntdDadosLido++;				
		switch(bufferRecepcao->qntdDadosLido){
		case 1:
			bufferRecepcao->direcao = dadoRecebido;
			break;
		case 2:
			bufferRecepcao->dutyCicleM1 = dadoRecebido;
			break;
		case 3:		
			bufferRecepcao->dutyCicleM2 = dadoRecebido;
			break;
		case 4:
			if(dadoRecebido == '1') {
				//SetaFonteAlimentacao(bufferRecepcao);	
				bufferRecepcao->fonteAlimentacao = 'P';
				Usart_Transmit('[');
				Usart_Transmit('w');
				Usart_Transmit(']');	
			}				
			
			bufferRecepcao->iniciado = 'n';
			bufferRecepcao->completo = 'y';
			
			DirecaoCarro(bufferRecepcao);
				
			break;
		}
	}		
			
	return bufferRecepcao->completo;
}	

//----------------------------------------------------------------------------

void TrasmitiBuffer(
	BufferRecep* bufferRecepcao
)
{	
	uint8_t indicaInicioTransmissao = 'z';
	volatile uint8_t fonte = bufferRecepcao->fonteAlimentacao;
	volatile uint8_t tensaoBat = TensaoBateria();
	volatile uint8_t tensaoPainel = TensaoPainel();
					
	/*Indica o recebimento do protocolo e o inicio do envio do novo protocolo*/			
	Usart_Transmit(indicaInicioTransmissao);
	
	/*Indica qual fonte está selecionada 'B' bateria ou 'P' painel*/
	Usart_Transmit(fonte);
	
	/*Tensão na bateria*/	
	Usart_Transmit(tensaoBat);
	
	/*Tensão no Painel*/
	Usart_Transmit(tensaoPainel);
}

//----------------------------------------------------------------------------

uint8_t TensaoBateria()
{
	uint16_t lido16 = ADC_Read(AD_BATERIA);
	uint8_t tensao = (uint8_t)(lido16 * AJUSTE_AD);
		
	return tensao;
}

//----------------------------------------------------------------------------

uint8_t TensaoPainel()
{		
	uint16_t lido16 = ADC_Read(AD_PAINEL);
	uint8_t tensao = (uint8_t)(lido16 * AJUSTE_AD);
	
	return tensao;
	//return lidoAd;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

