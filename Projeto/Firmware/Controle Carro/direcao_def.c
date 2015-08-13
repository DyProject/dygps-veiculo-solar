#include "direcao_def.h"

#include "adc_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------
	
/*Irá receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM1(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
)
{
	if(porCentagem > 100)
		porCentagem = 100;
	
	/*Valor é igual ao (35000 * 0,25) ou (350*porcentagem)*/
	uint16_t valor = porCentagem * 350;
				
	if(estadoCarro == ANDANDO_TRAS)
		valor = 35000 - valor;
		
	return valor;
}

//----------------------------------------------------------------------------

/*Irá receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM2(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
)
{
	if(porCentagem > 100)
		porCentagem = 100;
	
	uint16_t valor = porCentagem * 350;
		
	if(estadoCarro == ANDANDO_TRAS)
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
			
	ConfigInitServo();
					
	CarroParado();
};

//----------------------------------------------------------------------------

void ConfigInitServo() {
	/*pinos OC0B e OC0A como saída*/
	DDRD |= (1 << DDD6) | (1 << DDD5);
	
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // set none-inverting mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00);// set fast PWM Mode
	
	TCCR0B |= (1 << CS02) | (1 << CS00); // set prescaler to 1024 (Freq Servo Motor 50Hz)
	
	OCR0A = 23; // equivale a 1.5 ms do periodo do servo
	OCR0B = 23;
}

//----------------------------------------------------------------------------

void SetaFonteAlimentacao(
	volatile uint8_t* fonteAlimentacao
)
{	
	if(*fonteAlimentacao == 'P') {
		DESATIVA_RELE(RELE_CHAVE_PAINEL_BATERIA);
		*fonteAlimentacao = 'B';
	}			
	else  if(*fonteAlimentacao == 'B'){
		ATIVA_RELE(RELE_CHAVE_PAINEL_BATERIA);
		*fonteAlimentacao = 'P';
	}				
}	
	
//----------------------------------------------------------------------------

void ParadaLenta(
	BufferRecep* bufferRecepcao
) 
{
	uint8_t incremento = 35;
	uint8_t incrementoInicial = 15;
	
	switch (bufferRecepcao->estadoCarro) {			
		case ANDANDO_FRENTE:
		case ANDANDO_TRAS:			
			if(bufferRecepcao->dutyCicleM1 > incrementoInicial)//Abaixo desse valor o carrinho não anda
				bufferRecepcao->dutyCicleM1 -= incremento;
		
			if(bufferRecepcao->dutyCicleM2 > incrementoInicial)//Abaixo desse valor o carrinho não anda
				bufferRecepcao->dutyCicleM2 -= incremento;
		
			if(bufferRecepcao->dutyCicleM1 < incrementoInicial && bufferRecepcao->dutyCicleM2 < incrementoInicial) {
					bufferRecepcao->estadoCarro = PARADO;
					bufferRecepcao->iniciado = 'n';
					bufferRecepcao->completo = 'n';
					bufferRecepcao->qntdDadosLido = 0;
					CarroParado();
				}
				
			else {
				OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1, bufferRecepcao->estadoCarro);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2, bufferRecepcao->estadoCarro);
			}
	
		break;
		case PARADO:
		break;
	}
}

//----------------------------------------------------------------------------

void DirecaoCarro(
	BufferRecep* bufferRecepcao
)
{		
	switch (bufferRecepcao->estadoCarro) {
		case PARADO:
			if ( bufferRecepcao->direcao == 'F') {
				bufferRecepcao->estadoCarro = ANDANDO_FRENTE;
				AndandoFrente(bufferRecepcao);
			}				
			else if ( bufferRecepcao->direcao == 'T') {
				bufferRecepcao->estadoCarro = ANDANDO_TRAS;
				AndandoTras(bufferRecepcao);		
			}			
			else bufferRecepcao->direcao = 'P';
			break;
		
		case ANDANDO_FRENTE:	
			if(bufferRecepcao->direcao == 'P') {
				bufferRecepcao->estadoCarro = PARADO;
				CarroParado();
			}				
			else if(bufferRecepcao->direcao == 'F'){
				OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1, bufferRecepcao->estadoCarro);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2, bufferRecepcao->estadoCarro);
			}			
			break;
			
		case ANDANDO_TRAS:
			if(bufferRecepcao->direcao == 'P') {
				bufferRecepcao->estadoCarro = PARADO;
				CarroParado();
			}				
			else if(bufferRecepcao->direcao == 'T'){
				OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1, bufferRecepcao->estadoCarro);
				OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2, bufferRecepcao->estadoCarro);
			}			
			break;			
	}		
}		

//----------------------------------------------------------------------------

void AnguloServo(
	BufferRecep* bufferRecepcao
){	
	
	/*O frequencia do servo é de 50Hz de 1ms a 2ms. a frequencia minima alcançada foi de 60Hz (16.66s). para 1ms é 6% de 16.66s e 2ms é igual a 12%. Desta forma, o timer zero com valor máximo de 255 deve
	gerar um valor aproximado de 15 a 30 (6% e 12% de 255) */
	uint16_t anguloServoLeft = (uint16_t)(((bufferRecepcao->anguloServoLeft) * 0.125) + 15); //(Valor = (dutyCicle * 15) / 120) + 15
	uint16_t anguloServoRight = (uint16_t)(((bufferRecepcao->anguloServoRight) * 0.125) + 15); 
	
	unsigned char left = (unsigned char) anguloServoLeft; 
	unsigned char right = (unsigned char) anguloServoRight;
	
	OCR0A = left;
	OCR0B = right;
}


