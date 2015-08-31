#include "controle_def.h"

#include "globals_def.h"
#include "LCD4b.h"
#include "usart_def.h"
#include "joystick.h"

//----------------------------------------------------------------------------

 //Estado do carro. Inicialmente parado 
volatile TEstadoCarro estadoCarro = PARADO;
unsigned char caracterConvertido_g[5];

//----------------------------------------------------------------------------

/*Recebe os dados em 3ms*/
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
	volatile uint16_t anguloServoLeft;
	volatile uint16_t anguloServoRigth;
	

	sentido = CalculaSentido();
	bufferDados->sentido = sentido;
	
	direcao = DirecaoCarro(sentido);
	bufferDados->direcao = direcao;
	
	dutyLadoEsq = CalculaDutyCycleLadoEsq();
	bufferDados->dutyLadoEsq = dutyLadoEsq;
	dutyLadoDir = CalculaDutyCycleLadoDir();
	bufferDados->dutyLadoDir = dutyLadoDir;
	
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
	
	CalculaAngulosServo(&joystick, bufferDados);
	anguloServoLeft = bufferDados->anguloServoLeft;
	anguloServoRigth = bufferDados->anguloServoRight;
		//
	/*Indica o inicio do protocolo*/
	Usart_Transmit(indicaInicioTransmissao);
	/*Indica a direcao do carro*/
	Usart_Transmit(direcao);
	/*Indica o duty cicle motor 1*/
	Usart_Transmit(dutyLadoEsq);
	/*Indica o duty cicle motor 1*/
	Usart_Transmit(dutyLadoDir);
	
	/*Informações Servo*/
	Usart_Transmit(anguloServoLeft);
	Usart_Transmit(anguloServoRigth);
	
	/*Indica se deve alterar a fonte de alimentacao*/
	if(bufferDados->botaoSelFontePress == 'y')
		fonte = '1';//deve inverter a fonte de alimentação
	else fonte = '0';
	bufferDados->botaoSelFontePress = 'n';	
	Usart_Transmit(fonte);
}

//----------------------------------------------------------------------------

void MostraDadosLCD(
	BufferDados* bufferDados
)
{
	FILE mystdout = FDEV_SETUP_STREAM(cmd_LCD_Printf, NULL, _FDEV_SETUP_WRITE);
	
	stdout = &mystdout;
			
	LCD_setPos(1,0);
	printf("MD:%d ", bufferDados->dutyLadoDir);
	LCD_setPos(2,0);
	printf("ME:%d ", bufferDados->dutyLadoEsq);
	
	LCD_setPos(1,7);
	printf("SD:%d ", bufferDados->anguloServoRight);
	LCD_setPos(2,7);
	printf("SE:%d ", bufferDados->anguloServoLeft);
	
	LCD_setPos(1,14);
	if(bufferDados->estacomunicando == 'n') 
		printf("NC");	
	else 
		printf("OK");
		
	LCD_setPos(2,14);	
	printf("%c", bufferDados->direcao);
	
}
//----------------------------------------------------------------------------