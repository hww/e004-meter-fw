//----------------------------------------------------------------------------

//Brightness control class, header file

//----------------------------------------------------------------------------

#ifndef BRIGHT_H
#define BRIGHT_H

//----------------------------------------------------------------------------

#ifdef STM32F10X_MD_VL
  #include "dac.hpp"
#endif

//----------------------------------------------------------------------------
//------------------------------- Constants: ---------------------------------
//----------------------------------------------------------------------------

//Brightness control modes:
enum BrightMode_t
{
  BRM_OFF, //brightness control off
  BRM_VR,  //brightness control with trimmer pots
  BRM_DAC, //brightness control with built-in DACs
  BRM_PWM  //brightness control with PWM
};

//----------------------------------------------------------------------------
//----------------------------- TBright class: -------------------------------
//----------------------------------------------------------------------------

class TBright
{
private:
  Pin_PWM1_t Pin_PWM1;
  Pin_PWM2_t Pin_PWM2;
  Pin_DAC1_t Pin_DAC1;
  Pin_DAC2_t Pin_DAC2;
#ifdef STM32F10X_MD_VL
  TDac<DAC1> Dac1;
  TDac<DAC2> Dac2;
#endif
  uint8_t BrMode;
  uint8_t BrMeter;
  uint8_t BrScale;
  static const uint16_t PWM_MAX = 255;
  static const uint16_t PER_MAX = 100;
  void OffMode(void);
  void VrMode(void);
  void DacMode(void);
  void PwmMode(void);
  void PwmOff(void);
public:
  TBright(void);
  void SetMode(uint8_t m);
  inline uint8_t GetMode(void) { return(BrMode); };
  void SetMeter(uint8_t b);
  inline uint8_t GetMeter(void) { return(BrMeter); };
  void SetScale(uint8_t b);
  inline uint8_t GetScale(void) { return(BrScale); };
  void EERead(uint8_t p);
  void EESave(void);
};

//----------------------------------------------------------------------------

extern TBright *Bright;

//----------------------------------------------------------------------------

#endif
