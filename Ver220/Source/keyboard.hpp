//----------------------------------------------------------------------------

//Keyboard support class: header file

//----------------------------------------------------------------------------

#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

//------------------------------- Constants: ---------------------------------

enum KeyMsg_t //keyboard messages
{
  KBD_NOP    = 0x00, //no press
  KBD_MODE   = 0x01, //MODE button
  KBD_STAT   = 0x02, //STAT button
  KBD_HOLD   = 0x80, //hold flag
  KBD_STATH  = KBD_STAT + KBD_HOLD, //STAT button hold
};

//----------------------------------------------------------------------------
//--------------------------- TKeyboard class: -------------------------------
//----------------------------------------------------------------------------

class TKeyboard
{
private:
  Pin_MODE_t Pin_MODE;
  Pin_STAT_t Pin_STAT;
  TSoftTimer *KeyboardTimer;
  enum KbdStates_t { KS_DEBNC, KS_PRESS, KS_HOLD };
  uint8_t State;
  uint8_t Prev_Key;
  static const uint16_t DEBOUNCE_TM = 50;  //debounce time, ms
  static const uint16_t HOLD_DELAY = 2000; //button hold delay, ms
  uint8_t Scan(void);
public:
  TKeyboard(void);
  uint8_t Execute(void);
  uint8_t Jumpers(void);
};

//----------------------------------------------------------------------------

#endif
