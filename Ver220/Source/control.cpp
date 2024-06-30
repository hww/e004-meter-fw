//----------------------------------------------------------------------------

//Common control class

//----------------------------------------------------------------------------

#include "control.hpp"
#include "bright.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//---------------------------- Resources Used: -------------------------------

//The TControl class performs common control of the measurement process.
//The Execute methods of the TProcess class for each of the channels are
//called In a loop. These methods do ADC reading and digital processing of
//readings. The calculated values of the levels are updated at a frequency
//of 375 Hz, then the ready flags UpdL and UpdR are set. When the data of the
//two channels is ready, they are read. Depending on the display mode DispMode,
//certain values are read. Values that are not displayed remain at zero.
//Read values are shown on the display. At the same time, the preset switching
//service works. The full set of parameters for the selected preset is read
//from EEPROM and loaded into all modules.

//------------------------------ Variables: ----------------------------------

TBright *Bright;
TEeprom *Eeprom;

//----------------------------------------------------------------------------
//---------------------------- TControl class: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TControl::TControl(void)
{
  Eeprom = new TEeprom();
  Display = new TDisplay();
  Keyboard = new TKeyboard();
  ProcessL = new TProcess(CHANL);
  ProcessR = new TProcess(CHANR);
  Bright = new TBright();

#ifdef USE_LEDS_MUX
  Pin_Mux0.Init(OUT_PP_2M, OUT_LO);
  Pin_Mux1.Init(OUT_PP_2M, OUT_LO);
  Pin_Mux2.Init(OUT_PP_2M, OUT_LO);
  Pin_MuxOut.Init(IN_PULL, PULL_DN);
  MuxData = 0;
  MuxEn = 0;
#endif

  Jumpers = PRESETS;
  Preset = PRESETS;

#ifdef USE_KEYBOARD
  ChangePreset(Eeprom->ReadP());
#else
  PresetsService();
#endif

  InitTest();

  UpdL = 0;
  UpdR = 0;
  Upd = 0;
}

//--------------------------- Execute control: -------------------------------

void TControl::Execute(void)
{
  //data ready check for channel L:
  if(ProcessL->Execute()) UpdL = 1;
  //data ready check for channel R:
  if(ProcessR->Execute()) UpdR = 1;
  //if both channels are ready, display:
  if(UpdL && UpdR)
  {
    ReadValues();    //read measured values
    DisplayValues(); //display values
    UpdL = 0;        //ready flags clear
    UpdR = 0;
    Upd = 1;         //set common ready flag
  }

  //services execution:
  if(TSysTimer::Tick) //every system tick
  {
    PresetsService(); //switch presets
#ifdef USE_LEDS_MUX
    MuxService();     //LEDs control using MUX
#endif
  }
}

//----------------------------------------------------------------------------
//----------------------- Services Implementation: ---------------------------
//---------------- (Services are called every system tick) -------------------
//----------------------------------------------------------------------------

//------------------------ Presets switch service: ---------------------------

void TControl::PresetsService(void)
{
#ifdef USE_KEYBOARD
  uint8_t k = Keyboard->Execute();
  if(k == KBD_MODE)
  {
    uint8_t p = Preset;
    if(DispMode & DP_ABSMAX)
    {
      Preset = PRESETS;
    }
    else
    {
      if(++p == PRESETS) p = 0;
    }
    ChangePreset(p);
    if(p != Eeprom->ReadP())
      Eeprom->WriteP(p);
  }
  else if(k == KBD_STAT)
  {
    SetDispMode(DispMode | DP_ABSMAX);
    LedsSet |= TR8;
    Display->Leds(LedsSet);
  }
  else if(k == KBD_STATH)
  {
    StatReset();
  }
#else
  uint8_t j = Keyboard->Jumpers();
  if(Jumpers != j)
  {
    Jumpers = j;
    ChangePreset(j);
  }
#endif
}

//----------------------- LEDs control using MUX service: --------------------

//#define BASIC_MUX
#define E004_MUX
//#define UNI_MUX
//#define SERGEM_MUX

