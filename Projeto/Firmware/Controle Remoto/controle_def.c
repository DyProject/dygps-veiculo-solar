#include "controle_def.h"

#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"

extern FILE stdoutUART;
extern FILE stdoutLCD4;

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;

//----------------------------------------------------------------------------
/*A atual configuração do carro não permite acionar um motor para tras e outro para frente. Necessário ajuste de hardware*/
uint8_t AjustaValoresDutyParaConfigCarroESQ(
	JoyStick joy,
	unsigned char direcao
)
{
	volatile int16_t dutyLE, dutyProv;

	if(direcao == 'T')
		dutyLE = fabs(joy.dutyLD);

	else if (direcao == 'F') {
		if(joy.dutyLE < 0)
			dutyLE = 0;
		else 
			dutyLE = joy.dutyLE;
	}

	else
		dutyLE = 0;

	dutyProv = SoftStarterLadoEsq(dutyLE);

	return dutyProv;	
}

//----------------------------------------------------------------------------

uint8_t AjustaValoresDutyParaConfigCarroDIR(
	JoyStick joy,
	unsigned char direcao
)
{
	volatile int8_t dutyLD, dutyProv;

	if(direcao == 'T')
		dutyLD = (int8_t)fabs(joy.dutyLE);

	else if (direcao == 'F'){
		if(joy.dutyLD < 0)
			dutyLD = 0;
		else
			dutyLD = joy.dutyLD;
	}

	else
		dutyLD = 0;

	dutyProv = SoftStarterLadoDir(dutyLD);

	return dutyProv;
}

//----------------------------------------------------------------------------

void CarregaBufferTransmissao(
	BufferDados* bufferDados,
	unsigned char* buff
)
{
	//Valores do angulo do servo
	JoyStick joystickServo;
	joystickServo.valorEixoX = (int32_t)ValorEixoX(AD_EIXO_X_SERVO);
	joystickServo.valorEixoY = (int32_t)ValorEixoY(AD_EIXO_Y_SERVO);
	TankDrive(&joystickServo);

	//Valores do angulo do servo
	JoyStick joystickDIR;
	joystickDIR.valorEixoX = (int32_t)ValorEixoX(AD_EIXO_X_DIR);
	joystickDIR.valorEixoY = (int32_t)ValorEixoY(AD_EIXO_Y_DIR);
	TankDrive(&joystickDIR);
	
	volatile unsigned char direcao = DirecaoCarro(joystickDIR);
	*(buff + DIRECAO) = direcao;
	*(buff + DUTY_MOTOR_E) = AjustaValoresDutyParaConfigCarroESQ(joystickDIR, direcao);
	*(buff + DUTY_MOTOR_D) = AjustaValoresDutyParaConfigCarroDIR(joystickDIR, direcao);
	*(buff + ANGULO_SERVO_E) = CalculaAnguloServoLeft(joystickServo.dutyLE);
	*(buff + ANGULO_SERVO_D) = CalculaAnguloServoRight(joystickServo.dutyLD);;
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
	printf("%c", QUANT_DADOS_PACOTE_TRANS);
	
	//Envia dados
	uint8_t cont;
	for (cont = 0; cont < QUANT_DADOS_PACOTE_TRANS; cont++) {
		printf("%c", *(buff + cont));
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
	printf("%c", bufferTransmissao[DIRECAO]);
	
}

//----------------------------------------------------------------------------
