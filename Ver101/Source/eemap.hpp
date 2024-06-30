//----------------------------------------------------------------------------

//Карта EEPROM

//----------------------------------------------------------------------------

#ifndef EEMAP_HPP
#define EEMAP_HPP

//----------------------------------------------------------------------------

#include "display.hpp"

//---------------------------- Индексы пресетов: -----------------------------

enum EE_PreIdx_t
{
  EE_PRE1,
  EE_PRE2,
  EE_PRE3,
  EE_PRE4,
  EE_PREN
};

//----------------------------- Карта пресета: -------------------------------

enum EE_Preset_t
{
  EE_SIGN,     //signature

  EE_INTEG1_T, //bar params
  EE_DECAY1_T,
  EE_SCALE1_T,
  EE_RESP1_T,
  EE_FALL1_T,
  EE_HOLD1_T,
  EE_MFALL1_T,
  EE_MHOLD1_T,

  EE_INTEG2_T, //peak params
  EE_DECAY2_T,
  EE_SCALE2_T,
  EE_RESP2_T,
  EE_FALL2_T,
  EE_HOLD2_T,
  EE_MFALL2_T,
  EE_MHOLD2_T,

  EE_DB_TAB,   //scale table
  EE_DB_TAB_END = EE_DB_TAB + SEGS - 1,

  EE_DMODE,    //display mode
  EE_BRMODE,   //brightness control mode
  EE_MBRGHT,   //meter brightness
  EE_SBRGHT,   //scale brightness
  EE_LEDS,     //leds set

  EE_PRE_SIZE  //preset size (in 16-bit words)
};

//----------------------------- Карта EEPROM: --------------------------------

enum EE_Map_t
{
  EE_PRESET1,
  EE_PRESET2 = EE_PRESET1 + EE_PRE_SIZE,
  EE_PRESET3 = EE_PRESET2 + EE_PRE_SIZE,
  EE_PRESET4 = EE_PRESET3 + EE_PRE_SIZE,
  EE_END
};

//----------------------------------------------------------------------------

#endif