//----------------------------------------------------------------------------

void AndandoFrente(
	BufferRecep* bufferRecepcao
)
{
	_delay_us(10);
	set_bit(PORTB,ENA_ENB);	
	clr_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1, bufferRecepcao->estadoCarro);
	OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2, bufferRecepcao->estadoCarro);
}

//----------------------------------------------------------------------------

void AndandoTras(
	BufferRecep* bufferRecepcao
)
{
	_delay_us(10);
	set_bit(PORTB,ENA_ENB);	
	set_bit(PORTB,IN2_IN4);
	OCR1A = CalculaDutyCicleM1(bufferRecepcao->dutyCicleM1, bufferRecepcao->estadoCarro);
	OCR1B = CalculaDutyCicleM2(bufferRecepcao->dutyCicleM2, bufferRecepcao->estadoCarro);
}

//----------------------------------------------------------------------------

void CarroParado()
{
	_delay_us(10);
	clr_bit(PORTB,ENA_ENB);	
	set_bit(PORTB,IN1);	
	set_bit(PORTB,IN3);	
	set_bit(PORTB,IN2_IN4);	
}

//----------------------------------------------------------------------------

uint8_t RecebeProtocolo(
	BufferRecep* bufferRecepcao
)
{	
	uint8_t dadoRecebido = UDR0;	
	bufferRecepcao->completo = 'n';
	/*Inicia transmissão*/
	if(dadoRecebido == 'S') {
		bufferRecepcao->iniciado = 'y';		
		bufferRecepcao->qntdDadosLido++;
	}		
	else if(bufferRecepcao->iniciado == 'y') {
						
		if(bufferRecepcao->qntdDadosLido == 1) {
			bufferRecepcao->direcao = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;
		}
		else if(bufferRecepcao->qntdDadosLido == 2) {		
			bufferRecepcao->dutyCicleM1 = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;
		}			
		else if(bufferRecepcao->qntdDadosLido == 3) {
			bufferRecepcao->dutyCicleM2 = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;	
		}	
		else if(bufferRecepcao->qntdDadosLido == 4) {
			bufferRecepcao->anguloServoLeft = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;
		}		
		else if(bufferRecepcao->qntdDadosLido == 5) {
			bufferRecepcao->anguloServoRight = dadoRecebido;
			bufferRecepcao->qntdDadosLido++;
		}
		else {
			if(dadoRecebido == '1')
				SetaFonteAlimentacao(&bufferRecepcao->fonteAlimentacao);		
			
			bufferRecepcao->iniciado = 'n';
			bufferRecepcao->completo = 'y';
			bufferRecepcao->qntdDadosLido = 0;
		}
	}		
			
	return bufferRecepcao->completo;
}	

//----------------------------------------------------------------------------

void TransmitiBuffer(
	volatile uint8_t* fonteAlimentacao
)
{	
	volatile uint8_t indicaInicioTransmissao = 'z';
	volatile uint8_t tensaoBat ;
	volatile uint8_t tensaoPainel;
	//volatile uint16_t lidoADBat;
	//volatile uint16_t lidoADPain;	
	
	//lidoADBat = ADC_Read(AD_BATERIA);
	//lidoADPain = ADC_Read(AD_PAINEL);
	//tensaoBat = (uint8_t)((lidoADBat * 30)/640);
	//tensaoPainel = (uint8_t)((lidoADPain * 30)/640);
						
	tensaoBat = TensaoBateria();
	tensaoPainel = TensaoPainel();
						
	/*Indica o recebimento do protocolo e o inicio do envio do novo protocolo*/			
	Usart_Transmit(indicaInicioTransmissao);
	
	/*Indica qual fonte está selecionada 'B' bateria ou 'P' painel*/
	Usart_Transmit(*fonteAlimentacao);
	
	/*Tensão na bateria*/	
	Usart_Transmit('2');//trace
	//Usart_Transmit(tensaoBat);
	
	/*Tensão no Painel*/
	Usart_Transmit('!');//trace
	//Usart_Transmit(tensaoPainel);
}

//----------------------------------------------------------------------------

uint8_t TensaoBateria()
{
	volatile uint16_t lido16;
	volatile uint8_t tensao;
	
	lido16 = ADC_Read(AD_BATERIA);
	tensao = (uint8_t)((lido16 * 30)/640);
		
	return tensao;
}

//----------------------------------------------------------------------------

uint8_t TensaoPainel()
{		
	volatile uint16_t lido16;
	volatile uint8_t tensao;
	
	lido16 = ADC_Read(AD_PAINEL);
	tensao = (uint8_t)((lido16 * 30)/640);
	
	/*Se a tensão do painel for menor que 13 volts o painel não conseguirá 
	fornecer a corrente necessária para os motores. Então foi colocado um
	relé para selecionar entre a tensão da bateria ou a tensão do painel*/
	if(tensao >= 13) 
		ATIVA_RELE(RELE_TENSAO_PAINEL);
	else 
		DESATIVA_RELE(RELE_TENSAO_PAINEL);
		
	
	return tensao;
	//return lidoAd;
}

//--------------------------------------------------------------------------------------------------

