#ifndef _LCD4BITS_H_
#define _LCD4BITS_H_

#define F_CPU 16000000UL

#include <avr/io.h> 	    //definições do componente especificado
#include <util/delay.h>		//biblioteca para o uso das rotinas de _delay_ms e _delay_us()
#include <stdio.h>

//Definições
#define DADOS_LCD    	PORTB  	//4 bits de dados do LCD LSB da porta 
#define DDR_DADOS_LCD   DDRB

#define CONTR_LCD 		PORTB  	//para facilitar a troca dos pinos do hardware e facilitar a re-programação
#define DDR_CONTR_LCD	DDRB
#define E    			5    //pino de instrução ou dado para o LCD
#define RS   			4     //pino de enable do LCD


#define LCD_CLR 0x01			//limpeza do display com retorno do cursor
#define LCD_OFF_CURSOR 0x0C 	//mensagem aparente cursor ativo não piscando
#define LCD_OFF_DISPLAY 0x08	//desliga o DISPLAY  
#define LCD_4BIT 0x28			//modo 4 bits, matriz 7x5

#define NUM_LIN_LCD 16			//define o numero de linhas do LCD
#define NUM_COL_LCD 2			//define onumero de colunas do LCD
#define LCD_LINHA1	0x80		//define primeiro endereço da memória, linha 1
#define LCD_LINHA2	0xC0		//define segundo endereço endereço de memória, linha 2

//sinal de habilitação para o LCD
#define pulso_enable 	_delay_us(1); set_bit(CONTR_LCD,E); _delay_us(1); clr_bit(CONTR_LCD,E); _delay_us(45)

void cmd_LCD(unsigned char c, char cd);
void cmd_LCD_Printf(char c, FILE *stream);
void inic_LCD_4bits();
void escreve_LCD(char *c);
void EscreveCaracterLCD(char caracter);
void LCD_setPos(int posx, int posy);
void Limpa_matriz_LCD(int posx,int posy,int limpa);
void Limpa_LCD_RetornaCarro();

#endif