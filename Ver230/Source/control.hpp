//----------------------------------------------------------------------------

//Common control class, header file

//----------------------------------------------------------------------------

#ifndef CONTROL_HPP
#define CONTROL_HPP

//----------------------------------------------------------------------------

#include "main.hpp"
#include "display.hpp"
#include "keyboard.hpp"
#include "process.hpp"

//----------------------------------------------------------------------------
//------------------------------- Constants: ---------------------------------
//----------------------------------------------------------------------------

//Display modes:
enum DispMode_t
{
  DP_BAR     = 0x01, //display bars
  DP_PEAK    = 0x02, //display peaks
  DP_BARMAX  = 0x04, //hold bars max
  DP_PEAKMAX = 0x08, //hold peaks max
  DP_ABSMAX  = 0x10  //display statistics
};

//Mux inputs:
enum MuxIns_t
{
  MUX1 = 0x01,
  MUX2 = 0x02,
  MUX3 = 0x04,
  MUX4 = 0x08,
  MUX5 = 0x10,
  MUX6 = 0x20,
  MUX7 = 0x40,
  MUX8 = 0x80
};

//Presets numbers:
enum Presets_t
{
  PRESET1,
  PRESET2,
  PRESET3,
  PRESET4,
  PRESETS
};

//0.01 dB step table (0.00dB to -0.15dB):
static const uint16_t DB_001[16] =
{ 65535, 65460, 65384, 65309, 65234, 65159, 65084, 65009,
  64934, 64859, 64785, 64710, 64636, 64561, 64487, 64413 };

//0.16 dB step table (0.00dB to -2.40dB):
static const uint16_t DB_016[16] =
{ 65535, 64339, 63165, 62012, 60880, 59769, 58678, 57607,
  56555, 55523, 54510, 53515, 52538, 51579, 50638, 49713 };

//2.56 dB step table (0.00dB to -38.40dB):
static const uint16_t DB_256[16] =
{ 65535, 48806, 36347, 27069, 20159, 15013, 11181, 8327,
   6201,  4618,  3439,  2561,  1908,  1421,  1058,  788 };

//40.96dB step table (0.00dB to -40.96dB):
static const uint16_t DB_4096[2] =
{ 65535, 587 };

//----------------------------------------------------------------------------
//---------------------------- TControl class: -------------------------------
//----------------------------------------------------------------------------

class TControl
{
private:
  TDisplay *Display;
  TKeyboard *Keyboard;
  TProcess *ProcessL;
  TProcess *ProcessR;
  static const uint16_t TEST_ON = 300;    //test duration, мс
  static const uint16_t TEST_FALL = 1000; //bars fall duration, мс
  uint8_t Jumpers;           //current jumpers state
  uint8_t Preset;            //current preset number
  uint8_t DispMode;          //current display mode
  bool UpdL, UpdR;           //update flags for channels L and R
  uint16_t BarL;             //current measured values...
  uint16_t BarR;
  uint16_t PeakL;
  uint16_t PeakR;
  uint16_t PeakMaxL;
  uint16_t PeakMaxR;
  uint16_t BarMaxL;
  uint16_t BarMaxR;
  uint8_t PeakSegL;          //segments numbers for peak values...
  uint8_t PeakSegR;
  uint8_t PeakMaxSegL;
  uint8_t PeakMaxSegR;
  uint8_t BarMaxSegL;
  uint8_t BarMaxSegR;
  int16_t dBTable[SEGS];     //dB table
  uint16_t dBcTable[SEGS];   //dB codes table
  uint16_t LedsSet;          //current LEDs set
  void InitTest(void);       //initial display test
  void PresetsService(void); //switch presets service
#ifdef USE_LEDS_MUX
  Pin_RXD2_t Pin_Mux0;
  Pin_DIR_t Pin_Mux1;
  Pin_TXD2_t Pin_Mux2;
  Pin_BKL_t Pin_MuxOut;
  static const uint8_t MUX_INS = 8; //MUX inputs count
  bool MuxRead(void);        //MUX read
  void MuxService(void);     //LEDs control using MUX
  uint8_t MuxData;           //MUX inputs copy
  bool MuxEn;                //MUX enable
  bool Mux(uint8_t mask, uint8_t val); //check MUX
#endif
  void ReadValues(void);     //read measured values
  void DisplayValues(void);  //display measured values
  uint16_t dB2Code(uint16_t dB); //dB to code convertion
public:
  TControl(void);
  void Execute(void);
  void ChangePreset(uint8_t p);
  inline uint8_t GetPreset(void)    { return(Preset); }
  void SetDispMode(uint8_t m);
  inline uint8_t GetDispMode(void)  { return(DispMode); }
  bool Upd;
  inline uint16_t GetBarL(void)     { return(BarL); }
  inline uint16_t GetBarR(void)     { return(BarR); }
  inline uint16_t GetBarMaxL(void)  { return(BarMaxL); }
  inline uint16_t GetBarMaxR(void)  { return(BarMaxR); }
  inline uint16_t GetPeakL(void)    { return(PeakL); }
  inline uint16_t GetPeakR(void)    { return(PeakR); }
  inline uint16_t GetPeakMaxL(void) { return(PeakMaxL); }
  inline uint16_t GetPeakMaxR(void) { return(PeakMaxR); }
  void StatReset(void);
  void SetTab(uint8_t n, int16_t v);
  void ApplyTab(void);
  int16_t GetTab(uint8_t n);
  void SetLeds(uint16_t n);
  inline uint16_t GetLeds(void)     { return(LedsSet); }
  void EERead(void);
  bool EESave(void);
  void LoadDefaults(void);
};

//----------------------------------------------------------------------------

extern TControl *Control;

//----------------------------------------------------------------------------

#endif
