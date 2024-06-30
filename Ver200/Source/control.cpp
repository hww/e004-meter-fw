//----------------------------------------------------------------------------

//����� ������ ����������

//----------------------------------------------------------------------------

#include "control.hpp"
#include "bright.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//------------------------- ������������ �������: ----------------------------

//����� TControl ��������� ����� ���������� ��������� ���������. �� � �����
//�������� ������ Execute ������ TProcess ��� ������� �� �������. ���
//������������ ���������� ��� � �������� ��������� ��������. ������� ��������
//������� ����������� � �������� 375 ��, ����� ��������������� ����� ����������
//UpdL � UpdR. ����� ������ ���� ������� ������, ������������ �� ����������.
//� ����������� �� ������ ����������� DispMode ����������� �� ��� ���� ��������.
//��������, ������� �� ������������, �������� ������� ����. ��������� ��������
//������������ �� �������.
//����������� �������� ������ ������������ ��������. �� ������ �� ����������
//���������. ���� ��� ����������, �������� ������������ �������. ��� ����
//�� EEPROM ����������� ������ ����� ���������� � ����������� �� ��� ������.

//----------------------------- ����������: ----------------------------------

TBright *Bright;
TEeprom *Eeprom;

//----------------------------------------------------------------------------
//---------------------------- ����� TControl: -------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------

TControl::TControl(void)
{
  Pin_MD1.Init(IN_PULL, PULL_UP);
  Pin_MD2.Init(IN_PULL, PULL_UP);
  Eeprom = new TEeprom();
  Display = new TDisplay();
  ProcessL = new TProcess(CHANL);
  ProcessR = new TProcess(CHANR);
  Bright = new TBright();

  Jumpers = PRESETS;
  Preset = PRESETS;
  PresetsService();

  InitTest();

  UpdL = 0;
  UpdR = 0;
  Upd = 0;
}

//------------------------ ���������� ����������: ----------------------------

void TControl::Execute(void)
{
  //�������� ���������� ������ ������ L:
  if(ProcessL->Execute()) UpdL = 1;
  //�������� ���������� ������ ������ R:
  if(ProcessR->Execute()) UpdR = 1;
  //���� ��� ������ ������, �����:
  if(UpdL && UpdR)
  {
    ReadValues();    //������ ���������� ��������
    DisplayValues(); //����� �� �������
    UpdL = 0;        //����� ������ ���������� �������
    UpdR = 0;
    Upd = 1;         //��������� ������ ����� ����������
  }

  //���������� ��������:
  if(TSysTimer::Tick) //������ ��������� ���
  {
    PresetsService(); //������������ ��������
  }
}

//----------------------------------------------------------------------------
//------------------------- ���������� ��������: -----------------------------
//----------------------------------------------------------------------------

//-------------------- ������ ������������ ��������: -------------------------

void TControl::PresetsService(void)
{
  uint8_t j = 0;
  if(!Pin_MD1) j = j | 1;
  if(!Pin_MD2) j = j | 2;
  if(Jumpers != j)
  {
    Jumpers = j;
    ChangePreset(j);
  }
}

//----------------------------------------------------------------------------
//------------------------ ��������������� ������: ---------------------------
//----------------------------------------------------------------------------

//---------------------- ��������� ��������� ������: -------------------------

void TControl::InitTest(void)
{
  Display->BarL(SEGS);
  Display->BarR(SEGS);
  Display->Update();
  Display->Leds(TSALL);
  Display->Update();
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
  Display->Leds(LedsSet);
  Display->Update();
}

//---------------------- ������ ���������� ��������: -------------------------

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

//-------------------------- ����������� ������: -----------------------------

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

//-------------------------- ������� dB � ���: -------------------------------

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

//----------------------------------------------------------------------------
//------------------------- ������������ ������: -----------------------------
//----------------------------------------------------------------------------

//-------------------------- �������� �������: -------------------------------

void TControl::SetTab(uint8_t n, int16_t v)
{
  if(n < SEGS) dBTable[n] = v;
}

//----------------------- ���������� ������� �����: --------------------------

void TControl::ApplyTab(void)
{
  int16_t dB_Max = dBTable[SEGS - 1];
  for(uint8_t i = 0; i < SEGS; i++)
  {
    dBcTable[i] = dB2Code(dB_Max - dBTable[i]);
  }
}

//--------------------------- ������ �������: --------------------------------

int16_t TControl::GetTab(uint8_t n)
{
  return(dBTable[n]);
}

//------------------------- ������������ �������: ----------------------------

void TControl::ChangePreset(uint8_t p)
{
  if(p < PRESETS && p != Preset)
  {
    Preset = p;
    Eeprom->SelectPreset(Preset);
    EERead();
  }
}

//--------------------- ������������ ������ ����������: ----------------------

void TControl::SetDispMode(uint8_t m)
{
  DispMode = m;
  BarL     = 0; BarR     = 0;
  PeakL    = 0; PeakR    = 0;
  BarMaxL  = 0; BarMaxR  = 0;
  PeakMaxL = 0; PeakMaxR = 0;
}

//--------------------------- ����� ����������: ------------------------------

void TControl::StatReset(void)
{
  ProcessL->ResetAbsMax();
  ProcessR->ResetAbsMax();
}

//------------------------- ��������� ������ LEDs: ---------------------------

void TControl::SetLeds(uint16_t n)
{
  LedsSet = n;
  Display->Leds(LedsSet);
  Display->Update();
}

//--------------------- ������ ���������� �� EEPROM: -------------------------

void TControl::EERead(void)
{
  TProcess::EERead(Preset);
  Bright->EERead(Preset);
  for(uint8_t i = 0; i < SEGS; i++)
    dBTable[i] = Eeprom->Read(EE_DB_TAB + i, DBTABLE[i]);
  ApplyTab();
  SetDispMode(Eeprom->Read(EE_DMODE, DMD_P[Preset]));
  SetLeds(Eeprom->Read(EE_LEDS, LEDS_P[Preset]));
}

//------------------- ���������� ���������� � EEPROM: ------------------------

void TControl::EESave(void)
{
  TProcess::EESave();
  Bright->EESave();
  for(uint8_t i = 0; i < SEGS; i++)
    Eeprom->Write(EE_DB_TAB + i, dBTable[i]);
  Eeprom->Write(EE_DMODE, DispMode);
  Eeprom->Write(EE_LEDS, LedsSet);
  Eeprom->Validate();
}

//------------------ �������� ���������� �� ���������: -----------------------

void TControl::LoadDefaults(void)
{
  Eeprom->Invalidate();
  EERead();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
