//----------------------------------------------------------------------------

//I2C bus software implementation class

//-------------------------- Resources Used: ---------------------------------

//Pins SCL (PB8) and SDA (PB9) are used.
//The TIM16 (TIM4 for STM32F103) timer is used to generate bus timings.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "i2csw.hpp"

//----------------------------------------------------------------------------
//----------------------------- TI2Csw class: --------------------------------
//----------------------------------------------------------------------------

//---------------------------- Initialization: -------------------------------

TGpio<PORTB, PIN8> TI2Csw::Pin_SCL;
TGpio<PORTB, PIN9> TI2Csw::Pin_SDA;

void TI2Csw::Init(void)
{
  Pin_SCL.Init(OUT_OD_2M, OUT_HI);
  Pin_SDA.Init(OUT_OD_2M, OUT_HI);

#ifdef STM32F10X_MD_VL
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //TIM16 clock enable
  TIM16->PSC = 0;                      //prescaler clear
  TIM16->ARR = UINT16_MAX;             //period
  TIM16->CR1 = TIM_CR1_CEN;            //timer enable
#else
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;  //TIM4 clock enable
  TIM4->PSC = 0;                       //prescaler clear
  TIM4->ARR = UINT16_MAX;              //period
  TIM4->CR1 = TIM_CR1_CEN;             //timer enable
#endif

  Free(); //I2C reset
  Stop();
}

//--------------------------- I2C bus release: -------------------------------

void TI2Csw::Free(void)
{
  for(uint8_t i = 0; i < 9; i++)
  {
    if(Pin_SDA) break;
    Pin_SCL = 0;
    BitDelay();
    Pin_SCL = 1;
    BitDelay();
  }
}

//----------------- Generation of "START" condition on I2C: ------------------

void TI2Csw::Start(void)
{
  Pin_SDA = 0;
  BitDelay();
}

//------------------------- Writing a byte via I2C: --------------------------

bool TI2Csw::Write(uint8_t data)
{
  bool ack = 0;
  for(uint8_t i = 0; i < 8; i++)
  {
    Pin_SCL = 0;
    if(data & 0x80)
      Pin_SDA = 1;
        else Pin_SDA = 0;
    data <<= 1;
    BitDelay();
    Pin_SCL = 1;
    BitDelay();
  }
  Pin_SCL = 0;
  Pin_SDA = 1;
  BitDelay();
  Pin_SCL = 1;
  BitDelay();
  ack = !Pin_SDA;
  BitDelay();
  return(ack);
}

//------------------------ Reading a byte via I2C: ---------------------------

uint8_t TI2Csw::Read(bool ack)
{
  uint8_t data;
  for(uint8_t i = 0; i < 8; i++)
  {
    Pin_SCL = 0;
    Pin_SDA = 1;
    BitDelay();
    Pin_SCL = 1;
    BitDelay();
    data = data << 1;
    if(Pin_SDA) data |= 0x01;
  }
  Pin_SCL = 0;
  Pin_SDA = !ack;
  BitDelay();
  Pin_SCL = 1;
  BitDelay();
  return(data);
}

//----------------- Generation of "STOP" condition on I2C: -------------------

void TI2Csw::Stop(void)
{
  Pin_SCL = 0;
  Pin_SDA = 0;
  BitDelay();
  Pin_SCL = 1;
  BitDelay();
  Pin_SDA = 1;
  BitDelay();
}

//------------------------- Delay 1/2 period I2C: ----------------------------

void TI2Csw::BitDelay(void)
{
#ifdef STM32F10X_MD_VL
  TIM16->CNT = 0;
  while(TIM16->CNT < I2C_DELAY);
#else
  TIM4->CNT = 0;
  while(TIM4->CNT < I2C_DELAY);
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
