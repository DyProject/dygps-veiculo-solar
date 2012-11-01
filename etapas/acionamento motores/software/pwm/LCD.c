// -----------------------------------------------------------------------------
// Arquivo:			LCD.c
// Modulo:			Display de cristal líquido
// Autores:			Leandro Schwarz
//					Charles Borges de Lima
// Versão:			2.2
// Modificado em:	21/06/2011
// Observações:		Para utilizar, é necessário alterar os valor das constantes
//					LCD_DADOS, LCD_CONTROLE, LCD_RS, LCD_E e LCD_D4, definidas 
//					no arquivo de cabeçalho
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Arquivos necessários --------------------------------------------------------

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "LCD.h"

// -----------------------------------------------------------------------------
// Função:		LCD_inicia_4bits
// Argumentos:	Nenhum
// Retono:		Nenhum
// Objetivo:	Executa ações para incialização do LCD
// -----------------------------------------------------------------------------

void LCD_inicia_4bits(void)
{
	__lcd_clr_bit(LCD_CONTROLE,LCD_RS);
	__lcd_clr_bit(LCD_CONTROLE,LCD_E);

	_delay_ms(20);

	LCD_DADOS = (LCD_DADOS & (0xF0 >> LCD_D4)) | (0x03 << LCD_D4);
	pulso_enable;
	_delay_ms(5);
	pulso_enable;
	_delay_ms(200);
	pulso_enable;

	LCD_DADOS = (LCD_DADOS & (0xF0 >> LCD_D4)) | (0x02 << LCD_D4);

	pulso_enable;


   	LCD_comando(0x28,0);	//interface de 4 bits 2 linhas (aqui se habilita as 2 linhas) 
						//são enviados os 2 nibbles (0x2 e 0x8)
   	
   	LCD_comando(0x08,0);	//desliga o display
   	LCD_display_limpa();
   	LCD_display_liga();   
   	LCD_move_primeira_linha();
}

// -----------------------------------------------------------------------------
// Função:		LCD_comando
// Argumentos:	caractere e tipo
// Retono:		Nenhum
// Objetivo:	Envia o caractere para o LCD na forma de comando (quando cd = 0)
//				ou texto (quando cd = 1)
// -----------------------------------------------------------------------------

void LCD_comando(unsigned char c, char cd)
{
	unsigned char i = 2;

	LCD_DADOS = (LCD_DADOS & (0xF0 >> LCD_D4)) | ((c >> 4) << LCD_D4);
	do
	{
		if(cd == 0)
			__lcd_clr_bit(LCD_CONTROLE,LCD_RS);
		else
			__lcd_set_bit(LCD_CONTROLE,LCD_RS);
		
		pulso_enable;
		
		if((cd == 0) && (c < 4))
			_delay_ms(2);
		LCD_DADOS = (LCD_DADOS & (0xF0 >> LCD_D4)) | ((c & 0x0F) << LCD_D4);
		i--;
	}while(i > 0);
}

// -----------------------------------------------------------------------------
// Função:		LCD_escreve
// Argumentos:	string de texto
// Retono:		Nenhum
// Objetivo:	Escreve a string de caracteres no LCD
// -----------------------------------------------------------------------------

void LCD_escreve(char *c)
{
	for (;*c != 0;c++)  LCD_comando(*c,1);
}
