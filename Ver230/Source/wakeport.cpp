//----------------------------------------------------------------------------

//Wake protocol based port class

//----------------------------------------------------------------------------

#include "main.hpp"
#include "wakeport.hpp"

//----------------------------------------------------------------------------
//--------------------------- TWakePort class: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TWakePort::TWakePort(uint32_t baud, uint8_t frame) : TWake(frame)
{
  TWakePort::Wp = this;
  //ports init:
  Pin_TXD.Init(AF_PP_2M, OUT_HI);
  Pin_RXD.Init(IN_PULL, PULL_UP);
  //USART2 init:
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  USART1->BRR = APB2_CLOCK / baud;
  USART1->CR1 =
    USART_CR1_RE |     //receiver enable
    USART_CR1_TE |     //transmitter enablr
    USART_CR1_RXNEIE | //RXNE interrupt enable
    USART_CR1_UE;      //USART2 enable
  //interrupts init:
  NVIC_SetPriority(USART1_IRQn, 15);
  NVIC_EnableIRQ(USART1_IRQn);
}

//-------------------------- USART1 interrupt: -------------------------------

TWakePort* TWakePort::Wp;

void USART1_IRQHandler(void)
{
  //USART RX interrupt:
  if(USART1->SR & USART_SR_RXNE)
  {
    TWakePort::Wp->Rx(USART1->DR);
  }
  //USART TX interrupt:
  if(USART1->SR & USART_SR_TXE)
  {
    uint8_t data;
    if(TWakePort::Wp->Tx(data))
      USART1->DR = data;
        else
          USART1->CR1 &= ~USART_CR1_TXEIE; //TXE interrupt disable
  }
  //uint8_t x; Addr = &x; //TODO: stack profiling
}

//--------------------------- Packet transmit: -------------------------------

void TWakePort::StartTx(uint8_t cmd)
{
  uint8_t data;
  TxStart(cmd, data);
  USART1->DR = data;
  USART1->CR1 |= USART_CR1_TXEIE; //TXE interrupt enable
}

//----------------------------------------------------------------------------
