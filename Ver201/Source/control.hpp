//----------------------------------------------------------------------------

//����� ������ ����������, ������������ ����

//----------------------------------------------------------------------------

#ifndef CONTROL_HPP
#define CONTROL_HPP

//----------------------------------------------------------------------------

#include "main.hpp"
#include "display.hpp"
#include "process.hpp"

//----------------------------------------------------------------------------
//------------------------------- ���������: ---------------------------------
//----------------------------------------------------------------------------

//������ ����������:
enum DispMode_t
{
  DP_BAR     = 0x01, //��������� ���������
  DP_PEAK    = 0x02, //��������� �����
  DP_BARMAX  = 0x04, //��������� ���������� ���������
  DP_PEAKMAX = 0x08, //��������� ���������� �����
  DP_ABSMAX  = 0x10  //��������� ����������
};

//������ ��������:
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
//---------------------------- ����� TControl: -------------------------------
//----------------------------------------------------------------------------

class TControl
{
private:
  TDisplay *Display;
  TProcess *ProcessL;
  TProcess *ProcessR;
  Pin_MD1_t Pin_MD1;
  Pin_MD2_t Pin_MD2;
  static const uint16_t TEST_ON = 300;    //������������ ����� ���������, ��
  static const uint16_t TEST_FALL = 1000; //������������ ����� ������, ��
  uint8_t Jumpers;           //������� ��������� ���������
  uint8_t Preset;            //������� ����� �������
  uint8_t DispMode;          //������� ����� �����������
  bool UpdL, UpdR;           //����� ���������� ������� L � R
  uint16_t BarL;             //������� ���������� ��������...
  uint16_t BarR;
  uint16_t PeakL;
  uint16_t PeakR;
  uint16_t PeakMaxL;
  uint16_t PeakMaxR;
  uint16_t BarMaxL;
  uint16_t BarMaxR;
  uint8_t PeakSegL;          //������ ��������� ��� ������� ��������...
  uint8_t PeakSegR;
  uint8_t PeakMaxSegL;
  uint8_t PeakMaxSegR;
  uint8_t BarMaxSegL;
  uint8_t BarMaxSegR;
  int16_t dBTable[SEGS];     //������� dB
  uint16_t dBcTable[SEGS];   //������� dB codes
  uint16_t LedsSet;          //������� ����� ������� �������������
  void InitTest(void);       //��������� ������������ �������
  void PresetsService(void); //������ ������������ ��������
  void ReadValues(void);     //������ ���������� ��������
  void DisplayValues(void);  //����� ���������� ��������
  uint16_t dB2Code(uint16_t dB); //������� dB � ���
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
  void EESave(void);
  void LoadDefaults(void);
};

//----------------------------------------------------------------------------

extern TControl *Control;

//----------------------------------------------------------------------------

#endif
