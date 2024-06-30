//----------------------------------------------------------------------------

//Keyboard support class

//-------------------------- Resources Used: ---------------------------------

//The "MODE" and "STAT" buttons are connected to the Pin_MODE and Pin_STAT ports,
//respectively. When the button is pressed, the port level goes LOW.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "keyboard.hpp"

//----------------------------------------------------------------------------
//--------------------------- TKeyboard class: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TKeyboard::TKeyboard(void)
{
  Pin_MODE.Init(IN_PULL, PULL_UP);
  Pin_STAT.Init(IN_PULL, PULL_UP);
  State = KS_DEBNC;
  Prev_Key = KBD_NOP;
  KeyboardTimer = new TSoftTimer();
  KeyboardTimer->Oneshot = 1;
}

//----------------------------- Keyboard scan: -------------------------------

inline uint8_t TKeyboard::Scan(void)
{
  char s = KBD_NOP;
  if(!Pin_MODE) s |= KBD_MODE; //MODE button
  if(!Pin_STAT) s |= KBD_STAT; //STAT button
  return((KeyMsg_t)s);
}

//--------------------- Keyboard debounce/hold detect: -----------------------

uint8_t TKeyboard::Execute(void)
{
  uint8_t Msg = KBD_NOP;
  uint8_t Key = Scan();                //keyboard scan
  if(Key != Prev_Key)                  //if the code is different
  {
    KeyboardTimer->Start(DEBOUNCE_TM); //start debounce timer
    State = KS_DEBNC;                  //debounce state
    Prev_Key = Key;                    //save new code
  }
  else if(Key != KBD_NOP)              //code the same and not KBD_NOP
  {
    if(State == KS_DEBNC)              //if debounce state
    {
      if(KeyboardTimer->Over())        //check debounce timer
      {
        State = KS_PRESS;              //if over, press state
        Msg = Key;                     //press message
        KeyboardTimer->Start(HOLD_DELAY); //start hold timer
      }
    }
    else if(State == KS_PRESS)         //if press state
    {
      if(KeyboardTimer->Over())        //check hold timer
      {
        State = KS_HOLD;               //if over, hold state
        Msg = Key + KBD_HOLD;          //hold message
      }
    }
  }
  return(Msg);
}

//--------------------- Direct read jumpers state: ---------------------------

uint8_t TKeyboard::Jumpers(void)
{
  uint8_t j = 0;
  if(!Pin_MODE) j = j | 1;
  if(!Pin_STAT) j = j | 2;
  return(j);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
