//----------------------------------------------------------------------------

//System timer class

//----------------------------------------------------------------------------

#include "main.hpp"
#include "systimer.hpp"

//----------------------------- Constants: -----------------------------------

#define CLK_PER_US (SYSTEM_CORE_CLOCK / 1000000)
#define CLK_PER_MS (SYSTEM_CORE_CLOCK / 1000)
#define MS_PER_S   (1000)

#define INPUT_PULL 0x88888888 //enable inputs pullups (CNFx_1 = 1)

//----------------------- System initialization: -----------------------------

void SystemInit(void)
{
  //turning on the PVD and waiting for power to be established:
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR = PWR_CR_PVDE | PWR_CR_PLS_2V9;
  while(PWR->CSR & PWR_CSR_PVDO);

  //Clocking from HSI using PLL:
  RCC->CFGR |= RCC_CFGR_PLLMULL6;         //HSI / 2 * 6 = 24 MHz
  RCC->CR |= RCC_CR_PLLON;                //PLL enable
  while((RCC->CR & RCC_CR_PLLRDY) == 0);  //wait for PLL ready
  RCC->CFGR &= ~RCC_CFGR_SW;              //clear SW bits
  RCC->CFGR |= RCC_CFGR_SW_PLL;           //select clocking from PLL
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1); //wait for PLL select

/*
  //Not used due to increased noise level on the ADC
  //Clocking from HSE using PLL:
  RCC->CR |=  RCC_CR_HSEON;               //HSE generator on (8 MHz)
  while((RCC->CR & RCC_CR_HSERDY) == 0);  //wait for HSE ready
  RCC->CFGR |= RCC_CFGR_PLLSRC;           //clocking PLL from HSE
  RCC->CFGR |= RCC_CFGR_PLLMULL3;         //HSE (8 MHz) * 3 = 24 MHz
  RCC->CR |= RCC_CR_PLLON;                //PLL enable
  while((RCC->CR & RCC_CR_PLLRDY) == 0);  //wait for PLL ready
  RCC->CFGR |= RCC_CFGR_SW_PLL;           //select clocking from PLL
  while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); //wait for PLL select
  RCC->CR &= ~RCC_CR_HSION;               //turn off HSI
*/

  //Enable clock for all ports:
  RCC->APB2ENR |=
    RCC_APB2ENR_AFIOEN |
    RCC_APB2ENR_IOPAEN |
    RCC_APB2ENR_IOPBEN |
    RCC_APB2ENR_IOPCEN |
    RCC_APB2ENR_IOPDEN;
  //Setting all ports to pull-down input(CNFx_1 = 1),
  //leaving CMOS inputs hanging in the air is not good.
  GPIOA->ODR = 0; //JTAG pins reset
  GPIOB->ODR = 0; //JTAG pins reset
  GPIOA->CRL = INPUT_PULL;
  GPIOA->CRH = INPUT_PULL;
  GPIOB->CRL = INPUT_PULL;
  GPIOB->CRH = INPUT_PULL;
  GPIOC->CRL = INPUT_PULL;
  GPIOC->CRH = INPUT_PULL;
  GPIOD->CRL = INPUT_PULL;
  GPIOD->CRH = INPUT_PULL;
}

//----------------------------------------------------------------------------
//--------------------------- TSysTimer class --------------------------------
//----------------------------------------------------------------------------

//--------------------- Initializing the system timer: -----------------------

void TSysTimer::Init(void)
{
  Counter = 0;
  SysTick_Config(CLK_PER_MS);
}

//------------------------ System timer interrupt: ---------------------------

volatile uint32_t TSysTimer::Counter;

void SysTick_Handler(void)
{
  TSysTimer::Counter++;
}

//-------------------------- Reset second timer: -----------------------------

#ifdef USE_SEC
void TSysTimer::SecReset(void)
{
  SecCount = Counter;
}
#endif

//------------------- Synchronization of system ticks: -----------------------

bool TSysTimer::Tick;
#ifdef USE_SEC
  bool TSysTimer::SecTick;
  uint32_t TSysTimer::SecCount;
#endif

void TSysTimer::Sync(void)
{
  Tick = (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk);
#ifdef USE_SEC
  if(Tick && (Counter - SecCount >= MS_PER_S))
  {
    SecTick = 1;
    SecCount += MS_PER_S;
  }
  else
  {
    SecTick = 0;
  }
#endif
}

//----------------------- Microsecond delay function: ------------------------

void TSysTimer::Delay_us(uint16_t d)
{
  uint32_t DelayStart = SysTick->VAL;
  uint32_t DelayTicks = d * CLK_PER_US;
  int32_t  Delta;
  do
  {
    Delta = DelayStart - SysTick->VAL;
    if(Delta < 0) Delta += CLK_PER_MS;
  }
  while(Delta < DelayTicks);
}

//----------------------- Millisecond delay function: ------------------------

void TSysTimer::Delay_ms(uint32_t d)
{
  uint32_t DelayStart = Counter;
  while(Counter - DelayStart < d);
}

//------------------------ Timeout microsecond range: ------------------------

uint32_t TSysTimer::Start_us;
uint32_t TSysTimer::Interval_us;

void TSysTimer::TimeoutStart_us(uint16_t t)
{
  Start_us = SysTick->VAL;
  Interval_us = t * CLK_PER_US;
}

bool TSysTimer::TimeoutOver_us(void)
{
  int32_t Delta = Start_us - SysTick->VAL;
  if(Delta < 0) Delta += CLK_PER_MS;
  return(Delta >= Interval_us);
}

//----------------------- Timeout millisecond range: -------------------------

uint32_t TSysTimer::Start_ms;
uint32_t TSysTimer::Interval_ms;

void TSysTimer::TimeoutStart_ms(uint32_t t)
{
  Start_ms = Counter;
  Interval_ms = t;
}

bool TSysTimer::TimeoutOver_ms(void)
{
  return(Counter - Start_ms >= Interval_ms);
}

//----------------------------------------------------------------------------
//-------------------------- TSoftTimer class: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TSoftTimer::TSoftTimer(uint32_t t)
{
  Autoreload = 0;
  Oneshot = 0;
  Event = (t == 0)? 1 : 0;
  Interval = t;
  StartCount = TSysTimer::Counter;
}

//-------------------------------- Start: ------------------------------------

void TSoftTimer::Start()
{
  Event = 0;
  StartCount = TSysTimer::Counter;
}

void TSoftTimer::Start(uint32_t t)
{
  Interval = t;
  Event = (t == 0)? 1 : 0;
  StartCount = TSysTimer::Counter;
}

//--------------------------- Interval setting: ------------------------------

void TSoftTimer::SetInterval_ms(uint32_t t)
{
  Interval = t;
}

void TSoftTimer::SetInterval_sec(uint32_t t)
{
  Interval = t * 1000;
}

void TSoftTimer::SetInterval_min(uint32_t t)
{
  Interval = t * 60000;
}

void TSoftTimer::SetInterval_hrs(uint32_t t)
{
  Interval = t * 3600000;
}

//--------------------------- Forced overflow: -------------------------------

void TSoftTimer::Force(void)
{
  StartCount = TSysTimer::Counter - Interval;
}

//-------------------------- Reading overflow: -------------------------------

bool TSoftTimer::Over(void)
{
  bool event = TSysTimer::Counter - StartCount >= Interval;
  if(event)
  {
    if(Oneshot && Event) event = 0;
    Event = 1;
    if(Autoreload)
    {
      StartCount = TSysTimer::Counter; //restart
      Event = 0;
    }
  }
  if(!Oneshot && !Autoreload) event = Event;
  return(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
