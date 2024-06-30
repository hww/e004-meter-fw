//----------------------------------------------------------------------------

//Модуль поддержки последовательного регистра

//----------------------- Используемые ресурсы: ------------------------------

//Используются цепочка внешних регистров 74HC595, которые подключены
//к порту SPI1:
//линия тактирования SCLK подключена к SCK (PB3, используется remap)
//линия данных SDATA подключена к MOSI (PB5, используется remap)
//линия загрузки LOAD подключена к пину PB6
//линия разрешения выхода OE подключена к пину PB7

//----------------------------------------------------------------------------

#include "main.hpp"
#include "sreg.hpp"

//----------------------------------------------------------------------------
//----------------------------- Класс TSreg: ---------------------------------
//----------------------------------------------------------------------------

//---------------------------- Инициализация: --------------------------------

void TSreg::Init(void)
{
  Pin_SCLK.Init(AF_PP_50M);
  Pin_SDATA.Init(AF_PP_50M);
  Pin_LOAD.Init(OUT_PP_50M, OUT_HI);
  Pin_OE.Init(OUT_OD_2M, OUT_HI);

  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE | //JTAG disable
                AFIO_MAPR_SPI1_REMAP;           //remap выводов SPI1

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //включение тактирования SPI1

  SPI1->CR1 =              //настройка SPI:
    SPI_CR1_CPHA     * 1 | //CPHA = 1
    SPI_CR1_CPOL     * 1 | //CPOL = 1
    SPI_CR1_MSTR     * 1 | //мастер
    SPI_CR1_BR_0     * 0 | //тактовая частота APB2 / 2
    SPI_CR1_SPE      * 1 | //разрешение SPI
    SPI_CR1_LSBFIRST * 0 | //MSB first
    SPI_CR1_SSI      * 1 | //мастер
    SPI_CR1_SSM      * 1 | //программное управление NSS
    SPI_CR1_RXONLY   * 0 | //full duplex
    SPI_CR1_DFF      * 0 | //8 bit data frame
    SPI_CR1_CRCNEXT  * 0 | //CRC не используется
    SPI_CR1_CRCEN    * 0 |
    SPI_CR1_BIDIOE   * 1 | //TX only
    SPI_CR1_BIDIMODE * 1;  //1-line mode
}

//TODO: DMA + IRQ

  /*
  SPI1->CR2 = SPI_CR2_TXDMAEN; //разрешение DMA по TXE

  RCC->AHBENR |= RCC_AHBENR_DMA1EN;   //включение тактирования DMA1
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

//Или использовать прерывания по SPI_SR_TXE?
//Что делать в конце? Ждать SPI_SR_BSY?
//Или все это не имеет смысла по сравнению с временем формирования битов для передачи?

//----------------------- Запись байта через SPI1: ---------------------------

void TSreg::SpiWr(uint8_t d)
{
  //ожидание освобождения буфера:
  while(!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = d;
}

//-------------------------- Загрузка регистров: -----------------------------

void TSreg::Load(uint8_t *Value)
{
  Pin_LOAD = 0;
  //передача REGS байт:
  for(uint8_t n = 0; n < REGS; n++)
    SpiWr(Value[n]);
  //ожидание окончания передачи последнего байта:
  while(!(SPI1->SR & SPI_SR_TXE));
  while(SPI1->SR & SPI_SR_BSY);
  Pin_LOAD = 1;
}

//---------------- Разрешение/запрещение выходов регистров: ------------------

void TSreg::Enable(bool en)
{
  Pin_OE = !en;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
