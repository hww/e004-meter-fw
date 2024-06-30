//----------------------------------------------------------------------------

//Модуль общего управления, заголовочный файл

//----------------------------------------------------------------------------

#ifndef CONTROL_HPP
#define CONTROL_HPP

//----------------------------------------------------------------------------

#include "main.hpp"
#include "display.hpp"
#include "process.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "dac.hpp"

//----------------------------------------------------------------------------
//---------------------------- Класс TControl: -------------------------------
//----------------------------------------------------------------------------

class TControl
{
private:
  TEeprom  *Eeprom;
  TDisplay *Display;
  TProcess *ProcessL;
  TProcess *ProcessR;
  //TDac<DAC1> Dac1;
  //TDac<DAC2> Dac2;
  bool UpdL, UpdR;
  uint16_t Bar, Peak;
  uint8_t Point;
  static int16_t dBTable[SEGS];
  uint16_t BarL;
  uint16_t BarR;
  uint16_t PeakL;
  uint16_t PeakR;
  void EERead(void);
public:
  TControl(void);
  void Execute(void);
  bool Upd;
  //TODO: void SetMode(uint8_t m);
  //TODO: uint8_t GetMode(void);
  //TODO: чтение и сброс статистики
  inline uint16_t GetBarL(void)  { return(BarL);  };
  inline uint16_t GetBarR(void)  { return(BarR);  };
  inline uint16_t GetPeakL(void) { return(PeakL); };
  inline uint16_t GetPeakR(void) { return(PeakR); };
  void SetTab(uint8_t n, int16_t v);
  int16_t GetTab(uint8_t n);
  void EESave(void);
};

//----------------------------------------------------------------------------

extern TControl *Control;

//----------------------------------------------------------------------------

#endif
