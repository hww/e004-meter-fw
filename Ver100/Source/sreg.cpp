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

#include "main.hpp"
#include "sreg.hpp"

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
    SPI_CR1_CPHA     * 1 | //CPHA = 1
    SPI_CR1_CPOL     * 1 | //CPOL = 1
    SPI_CR1_MSTR     * 1 | //������
    SPI_CR1_BR_0     * 0 | //�������� ������� APB2 / 2
    SPI_CR1_SPE      * 1 | //���������� SPI
    SPI_CR1_LSBFIRST * 0 | //MSB first
    SPI_CR1_SSI      * 1 | //������
    SPI_CR1_SSM      * 1 | //����������� ���������� NSS
    SPI_CR1_RXONLY   * 0 | //full duplex
    SPI_CR1_DFF      * 0 | //8 bit data frame
    SPI_CR1_CRCNEXT  * 0 | //CRC �� ������������
    SPI_CR1_CRCEN    * 0 |
    SPI_CR1_BIDIOE   * 1 | //TX only
    SPI_CR1_BIDIMODE * 1;  //1-line mode
}

//TODO: DMA + IRQ

  /*
  SPI1->CR2 = SPI_CR2_TXDMAEN; //���������� DMA �� TXE

  RCC->AHBENR |= RCC_AHBENR_DMA1EN;   //��������� ������������ DMA1
  DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR; //periph. address
  DMA1_Channel3->CMAR = (uint32_t)RegBuff;   //memory address
  DMA1_Channel3->CNDTR = REGS;               //buffer size

  DMA1_Channel3->CCR =
    DMA_CCR2_MEM2MEM * 0 | //memory to memory off
    DMA_CCR2_PL_0    * 0 | //low priority
    DMA_CCR2_MSIZE_0 * 0 | //mem. size 8 bit
    DMA_CCR2_PSIZE_0 * 0 | //periph. size 8 bit
    DMA_CCR2_MINC    * 1 | //memory increment enable
    DMA_CCR2_PINC    * 0 | //periph. increment disable
    DMA_CCR2_CIRC    * 0 | //circular mode disable
    DMA_CCR2_DIR     * 1 | //direction - from memory
    DMA_CCR2_TEIE    * 0 | //transfer error interrupt disable
    DMA_CCR2_HTIE    * 0 | //half transfer interrupt disable
    DMA_CCR2_TCIE    * 0 | //transfer complete interrupt disable - enable!
    DMA_CCR2_EN      * 1;  //DMA enable
  */

//��� ������������ ���������� �� SPI_SR_TXE?
//��� ������ � �����? ����� SPI_SR_BSY?
//��� ��� ��� �� ����� ������ �� ��������� � �������� ������������ ����� ��� ��������?

//----------------------- ������ ����� ����� SPI1: ---------------------------

void TSreg::SpiWr(uint8_t d)
{
  //�������� ������������ ������:
  while(!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = d;
}

//-------------------------- �������� ���������: -----------------------------

void TSreg::Load(uint8_t *Value)
{
  Pin_LOAD = 0;
  //�������� REGS ����:
  for(uint8_t n = 0; n < REGS; n++)
    SpiWr(Value[n]);
  //�������� ��������� �������� ���������� �����:
  while(!(SPI1->SR & SPI_SR_TXE));
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
