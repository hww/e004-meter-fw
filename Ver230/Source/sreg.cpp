//----------------------------------------------------------------------------

//Serial register support class

//-------------------------- Resources Used: ---------------------------------

//A chain of external registers 74HC595 is used, which are connected
//to the SPI1 port:
//SCLK clock line connected to SCK (PB3, remap is used)
//SDATA data line connected to MOSI (PB5, using remap)
//LOAD line is connected to pin PB6
//OE output enable line is connected to pin PB7
//The number of registers in the chain is passed to the Init function.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "sreg.hpp"

//----------------------------------------------------------------------------
//----------------------------- TSreg class: ---------------------------------
//----------------------------------------------------------------------------

//--------------------------- Initialization: --------------------------------

void TSreg::Init(uint8_t n)
{
  Regs = n;
  Pin_SCLK.Init(AF_PP_50M);
  Pin_SDATA.Init(AF_PP_50M);
  Pin_LOAD.Init(OUT_PP_50M, OUT_HI);
  Pin_OE.Init(OUT_OD_2M, OUT_HI);

  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE | //JTAG disable
                AFIO_MAPR_SPI1_REMAP;           //remap SPI1 pins

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //SPI1 clock enable

  SPI1->CR1 =              //SPI setup:
    SPI_CR1_CPHA     * 1 | //CPHA = 1
    SPI_CR1_CPOL     * 1 | //CPOL = 1
    SPI_CR1_MSTR     * 1 | //master
    SPI_CR1_BR_0     * SPI_DIV | //APB2 clock divider
    SPI_CR1_SPE      * 1 | //SPI enable
    SPI_CR1_LSBFIRST * 0 | //MSB first
    SPI_CR1_SSI      * 1 | //master
    SPI_CR1_SSM      * 1 | //software NSS
    SPI_CR1_RXONLY   * 0 | //full duplex
    SPI_CR1_DFF      * 0 | //8 bit data frame
    SPI_CR1_CRCNEXT  * 0 | //CRC not used
    SPI_CR1_CRCEN    * 0 |
    SPI_CR1_BIDIOE   * 1 | //TX only
    SPI_CR1_BIDIMODE * 1;  //1-line mode
}

//-------------------------- Write byte via SPI1: ----------------------------

void TSreg::SpiWr(uint8_t d)
{
  //waiting for a buffer to be freed:
  while(!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = d;
}

//----------------------------- Registers load: ------------------------------

void TSreg::Load(uint8_t *Value)
{
  Pin_LOAD = 0;
  //transfer REGS bytes:
  for(uint8_t n = 0; n < Regs; n++)
    SpiWr(Value[n]);
  //waiting for the end of the transmission of the last byte:
  while(!(SPI1->SR & SPI_SR_TXE));
  while(SPI1->SR & SPI_SR_BSY);
  Pin_LOAD = 1;
}

//------------------- Enable/disable registers outputs: ----------------------

void TSreg::Enable(bool en)
{
  Pin_OE = !en;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
