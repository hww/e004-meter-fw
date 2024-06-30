//----------------------------------------------------------------------------

//Brightness control class

//-------------------------- Resources Used: ---------------------------------

//Brightness can be adjusted in three ways:

//1. Adjustment with trimmer pots (BrMode = BRM_VR).
//The pots are powered from the DAC1 and DAC2 pins, and the output of the pots
//is connected to the PWM1 and PWM2 pins, from where the signal goes to the
//filters of the voltage regulators. In this mode, logic 1 must be output to the
//DAC1 and DAC2 pins, and the PWM1 and PWM2 pins must be switched to the Z-state.
//The brightness of the bars is adjusted using the pot R180.
//The brightness of the backlight is adjusted using the pot R181.

//2. Adjustment with DAC (BrMode = BRM_DAC). From the DAC1 and DAC2 outputs,
//the signal through jumpers installed instead of trimmers, goes to PWM1 and
//PWM2 pins, from where the signal goes to the filters of the voltage regulators.
//PWM1 and PWM2 pins must be switched to the Z-state.
//The brightness of the bars is adjusted using the DAC2 (PA5 pin).
//The brightness of the backlight is adjusted using the DAC1 (PA4 pin).

//3. Adjustment with PWM (BrMode = BRM_PWM). In this case, trimming pots are not
//installed. From the outputs PWM1 and PWM2 the signal goes to the filters of
//the voltage regulators. For TIM15, remap pins are used.
//The brightness of the bars is adjusted using the PWM1 (PB15, TIM15 CH2 pin).
//The brightness of the backlight is adjusted using the PWM2 (PB14, TIM15 CH1 pin).

//Update: brightness control with PWM causes an increase in noise, so it's not
//used. STM32F103 does not have built-in DACs, so adjustment with their help
//is not possible for this chip. In practice, the most convenient is the
//adjustment with pots, and it is used in the latest versions.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "bright.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//----------------------------------------------------------------------------
//----------------------------- TBright class: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TBright::TBright(void)
{
  BrMeter = 0;
  BrScale = 0;
  SetMode(BRM_OFF);
}

//----------------------------------------------------------------------------
//-------------------------- Additional Methods: -----------------------------
//----------------------------------------------------------------------------

//------------------------ Brightness control off: ---------------------------

void TBright::OffMode(void)
{
#ifdef STM32F10X_MD_VL
  Dac1.Off();
  Dac2.Off();
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_LO);      //R181 power off
  Pin_DAC2.Init(OUT_PP_2M, OUT_LO);      //R180 power off
#endif
  PwmOff();
  Pin_PWM1.Init(OUT_PP_2M, OUT_LO);      //R181 power off
  Pin_PWM2.Init(OUT_PP_2M, OUT_LO);      //R180 power off
}

//---------------- Brightness control with trimmer pots: ---------------------

void TBright::VrMode(void)
{
#ifdef STM32F10X_MD_VL
  Dac1.Off();
  Dac2.Off();
#endif
  PwmOff();
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //R181 power on
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //R180 power on
}

//---------------- Brightness control with built-in DACs: --------------------

void TBright::DacMode(void)
{
  PwmOff();
#ifdef STM32F10X_MD_VL
  Dac1.Init();                           //DAC1 setup
  Dac2.Init();                           //DAC2 setup
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //R181 power on
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //R180 power on
#endif
  SetMeter(BrMeter);                     //set bars brightness
  SetScale(BrScale);                     //set backlight brightness
}

//------------------- Brightness control with PWM: ---------------------------

