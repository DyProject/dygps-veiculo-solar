#include "spidef.h"

//--------------------------------------------------------------

void SPI_MasterInit()
{
	/*	Seta MOSI, SCK e SS como saída. SS como saída se torna um poino de uso 
		geral. MISO automaticamente como entrada */
	SPI_DDR |= (1 << SPI_DD_MOSI) | (1 << SPI_DD_SCK) | (1 << SPI_DD_SS);
	
	SPI_DESABILITA_SLAVE();
	
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR);
	
	SPI_SCK_FOSC_DIV_16();
}

//--------------------------------------------------------------

void SPI_Transmit(char cData)
{
	/* Inicia transmissão. Clock gerado automaticamente */
	SPDR = cData;
	
	/* Wait for transmission complete */
	while(!(SPSR & (1 << SPIF)));
}

//--------------------------------------------------------------

void SPI_SlaveInit()
{
	/* Seta MISO como saída, todos os outros como entrada automaticamente */
	SPI_DDR = (1 << SPI_DD_MISO);
		
	/*SS deve ser mantido em baixo para ativar SPI no modo slave*/
	SPI_PORT &= (~(1 << SPI_DD_SS));
	
	/* Habilita SPI */
	SPCR = (1 << SPE);
}

//--------------------------------------------------------------

char SPI_Receive()
{
	/* Wait for reception complete */
	while(!(SPSR & (1 << SPIF)));
	
	/* Return Data Register */
	return SPDR;
}

//--------------------------------------------------------------