#ifdef USE_LEDS_MUX
void TControl::MuxService(void)
{
  if(MuxEn && MuxRead())
  {
    LedsSet = 0;

#ifdef BASIC_MUX
    if(Mux(MUX7 | MUX8, MUX7))
                        LedsSet |= TR0; //TR0 = MUX8:MUX7 == MUX7 (9)
    else if(Mux(MUX7 | MUX8, MUX8))
                        LedsSet |= TR2; //TR2 = MUX8:MUX7 == MUX8 (38)
    else                LedsSet |= TR1; //TR1 = !TR0 & !TR2 (19)
    if(Mux(MUX6, MUX6)) LedsSet |= TR3; //TR3 = MUX6 (Auto Reverse)
    if(Mux(MUX5, MUX5)) LedsSet |= TR4; //TR4 = MUX5 (Cycle)
    if(Mux(MUX4, MUX4)) LedsSet |= TR5; //TR5 = MUX4 (NR)
    if(Mux(MUX3, MUX3)) LedsSet |= TR6; //TR6 = MUX3 (REC)
    if(Mux(MUX2, MUX2)) LedsSet |= TR7; //TR7 = MUX2 (SRC)
    else                LedsSet |= TR8; //TR8 = !MUX2 (TAPE)
    if(Mux(MUX1, MUX1)) LedsSet |= TR9; //TR9 = MUX1 (MUTE)
#endif

#ifdef E004_MUX
    if(Mux(MUX8, 0))    LedsSet |= TR0; //TR0 = !MUX4 (9)
    if(Mux(MUX8, MUX8)) LedsSet |= TR1; //TR1 = MUX4 (19)
                                        //TR2 = 0 (38)
    if(Mux(MUX7, MUX7)) LedsSet |= TR3; //TR3 = MUX7 (Auto Reverse)
    if(Mux(MUX6, 0))    LedsSet |= TR4; //TR4 = !MUX6 (Cycle)
    if(Mux(MUX5, MUX5)) LedsSet |= TR5; //TR5 = MUX5 (NR)
    if(Mux(MUX4, 0))    LedsSet |= TR6; //TR6 = !MUX4 (REC)
    if(Mux(MUX2, MUX2) || Mux(MUX3, MUX3))
      LedsSet |= TR7;                   //TR7 = MUX2 | MUX3 (SRC)
    if(Mux(MUX2, 0) || Mux(MUX3, 0))
      LedsSet |= TR8;                   //TR8 = !MUX2 | !MUX3 (TAPE)
    if(Mux(MUX1, MUX1)) LedsSet |= TR9; //TR9 = MUX1 (Mute)
#endif

#ifdef UNI_MUX
    if(Mux(MUX8, MUX8)) LedsSet |= TR0; //TR0 = MUX8 (REC)
    if(Mux(MUX7, MUX7)) LedsSet |= TR2; //TR2 = MUX7 (SRC)
    else                LedsSet |= TR1; //TR1 = !MUX7 (TAPE)
    if(Mux(MUX6, MUX6)) LedsSet |= TR4; //TR4 = MUX6 (19)
    else                LedsSet |= TR3; //TR3 = !MUX6 (9)
    if(Mux(MUX5, MUX5)) LedsSet |= TR5; //TR5 = MUX5 (FF)
    if(Mux(MUX4, MUX4)) LedsSet |= TR6; //TR6 = MUX4 (Reverse)
    if(Mux(MUX3, MUX3)) LedsSet |= TR7; //TR7 = MUX3 (PAUSE)
    if(Mux(MUX2, MUX2)) LedsSet |= TR8; //TR8 = MUX2 (FW)
    if(Mux(MUX1, MUX1)) LedsSet |= TR9; //TR9 = MUX1 (REW)
#endif

#ifdef SERGEM_MUX
                                        //TR0 = 0 (9)
    if(Mux(MUX4, 0))    LedsSet |= TR1; //TR1 = !MUX4 (19)
    if(Mux(MUX4, MUX4)) LedsSet |= TR2; //TR2 = MUX4 (38)
    LedsSet |= TR3;                     //TR3 = 1 (NAB)
                                        //TR4 = 0 (CCIR)
    if(Mux(MUX3, MUX3)) LedsSet |= TR5; //TR5 = MUX3 (NR)
    if(Mux(MUX1, MUX1) || Mux(MUX2, MUX2))
      LedsSet |= TR6;                   //TR6 = MUX1 | MUX2 (SRC)
    if(Mux(MUX1, 0) || Mux(MUX2, 0))
      LedsSet |= TR7;                   //TR7 = !MUX1 | !MUX2 (TAPE)
                                        //TR8 = 0 (CAL)
                                        //TR9 = 0 (BIAS)
#endif

    LedsSet |= SCALE; //scale is allways on in this mode
    Display->Leds(LedsSet);
    //Display->Update() is called in Execute() method (DisplayValues()).
  }
}
#endif

