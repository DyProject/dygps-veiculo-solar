#include "direcao_def.h"

#include "adc_def.h"
#include "usart_def.h"

//----------------------------------------------------------------------------
	
/*Ir� receber sempre os duty cicle definidos 25, 50, 75 e 100*/
uint16_t CalculaDutyCicleM1(
	uint16_t porCentagem,
	TEstadoCarro estadoCarro
)
{
	if(porCentagem > 100)
		porCentagem = 100;
	
	/*Valor � igual ao (35000 * 0,25) ou (350*porcentagem)*/
	uint16_t valor = porCentagem * 350;
				
	if(estadoCarro == ANDANDO_TRAS)
		valor = 35000 - valor;
		
	return valor;
}

//----------------------------------------------------------------------------

/*Ir� receber sempre os duty cicle definidos 25, 50, 75 e 100*/
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
	Buffer* bufferRecepcao
)
{	
	/*pinos OC1B e OC1A como sa�da*/
	DDRB |= (1 << IN2_IN4) | (1 << ENA_ENB) | (1 << IN1) | (1 << IN3);	
	PORTB |= (1 << IN2_IN4) | (1 << ENA_ENB);		
	PORTB &= (~(1 << IN1)) & (~(1 << IN3)); 
	
	/*Pino rel� como sa�da*/	
	RELE_DDR |= (1 << RELE_CHAVE_PAINEL_BATERIA) | (1 << RELE_TENSAO_PAINEL);
	
	/*Seleciona a fonte de alimenta��o da bateria*/
	DESATIVA_RELE(RELE_CHAVE_PAINEL_BATERIA);
	
	/*Desliga o painel*/
	DESATIVA_RELE(RELE_TENSAO_PAINEL);
		
	TCCR1A = 0b10100010;		//PWM n�o invertido nos pinos OC1A e OC1B
	TCCR1B = 0b00011001;		//liga TC1, prescaler = 1
	ICR1 = 35000;				//valor m�ximo para contagem
	OCR1A = 0;				//controle do ciclo ativo do PWM 0C1A
	OCR1B = 0;
			
	ConfigInitServo();
					
	CarroParado();
};

//----------------------------------------------------------------------------

void ConfigInitServo() {
	/*pinos OC0B e OC0A como sa�da*/
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
	Buffer* bufferRecepcao
) 
{
	uint8_t incremento = 35;
	uint8_t incrementoInicial = 15;
	
	switch (bufferRecepcao->estadoCarro) {			
		case ANDANDO_FRENTE:
		case ANDANDO_TRAS:			
			if(bufferRecepcao->dutyCicleM1 > incrementoInicial)//Abaixo desse valor o carrinho n�o anda
				bufferRecepcao->dutyCicleM1 -= incremento;
		
			if(bufferRecepcao->dutyCicleM2 > incrementoInicial)//Abaixo desse valor o carrinho n�o anda
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
	Buffer* bufferRecepcao
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

//Recebe um valor entre 0-100%. 100% equivale a 180�
/*O frequencia do servo � de 50Hz per�odo de 20ms com valor minimo de 1ms(5%) e m�ximo de 2ms(10%). O registrador OCR0x � de 8bits (0-255) o valor m�ximo para*/
void AnguloServo(
	Buffer* bufferRecepcao
){			
	/*O per�odo do duty do servo � de no m�nimo 1ms e no m�ximo 2ms. A Fpwm = fclk_io / (prescaler*256) para o prescaler do timer de 1024 o per�odo � de 16.384ms. 
	O contador do timer0 deve contar de 15,56 (1ms) at� 31,12 (2ms)*/
	uint16_t anguloServoLeft = (uint16_t)(((bufferRecepcao->anguloServoLeft) * 0.15) + 15); //(Valor = (dutyCicle * 15) / 100) + 15
	uint16_t anguloServoRight = (uint16_t)(((bufferRecepcao->anguloServoRight) * 0.15) + 15); 
	
	unsigned char left = (unsigned char) anguloServoLeft; 
	unsigned char right = (unsigned char) anguloServoRight;
	
	OCR0A = left;
	OCR0B = right;
}


//----------------------------------------------------------------------------

void AndandoFrente(
	Buffer* bufferRecepcao
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
	Buffer* bufferRecepcao
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

void TransmitiBuffer(
	Buffer* buffer
)
{	
	fflush(stdout);
	stdout = &stdoutUART;
		
	printf("z");
	//switch(buffer->estadoBufferTrans) {		
		//case IDLE:
			//buffer->estadoBufferTrans = DADO_1;
			//printf("%c", INICIO_PACOTE_TRANS);
		//break;
		//
		//case INICIADO:				
		//case DADO_1:
			//buffer->estadoBufferTrans = DADO_2;
			////printf("%c", buffer->fonteAlimentacao);
			//printf("%c", 'a');
		////break;
				//
		//case DADO_2:
			//buffer->estadoBufferTrans = DADO_3;
			////printf("%c", TensaoBateria());
			//printf("%c", 'b');
		////break;
			//
		//case DADO_3:
			//buffer->estadoBufferTrans = CONCLUIDO;
			////printf("%c", TensaoPainel());
			//printf("%c", 'c');
		//break;
			//
		//case ERRO_COMUNICACAO:
			//printf("%c", REPORTAR_ERRO);
			//printf("%c", buffer->erroCodeRecep);
		//break;
	//
		//case CONCLUIDO:
		//default:
			//buffer->estadoBufferTrans = IDLE;		
			//
	//}
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
	
	/*Se a tens�o do painel for menor que 13 volts o painel n�o conseguir� 
	fornecer a corrente necess�ria para os motores. Ent�o foi colocado um
	rel� para selecionar entre a tens�o da bateria ou a tens�o do painel*/
	if(tensao >= 13) 
		ATIVA_RELE(RELE_TENSAO_PAINEL);
	else 
		DESATIVA_RELE(RELE_TENSAO_PAINEL);
		
	
	return tensao;
	//return lidoAd;
}

//--------------------------------------------------------------------------------------------------

