//----------------------------------------------------------------------------

//����� ��������� LED-�������

//----------------------- ������������ �������: ------------------------------

//������������ ������� �� 7-���������� LED-�����������.
//��������� - �����������.
//������������� ������������ ����������.
//���������� � ���������� ���������� ����� ������� ���������
//�������, ������ � �������� �������������� ����� ����� TSreg.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "display.hpp"

//----------------------------------------------------------------------------
//---------------------------- ����� TDisplay --------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------

TDisplay::TDisplay(void)
{
  Sreg.Init();
  Clear();
  Update();
  Sreg.Enable(1);
}

//----------------------------------------------------------------------------
//------------------------- ������������ ������: -----------------------------
//----------------------------------------------------------------------------

//--------------------------- ������� �������: -------------------------------

void TDisplay::Clear(void)
{
  for(uint8_t i = 0; i < REGS; i++)
    RegData[i] = 0xFF;
  Leds(TRNON);
}

//-------------------------- ���������� �������: -----------------------------

void TDisplay::Update(void)
{
  Sreg.Load(RegData);
}

//-------------------- ��������� �������� ��� ������ L: ----------------------

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

//-------------------- ��������� �������� ��� ������ R: ----------------------

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

//-------------------- ��������� �������� ��� ������ L: ----------------------

void TDisplay::BarL(uint8_t n)
{
  for(uint8_t i = 0; i < SEGS; i++) SegL(i, i <= n);
}

//-------------------- ��������� �������� ��� ������ R: ----------------------

void TDisplay::BarR(uint8_t n)
{
  for(int i = 0; i < SEGS; i++) SegR(i, i <= n);
}

//------------------ ��������� ������������� � ���������: --------------------

void TDisplay::Leds(uint16_t s)
{
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
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
