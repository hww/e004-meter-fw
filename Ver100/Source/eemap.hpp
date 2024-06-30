//----------------------------------------------------------------------------

//Карта EEPROM

//----------------------------------------------------------------------------

#ifndef EEMAP_HPP
#define EEMAP_HPP

//----------------------------------------------------------------------------

#include "display.hpp"

//----------------------------- Карта EEPROM: --------------------------------

//Адреса слов в EEPROM:

//TODO: presets

enum EE_Map_t
{
  EE_SIGN,

  EE_INF1_A,
  EE_INF2_A,
  EE_DEF1_A,
  EE_DEF2_A,
  EE_RSF1_A,
  EE_RSF2_A,
  EE_FLF1_A,
  EE_FLF2_A,
  EE_SCL1_A,
  EE_SCL2_A,
  EE_HLD1_A,
  EE_HLD2_A,

  EE_DB_TAB,
  EE_DB_TAB_END = EE_DB_TAB + SEGS - 1,

  EE_SIZE //EEPROM size (in 16-bit words)
};

//----------------------------------------------------------------------------

#endif
