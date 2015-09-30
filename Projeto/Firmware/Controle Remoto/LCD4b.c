#include "LCD4b.h"
#include "globals_def.h"

FILE stdoutLCD4 = FDEV_SETUP_STREAM(cmd_LCD_Printf, NULL, _FDEV_SETUP_WRITE);

//----------------------------------------------------------------------------------------------------------
// Sub-rotina para enviar caracteres e comandos ao LCD com via de dados de 4 bits
//----------------------------------------------------------------------------------------------------------
void cmd_LCD(unsigned char c, char cd)		//c é o dado  e cd indica se é instrução ou caractere
{
	unsigned char i=2;

	DADOS_LCD = (DADOS_LCD & 0xF0)|(c>>4);	//primeiro os 4 MSB. (PA0 - PA1 - PA2 - PA3) -> (D4 - D5 - D6 - D7 LCD)
	//como a outra metade da porta pode ser utilizada para outros fins, não
	do										//se pode alterar o seu conteúdo
	{	if(cd==0)
		clr_bit(CONTR_LCD,RS);
		else
		set_bit(CONTR_LCD,RS);

		pulso_enable;

		if((cd==0) && (c<4))				//se for instrução de retorno ou limpeza, gasta mais tempo
		_delay_ms(2);
		
		DADOS_LCD = (DADOS_LCD & 0xF0) | (c & 0x0F);//segundo nibble, 4 LSB
		
		i--;
		
	}while(i>0);
}

//-----------------------------------------------------------------------------------------------------------

void cmd_LCD_Printf(char c, FILE *stream)		//c é o dado  e cd indica se é instrução ou caractere
{
	unsigned char i=2;
	 char cd = 1;

	DADOS_LCD = (DADOS_LCD & 0xF0)|(c>>4);	//primeiro os 4 MSB. (PA0 - PA1 - PA2 - PA3) -> (D4 - D5 - D6 - D7 LCD)
	//como a outra metade da porta pode ser utilizada para outros fins, não
	do										//se pode alterar o seu conteúdo
	{	if(cd==0)
		clr_bit(CONTR_LCD,RS);
		else
		set_bit(CONTR_LCD,RS);

		pulso_enable;

		if((cd==0) && (c<4))				//se for instrução de retorno ou limpeza, gasta mais tempo
		_delay_ms(2);
		
		DADOS_LCD = (DADOS_LCD & 0xF0) | (c & 0x0F);//segundo nibble, 4 LSB
		
		i--;
		
	}while(i>0);
}
//-----------------------------------------------------------------------------------------------------------
//Sub-rotina para inicialização do LCD com via de dados de 4 bits
//-----------------------------------------------------------------------------------------------------------
void inic_LCD_4bits()		//sequência ditada pelo fabricando do circuito integrado HD44780
{							//o LCD será só escrito. Então, R/W é sempre zero.

	//configuração a direção de dados das portas
	DDR_DADOS_LCD = 0x0F;	//configura 4 bits menos significativos da DDR como saida
	DDR_CONTR_LCD = (DDR_CONTR_LCD|(1<<E)|(1<<RS)); //Configura pinos de controle como saida
	_delay_us(1);						//ajuste dos pinos

	clr_bit(CONTR_LCD,RS);	//RS em zero indicando que o dado para o LCD será uma instrução
	clr_bit(CONTR_LCD,E);	//pino de habilitação em zero
	
	_delay_ms(20);	 		//tempo para estabilizar a tensão do LCD, após VCC ultrapassar 4.5 V (na prática pode
	//ser maior).
	DADOS_LCD = (DADOS_LCD & 0xF0) | 0x03;//interface de 8 bits (é importante utilizar este comando para não
	//haver problemas na inicialização seguida do LCD (reset do microcontrolador)
	pulso_enable;			//enable respeitando os tempos de resposta do LCD
	_delay_ms(5);
	pulso_enable;
	_delay_us(200);
	pulso_enable;			//até aqui ainda é uma interface de 8 bits.
	//Muitos programadores desprezam os comandos acima, respeitando apenas o tempo de
	//estabilização da tensão (geralmente funciona). Se o LCD não for inicializado primeiro no
	//modo de 8 bits, haverá problemas se o microcontrolador for inicializado e o display já o //tiver sido.

	DADOS_LCD = (DADOS_LCD & 0xF0) | 0x02;//interface de 4 bits, deve ser enviado duas vezes (a outra esta abaixo)
	
	pulso_enable;

	cmd_LCD(LCD_4BIT,0); 	//interface de 4 bits 2 linhas (aqui se habilita as 2 linhas)
	//são enviados os 2 nibbles (0x2 e 0x8)
	cmd_LCD(LCD_OFF_DISPLAY,0);	//desliga o display
	cmd_LCD(LCD_CLR,0);	//limpa todo o display
	cmd_LCD(LCD_OFF_CURSOR,0);	//mensagem aparente cursor ativo não piscando
	cmd_LCD(LCD_LINHA1,0);	//escreve na primeira posição a esquerda - 1a linha
}
//-----------------------------------------------------------------------------------------------------------
//Sub-rotina de escrita no LCD
//-----------------------------------------------------------------------------------------------------------
void escreve_LCD(char *c)
{
	for (; *c!=0;c++) cmd_LCD(*c,1);
}

//-----------------------------------------------------------------------------------------------

void EscreveCaracterLCD(char caracter)
{
	cmd_LCD(caracter, 1);
}
//-----------------------------------------------------------------------------------------------
//Sub-rotina de posicionar LCD
//-----------------------------------------------------------------------------------------------
//FUNCIONA APENAS PARA DUAS LINHAS
void LCD_setPos(int posx, int posy) {
	int posiciona=0;

	if(posx==1) posiciona=LCD_LINHA1+posy;
	if(posx==2) posiciona=LCD_LINHA2+posy;

	cmd_LCD(posiciona,0);
}
//-----------------------------------------------------------------------------------------------
//Sub-rotina de limpeza de matriz selecionável LCD
//-----------------------------------------------------------------------------------------------
void Limpa_matriz_LCD(int posx,int posy,int limpa) {
	unsigned char x;
	if(limpa==0)	cmd_LCD(LCD_CLR,0);
	LCD_setPos(posx,posy);
	for(x=0;x<limpa;x++){
		cmd_LCD(0x20,1);
	}
}

//-----------------------------------------------------------------------------------------------
//Sub-rotina para limpar todo o LCD
//-----------------------------------------------------------------------------------------------

void Limpa_LCD_RetornaCarro() {
	cmd_LCD(LCD_CLR,0);
}

//-----------------------------------------------------------------------------------------------