void TBright::PwmMode(void)
{
#ifdef STM32F10X_MD_VL
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;   //TIM15 clock enable
  TIM15->PSC = 0;                        //clear prescaler
  TIM15->ARR = PWM_MAX;                  //period
  TIM15->CCR1 = 0;                       //PWM code
  TIM15->CCMR1 =
    TIM_CCMR1_OC2M_0 * 6 |               //OC2 PWM mode 1
    TIM_CCMR1_OC2PE  * 1 |               //CCR2 preload enable
    TIM_CCMR1_OC1M_0 * 6 |               //OC1 PWM mode 1
    TIM_CCMR1_OC1PE  * 1;                //CCR1 preload enable
  TIM15->CCER =
    TIM_CCER_CC2P    * 0 |               //OC2 high active
    TIM_CCER_CC2E    * 1 |               //OC2 enable
    TIM_CCER_CC1P    * 0 |               //OC1 high active
    TIM_CCER_CC1E    * 1;                //OC1 enable
  TIM15->EGR = TIM_EGR_UG;               //update regs from shadow regs
  TIM15->BDTR = TIM_BDTR_MOE;            //main output enable
  TIM15->CR1 = TIM_CR1_CEN;              //timer enable

  AFIO->MAPR2 |= AFIO_MAPR2_TIM15_REMAP; //remap TIM15
  Pin_PWM1.Init(AF_PP_10M);              //PWM1 pin setup
  Pin_PWM2.Init(AF_PP_10M);              //PWM2 pin setup
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //R181 power on
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //R180 power on
#endif

  SetMeter(BrMeter);                     //set bars brightness
  SetScale(BrScale);                     //set backlight brightness
}

//-------------------------------- PWM off: ----------------------------------

void TBright::PwmOff(void)
{
#ifdef STM32F10X_MD_VL
  TIM15->CCER = 0;                        //OC1 and OC2 disable
  TIM15->BDTR = 0;                        //main output disable
  TIM15->CR1 = 0;                         //timer disable
  RCC->APB2ENR &= ~RCC_APB2ENR_TIM15EN;   //TIM15 clock disable
#endif
  Pin_PWM1.Init(IN_ANALOG);               //PWM1 pin setup
  Pin_PWM2.Init(IN_ANALOG);               //PWM2 pin setup
}

//----------------------------------------------------------------------------
//-------------------------- Interface Methods: ------------------------------
//----------------------------------------------------------------------------

//------------------- Switch brightness control modes: -----------------------

void TBright::SetMode(uint8_t m)
{
  if(BrMode != m)
  {
    switch(m)
    {
    case BRM_OFF: OffMode(); break;
    case BRM_VR:  VrMode();  break;
    case BRM_DAC: DacMode(); break;
    case BRM_PWM: PwmMode(); break;
    }
    BrMode = m;
  }
}

//--------------------------- Set bar brightness: ----------------------------

void TBright::SetMeter(uint8_t b)
{
  if(b > PER_MAX) b = PER_MAX;
  BrMeter = b;
#ifdef STM32F10X_MD_VL
  uint16_t v = b * PWM_MAX / PER_MAX;
  if(BrMode == BRM_DAC) Dac2 = v << 4;
  if(BrMode == BRM_PWM) TIM15->CCR2 = v;
#endif
}

//------------------------ Set backlight brightness: -------------------------

void TBright::SetScale(uint8_t b)
{
  if(b > PER_MAX) b = PER_MAX;
  BrScale = b;
#ifdef STM32F10X_MD_VL
  uint16_t v = b * PWM_MAX / PER_MAX;
  if(BrMode == BRM_DAC) Dac1 = v << 4;
  if(BrMode == BRM_PWM) TIM15->CCR1 = v;
#endif
}

//------------------- Reading parameters from EEPROM: ------------------------

void TBright::EERead(uint8_t p)
{
  SetMode(Eeprom->Read(EE_BRMODE, BMD_P[p]));
  SetMeter(Eeprom->Read(EE_MBRGHT, BRM_P[p]));
  SetScale(Eeprom->Read(EE_SBRGHT, BRS_P[p]));
}

//--------------------- Saving parameters in EEPROM: -------------------------

void TBright::EESave(void)
{
  Eeprom->Write(EE_BRMODE, BrMode);
  Eeprom->Write(EE_MBRGHT, BrMeter);
  Eeprom->Write(EE_SBRGHT, BrScale);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
