//----------------------------------------------------------------------------

//����� ����� � ���������� Wake

//----------------------------------------------------------------------------

#include "main.hpp"
#include "wakeport.hpp"

//----------------------------------------------------------------------------
//--------------------------- ����� TWakePort --------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------

TWakePort::TWakePort(uint32_t baud, uint8_t frame) : TWake(frame)
{
  TWakePort::Wp = this;
  //��������� ������:
  Pin_TXD.Init(AF_PP_2M, OUT_HI);
  Pin_RXD.Init(IN_PULL, PULL_UP);
  //��������� USART2:
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  USART1->BRR = APB2_CLOCK / baud;
  USART1->CR1 =
    USART_CR1_RE |     //���������� ���������
    USART_CR1_TE |     //���������� �����������
    USART_CR1_RXNEIE | //���������� ���������� RXNE
    USART_CR1_UE;      //���������� USART2
  //��������� ����������:
  NVIC_SetPriority(USART1_IRQn, 15);
  NVIC_EnableIRQ(USART1_IRQn);
}

//-------------------------- ���������� USART1: ------------------------------

TWakePort* TWakePort::Wp;

void USART1_IRQHandler(void)
{
  //���������� USART �� ������:
  if(USART1->SR & USART_SR_RXNE)
  {
    TWakePort::Wp->Rx(USART1->DR);
  }
  //���������� USART �� ��������:
  if(USART1->SR & USART_SR_TXE)
  {
    uint8_t data;
    if(TWakePort::Wp->Tx(data))
      USART1->DR = data;
        else
          USART1->CR1 &= ~USART_CR1_TXEIE; //������ ���������� TXE
  }
  //uint8_t x; Addr = &x; //TODO: stack profiling
}

//--------------------------- �������� ������: -------------------------------

void TWakePort::StartTx(uint8_t cmd)
{
  uint8_t data;
  TxStart(cmd, data);
  USART1->DR = data;
  USART1->CR1 |= USART_CR1_TXEIE; //���������� ���������� TXE
}

//----------------------------------------------------------------------------
