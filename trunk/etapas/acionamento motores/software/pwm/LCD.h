// -----------------------------------------------------------------------------
// Arquivo:			LCD.h
// Modulo:			Display de cristal l�quido
// Autores:			Leandro Schwarz
//					Charles Borges de Lima
// Vers�o:			2.2
// Modificado em:	21/06/2011
// Observa��es:		Para utilizar, � necess�rio alterar os valor das constantes
//					LCD_DADOS, LCD_CONTROLE, LCD_RS, LCD_E e LCD_D4, definidas 
//					na seq��ncia
// -----------------------------------------------------------------------------

#ifndef __LCD_H
#define __LCD_H

// -----------------------------------------------------------------------------
// Defini��es configur�veis ----------------------------------------------------

#define LCD_DADOS			PORTB
#define LCD_CONTROLE		PORTB
#define LCD_RS				PB5
#define LCD_E				PB4
#define LCD_D4				PB0

// -----------------------------------------------------------------------------
// Defini��es das fun��es macro necess�rias ------------------------------------

#define __lcd_set_bit(endereco,bit) (endereco|=(1 << bit))
#define __lcd_clr_bit(endereco,bit) (endereco&=~(1 << bit))
#define __lcd_tst_bit(endereco,bit) ((endereco>>bit)&1)
#define __lcd_cpl_bit(endereco,bit) (endereco^=(1 << bit))
#define pulso_enable 	_delay_us(1); __lcd_set_bit(LCD_CONTROLE,LCD_E); _delay_us(1); __lcd_clr_bit(LCD_CONTROLE,LCD_E); _delay_us(45)

// -----------------------------------------------------------------------------
// Defini��es das fun��es macro de controle ------------------------------------

#define LCD_display_liga()				LCD_comando(0x0C,0)
#define LCD_display_desliga()			LCD_comando(0x0A,0)
#define LCD_display_limpa()				LCD_comando(0x01,0)
#define LCD_direcao_esquerda_direita()	LCD_comando(0x04,0)
#define LCD_direcao_direita_esquerda()	LCD_comando(0x06,0)
#define LCD_cursor_liga()				LCD_comando(0x0E,0)
#define LCD_cursor_desliga()			LCD_comando(0x0C,0)
#define LCD_cursor_retorna()			LCD_comando(0x10,0)
#define LCD_cursor_avanca()				LCD_comando(0x14,0)
#define LCD_cursor_inicio()				LCD_comando(0x02,0)
#define LCD_cursor_piscante()			LCD_comando(0x0D,0)
#define LCD_cursor_alternante()			LCD_comando(0x0F,0)
#define LCD_move_primeira_linha()		LCD_comando(0x80,0)
#define LCD_move_segunda_linha()		LCD_comando(0xC0,0)

// -----------------------------------------------------------------------------
// Declara��o das fun��es da bibioteca -----------------------------------------

void LCD_comando(unsigned char c, char cd);
void LCD_inicia_4bits(void);
void LCD_escreve(char *c);

#endif
