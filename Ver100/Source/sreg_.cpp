//----------------------------------------------------------------------------

//������ ��������� ����������������� ��������

//----------------------- ������������ �������: ------------------------------

//������������ ������� ������� ��������� 74HC595, ������� ����������
//� ����� SPI1:
//����� ������������ SCLK ���������� � SCK (PB3, ������������ remap)
//����� ������ SDATA ���������� � MOSI (PB5, ������������ remap)
//����� �������� LOAD ���������� � ���� PB6
//����� ���������� ������ OE ���������� � ���� PB7

//----------------------------------------------------------------------------

#include "main.h"
#include "sreg.h"

//----------------------------------------------------------------------------
//----------------------------- ����� TSreg: ---------------------------------
//----------------------------------------------------------------------------

//---------------------------- �������������: --------------------------------

void TSreg::Init(void)
{
  Pin_SCLK.Init(AF_PP_50M);
  Pin_SDATA.Init(AF_PP_50M);
  Pin_LOAD.Init(OUT_PP_50M, OUT_HI);
  Pin_OE.Init(OUT_OD_2M, OUT_HI);

  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE | //JTAG disable
                AFIO_MAPR_SPI1_REMAP;           //remap ������� SPI1

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //��������� ������������ SPI1

  SPI1->CR1 =              //��������� SPI:
    SPI_CR1_CPHA     * 0 | //CPHA = 0
    SPI_CR1_CPOL     * 0 | //CPOL = 0
    SPI_CR1_MSTR     * 1 | //������
    SPI_CR1_BR_0     * 1 | //�������� ������� APB2 / 4
    SPI_CR1_LSBFIRST * 0 | //MSB first
    SPI_CR1_SSI      * 1 | //������
    SPI_CR1_SSM      * 1 | //����������� ���������� NSS
    SPI_CR1_RXONLY   * 0 | //full duplex
    SPI_CR1_DFF      * 0 | //8 bit data frame
    SPI_CR1_CRCNEXT  * 0 | //CRC �� ������������
    SPI_CR1_CRCEN    * 0 |
    SPI_CR1_BIDIOE   * 1 | //TX only
    SPI_CR1_BIDIMODE * 1 | //1-line mode
    SPI_CR1_SPE      * 1;  //���������� SPI
}

//----------------------- ������ ����� ����� SPI1: ---------------------------

void TSreg::SpiWr(uint8_t d)
{
  //�������� ������������ ������:
  while(!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = d;
}

//-------------------------- �������� ���������: -----------------------------

void TSreg::Load(uint8_t* Value)
{
  Pin_LOAD = 0;
  //�������� REGS ����:
  for(uint8_t i = REGS, n = 0; n < REGS; n++)
    SpiWr(Value[--i]);
  //�������� ��������� �������� ���������� �����:
  while(SPI1->SR & SPI_SR_BSY);
  Pin_LOAD = 1;
}

//---------------- ����������/���������� ������� ���������: ------------------

void TSreg::Enable(bool en)
{
  Pin_OE = !en;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