//----------------------------------------------------------------------------
//---------------------------- Auxiliary Methods: ----------------------------
//----------------------------------------------------------------------------

//------------------------- Initial bars lighting: ---------------------------

void TControl::InitTest(void)
{
  Display->BarL(SEGS);
  Display->BarR(SEGS);
  Display->Update();
  Display->Leds(TSALL);
  Display->Update();

#ifdef USE_LEDS_MUX
  do TSysTimer::Delay_ms(1);
  while(!MuxRead());
  MuxEn = MuxData;
#endif

  TSysTimer::Delay_ms(TEST_ON);
  Display->Leds(SCALE);
  Display->Update();

  for(uint8_t i = SEGS; i > 0; i--)
  {
    Display->BarL(i);
    Display->BarR(i);
    Display->Update();
    TSysTimer::Delay_ms(TEST_FALL / SEGS);
  }
#ifdef USE_LEDS_MUX
  if(MuxEn) LedsSet = SCALE;
#endif
  Display->Leds(LedsSet);
  Display->Update();
}

//------------------------- Read measured values: ----------------------------

#pragma inline = forced
void TControl::ReadValues(void)
{
  if(DispMode & DP_BAR)
  {
    BarL = ProcessL->GetBar();
    BarR = ProcessR->GetBar();
  }
  if(DispMode & DP_PEAK)
  {
    PeakL = ProcessL->GetPeak();
    PeakR = ProcessR->GetPeak();
  }
  if(DispMode & DP_BARMAX)
  {
    BarMaxL  = ProcessL->GetBarMax();
    BarMaxR  = ProcessR->GetBarMax();
  }
  if(DispMode & DP_PEAKMAX)
  {
    PeakMaxL = ProcessL->GetPeakMax();
    PeakMaxR = ProcessR->GetPeakMax();
  }
  if(DispMode & DP_ABSMAX)
  {
    BarL = ProcessL->GetBarAbsMax();
    BarR = ProcessR->GetBarAbsMax();
    PeakL = ProcessL->GetPeakAbsMax();
    PeakR = ProcessR->GetPeakAbsMax();
    BarMaxL = 0;
    BarMaxR = 0;
    PeakMaxL = 0;
    PeakMaxR = 0;
  }
}

//---------------------------- Display level: --------------------------------

#pragma inline = forced
void TControl::DisplayValues(void)
{
  PeakSegL    = SEGS;
  PeakSegR    = SEGS;
  PeakMaxSegL = SEGS;
  PeakMaxSegR = SEGS;
  BarMaxSegL  = SEGS;
  BarMaxSegR  = SEGS;
  //PeakSegL    = SEGS;
  //PeakSegR    = SEGS;
  for(uint8_t i = 0; i < SEGS; i++)
  {
    int16_t dBc = dBcTable[i];
    Display->SegL(i, BarL >= dBc);
    Display->SegR(i, BarR >= dBc);
    if(PeakL >= dBc) PeakSegL = i;
    if(PeakR >= dBc) PeakSegR = i;
    if(PeakMaxL >= dBc) PeakMaxSegL = i;
    if(PeakMaxR >= dBc) PeakMaxSegR = i;
    if(BarMaxL >= dBc) BarMaxSegL = i;
    if(BarMaxR >= dBc) BarMaxSegR = i;
  }
  Display->SegL(PeakSegL, 1);
  Display->SegR(PeakSegR, 1);
  Display->SegL(PeakMaxSegL, 1);
  Display->SegR(PeakMaxSegR, 1);
  Display->SegL(BarMaxSegL, 1);
  Display->SegR(BarMaxSegR, 1);
  Display->Update();
}

//------------------------ dB to code convertion: ----------------------------

