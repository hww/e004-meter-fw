//----------------------------------------------------------------------------

//Класс управления яркостью, заголовочный файл

//----------------------------------------------------------------------------

#ifndef BRIGHT_H
#define BRIGHT_H

//----------------------------------------------------------------------------

#include "dac.hpp"

//----------------------------------------------------------------------------
//------------------------------- Константы: ---------------------------------
//----------------------------------------------------------------------------

//Режимы управления яркостью:
enum BrightMode_t
{
  BRM_OFF, //управление выключено
  BRM_VR,  //управление переменными резисторами
  BRM_DAC, //управление с помощью встроенных ЦАП
  BRM_PWM  //управление с помощью ШИМ
};

//----------------------------------------------------------------------------
//--------------------------- Класс TBacklight: ------------------------------
//----------------------------------------------------------------------------

class TBright
{
private:
  Pin_PWM1_t Pin_PWM1;
  Pin_PWM2_t Pin_PWM2;
  Pin_DAC1_t Pin_DAC1;
  Pin_DAC2_t Pin_DAC2;
  TDac<DAC1> Dac1;
  TDac<DAC2> Dac2;
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
