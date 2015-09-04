#include "controle_def.h"

#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"
#include "joystick.h"

extern FILE stdoutUART;
extern FILE stdoutLCD4;

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;

//----------------------------------------------------------------------------

void CarregaBufferTransmissao(
	BufferDados* bufferDados,
	unsigned char* buff
)
{
	//Valores do angulo do servo
	JoyStick joystick;
	
	if(!PontoXNaPosInic(AD_EIXO_X_SERVO))
		joystick.valorEixoX = (((int32_t)ValorLidoADEixoX(AD_EIXO_X_SERVO))- 510);
	else
		joystick.valorEixoX = 0;
	
	if(!PontoYNaPosInic(AD_EIXO_Y_SERVO))
		joystick.valorEixoY = (((int32_t)(ValorLidoADEixoY(AD_EIXO_Y_SERVO))) - 506);
	else
		joystick.valorEixoY = 0;

	TankDrive(&joystick);
		
	*(buff + DIRECAO) = DirecaoCarro(CalculaSentido());
	*(buff + DUTY_MOTOR_E)= CalculaDutyCycleLadoEsq();
	*(buff + DUTY_MOTOR_D) = CalculaDutyCycleLadoDir();
	*(buff + ANGULO_SERVO_E) = CalculaAnguloServoLeft(joystick.dutyLE);
	*(buff + ANGULO_SERVO_D) = CalculaAnguloServoRight(joystick.dutyLD);;
	*(buff + FONTE_ALIMENTACAO) = bufferDados->fonteAlimentacao; 
}

//----------------------------------------------------------------------------

void TransmitiBuffer(
	unsigned char* buff
)
{
	fflush(stdout);
	stdout = &stdoutUART;
		
	//Indica inicio da transmissao
	printf("%c", START_TRANSMISSION);
	_delay_us(30);
	printf("%c", QUANT_DADOS_PACOTE_TRANS);
	_delay_us(30);	
	
	//Envia dados
	uint8_t cont;
	for (cont = 0; cont < QUANT_DADOS_PACOTE_TRANS; cont++) {
		printf("%c", *(buff + cont));
		_delay_us(30);
	}
	
		
	//Envia checksum
	//trace	
}

//----------------------------------------------------------------------------

void MostraDadosLCD(
	BufferDados* bufferDados,
	unsigned char* bufferDadosTrans
)
{
	fflush(stdout);
	stdout = &stdoutLCD4;
	Limpa_LCD_RetornaCarro();

	switch(bufferDados->msgLCD4){
		case MSG_INICIAL:
			LCD4_MSG_INICIAL();
		break;
		
		case MSG_1:
			LCD4_MSG1(bufferDados, bufferDadosTrans);
		break;
	}
	
}
//----------------------------------------------------------------------------

void LCD4_MSG_INICIAL()
{
	LCD_setPos(1,0);
	printf("Estabelecendo");
	LCD_setPos(2,0);
	printf("Comunicacao...");
}

//----------------------------------------------------------------------------

void LCD4_MSG1(
	BufferDados* bufferDadosRecep,
	unsigned char* bufferTransmissao
)
{
	LCD_setPos(1,0);
	printf("MD:%d ", bufferTransmissao[DUTY_MOTOR_D]);
	LCD_setPos(2,0);
	printf("ME:%d ", bufferTransmissao[DUTY_MOTOR_E]);
	
	LCD_setPos(1,7);
	printf("SD:%d ", bufferTransmissao[ANGULO_SERVO_D]);
	LCD_setPos(2,7);
	printf("SE:%d ", bufferTransmissao[ANGULO_SERVO_E]);
	
	LCD_setPos(1,14);
	if(bufferDadosRecep->estacomunicando == 'n')
		printf("NC");
	else
		printf("OK");
	
	LCD_setPos(2,14);
	printf("%c", bufferTransmissao[FONTE_ALIMENTACAO]);
	
}

//----------------------------------------------------------------------------