uint16_t TControl::dB2Code(uint16_t dB)
{
  if(dB > 8000) return(0);
  uint32_t y = DB_001[dB & 0xF];
  y = y *  DB_016[(dB >> 4)  & 0xF] / 65536;
  y = y *  DB_256[(dB >> 8)  & 0xF] / 65536;
  y = y * DB_4096[(dB >> 12) & 0x1] / 65536;
  y = y * 10000 / 65536;
  return(y);
}

//------------------------------- Read MUX: ----------------------------------

#ifdef USE_LEDS_MUX
bool TControl::MuxRead(void)
{
  static uint8_t mux_addr = 0;
  static uint8_t mux_data = 0;
  bool res = 0;
  if(Pin_MuxOut) mux_data |= 1 << mux_addr; //read MUX
  if(++mux_addr == MUX_INS) //next MUX address
  {
    mux_addr = 0;
    MuxData = mux_data;
    mux_data = 0;
    res = 1;
  }
  Pin_Mux0 = mux_addr & 1; //setup MUX address lines
  Pin_Mux1 = mux_addr & 2;
  Pin_Mux2 = mux_addr & 4;
  return(res);
}
#endif

//------------------------------ Check MUX: ----------------------------------

#ifdef USE_LEDS_MUX
inline bool TControl::Mux(uint8_t mask, uint8_t val)
{
  return((MuxData & mask) == val);
}
#endif

//----------------------------------------------------------------------------
//-------------------------- Interface Methods: ------------------------------
//----------------------------------------------------------------------------

//----------------------------- Load table: ----------------------------------

void TControl::SetTab(uint8_t n, int16_t v)
{
  if(n < SEGS) dBTable[n] = v;
}

//-------------------------- Codes table update: -----------------------------

void TControl::ApplyTab(void)
{
  int16_t dB_Max = dBTable[SEGS - 1];
  for(uint8_t i = 0; i < SEGS; i++)
  {
    dBcTable[i] = dB2Code(dB_Max - dBTable[i]);
  }
}

//------------------------------ Read table: ---------------------------------

int16_t TControl::GetTab(uint8_t n)
{
  return(dBTable[n]);
}

//---------------------------- Switch preset: --------------------------------

void TControl::ChangePreset(uint8_t p)
{
  if(p < PRESETS && p != Preset)
  {
    Preset = p;
    Eeprom->SelectPreset(Preset);
    EERead();
    ProcessL->ResetFilters();
    ProcessR->ResetFilters();
  }
}

//--------------------------- Switch display mode: ---------------------------

void TControl::SetDispMode(uint8_t m)
{
  DispMode = m;
  BarL     = 0; BarR     = 0;
  PeakL    = 0; PeakR    = 0;
  BarMaxL  = 0; BarMaxR  = 0;
  PeakMaxL = 0; PeakMaxR = 0;
}

//---------------------------- Reset statistics: -----------------------------

void TControl::StatReset(void)
{
  ProcessL->ResetAbsMax();
  ProcessR->ResetAbsMax();
}

//------------------------------ LEDs on/off: --------------------------------

void TControl::SetLeds(uint16_t n)
{
  LedsSet = n;
  Display->Leds(LedsSet);
  Display->Update();
}

//------------------------------ Read EEPROM: --------------------------------

void TControl::EERead(void)
{
  TProcess::EERead(Preset);
  Bright->EERead(Preset);
  for(uint8_t i = 0; i < SEGS; i++)
    dBTable[i] = Eeprom->Read(EE_DB_TAB + i, DBTABLE[i]);
  ApplyTab();
  SetDispMode(Eeprom->Read(EE_DMODE, DMD_P[Preset]));
#ifdef USE_LEDS_MUX
  if(!MuxEn)
#endif
  SetLeds(Eeprom->Read(EE_LEDS, LEDS_P[Preset]));
}

//----------------------------- Write EEPROM: --------------------------------

bool TControl::EESave(void)
{
  Eeprom->Error = 0;
  TProcess::EESave();
  Bright->EESave();
  for(uint8_t i = 0; i < SEGS; i++)
    Eeprom->Write(EE_DB_TAB + i, dBTable[i]);
  Eeprom->Write(EE_DMODE, DispMode);
  Eeprom->Write(EE_LEDS, LedsSet);
  Eeprom->Validate();
  return(Eeprom->Error);
}

//---------------------------- Defaults load: --------------------------------

void TControl::LoadDefaults(void)
{
  Eeprom->Invalidate();
  EERead();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
