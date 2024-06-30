//----------------------------------------------------------------------------

//LED display support class

//-------------------------- Resources Used: ---------------------------------

//Uses a LED bar display assembled from LED bar graph arrays.
//Bars with 35, 40 and 50 elements are supported. In the 50-element version,
//10 additional LEDs are used to illuminate the stencils with transparent
//symbols of operating modes. Separate SMD LEDs are used to illuminate the scale.
//To reduce the noise level, a static indication is used - the LEDs are
//connected to the outputs of the 74HC595 register chain.
//Registers are accessed through the TSreg class.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "display.hpp"

//----------------------------------------------------------------------------
//---------------------------- TDisplay class --------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TDisplay::TDisplay(void)
{
  Sreg.Init(REGS);
#ifdef METER40
  Pin_BKL.Init(OUT_PP_2M, OUT_LO);
#endif
  Clear();
  Update();
  Sreg.Enable(1);
}

//----------------------------------------------------------------------------
//-------------------------- Interface Methods: ------------------------------
//----------------------------------------------------------------------------

//---------------------------- Display clear: --------------------------------

void TDisplay::Clear(void)
{
  for(uint8_t i = 0; i < REGS; i++)
    RegData[i] = 0xFF;
  Leds(TRNON);
}

//---------------------------- Display update: -------------------------------

void TDisplay::Update(void)
{
  Sreg.Load(RegData);
}

//-------------------- Segment switch on for L channel: ----------------------

void TDisplay::SegL(uint8_t s, bool on)
{
  if(s < SEGS)
  {
    Regs_t Reg = SegsL[s].r;
    Outs_t Out = SegsL[s].q;
    if(on)
      RegData[Reg] &= ~(1 << Out);
        else RegData[Reg] |= (1 << Out);
  }
}

//-------------------- Segment switch on for R channel: ----------------------

void TDisplay::SegR(uint8_t s, bool on)
{
  if(s < SEGS)
  {
    Regs_t Reg = SegsR[s].r;
    Outs_t Out = SegsR[s].q;
    if(on)
      RegData[Reg] &= ~(1 << Out);
        else RegData[Reg] |= (1 << Out);
  }
}

//---------------------- Bar switch on for L channel: ------------------------

void TDisplay::BarL(uint8_t n)
{
  for(uint8_t i = 0; i < SEGS; i++) SegL(i, i <= n);
}

//---------------------- Bar switch on for R channel: ------------------------

void TDisplay::BarR(uint8_t n)
{
  for(int i = 0; i < SEGS; i++) SegR(i, i <= n);
}

//--------------------- LEDs and backlight switch on: ------------------------

void TDisplay::Leds(uint16_t s)
{
#ifdef METER50
  s ^= SCALE;
  for(uint8_t i = 0; i < TRS; i++)
  {
    Regs_t Reg = Transp[i].r;
    Outs_t Out = Transp[i].q;
    if(s & 1)
      RegData[Reg] &= ~(1 << Out);
        else RegData[Reg] |= (1 << Out);
    s = s >> 1;
  }
#endif

#ifdef METER40
  Pin_BKL = s & SCALE;
#endif

#ifdef METER35
  Regs_t Reg = Transp[SCA].r;
  Outs_t Out = Transp[SCA].q;
  if(s & SCALE)
    RegData[Reg] &= ~(1 << Out);
      else RegData[Reg] |= (1 << Out);
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
