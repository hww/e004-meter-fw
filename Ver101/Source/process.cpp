//----------------------------------------------------------------------------

//����� ��������� ������

//----------------------- ������������ �������: ------------------------------

//����� TProcess ��������� ��������� ������ ��� ������ ������ ����������.
//��� ��������� ������ ������������ ����� TAdc. ����� Execute() ������
//���������� � �������� �����. ���� ��� ��������� ������ ������ ��� ������,
//�� ������������ �� ��������� � ����� ���������� 1. ���� ������ �� ������,
//����� ���������� 0. ��������� ������� � ���� ����-�������: �� �������
//������������� 96 ��� (TS_MS domain) � ����� ��������� �� �������
//96000/256 = 375 �� (TD_MS domain).

//��������� ���������� � ������ 96 ��� (TS_MS domain).

//������� ��������� ���������� ������������ �������� �������.
//��� ����� ������������ DC Removal Filter, ��������� �� ���������������
//����������� ���������������� � ����������� � ������� (Leaky Integrator).
//������������ ��������� ������ �������������. ������� 12-��������� ������
//������� DCRF_in ��������� � ������� ���. ���������� ������������ �
//32-��������� ������������ DCRF_acc. ������� ����� ������� ��������
//������������� DCRF_A � ������� ������ 5 ��. ������ ����� ���� � �����
//z = 1 � ����� � ����� z = 0.999673. �������� ������ ������� DCRF_out_1
//���� 12-���������.

//��������� ����� ��������� - �������� ����������������� ��������.
//������������ ���������� ����������� �������� ����� DCRF_out_1.
//�������� ��� ��������� DET_out 11-���������.

//����� ������� ��� IIR-�������, ������� ��������� ����� ���������� ���
//��������� �������� (��� �������������) �������� (������������ ����������),
//� �������� �������� (������������ ��������� ������� ��������� - ������).
//���������� ������������ � 32-��������� ������������� Integ1_acc � Integ2_acc.
//�������� ��� ������� ��������� � ������� ����� ������������.
//������� �������� ������ ��� ���������� �������.
//���������� ������� ������������ �������������� Integ1_a � Integ2_a.
//�������� ������������� ����������� �� ������� Integ_a = 65536/96000/tau.
//����� �������� tau = 0 (True Peak), ���� ������ Integ_a = 65536.
//����� ���������� �������� � ������� ���������� Integ1_t � Integ2_t.
//��� ������ tau � INTEG_TTAU = 4.0 ��� (��-�� ���������� ���������� ������
//-2 �� ��� Sin �������).
//����� ���������� �������� � ��������� 0..2700.0 �� � ������������� 0.1 ��.
//Integ_a = INTEG_K (27306) / Integ_t (0..27000)
//INTEG_K = INTEG_TTAU (4) * TS_MS (1/96) * SCALE16 (65536) * INT_RES (10)

//���������� ��������� ������� � ������ 96000/256 = 375 �� (TD_MS domain).

//������� �������� ��������������� �������, ������� ���������� ��� ������������
//��������� ��������� � ����� �� ������������ ������� ��� ������ ����������
//������� ���������� ��� ��������� ����.
//�������� 11-��������� ��� ���������� �������� ���������� � ������� ������
//������������� Integ1_acc � Integ2_acc. �� ���������� �� ������������
//Scale1_a � Scale2_a, ����� ������� �� 4096. �� ������ ���������� ������������
//��� 10000, ������ ����� �������� ������������ � ������� �������������.
//����� ��������� �� ������������ ��� �������������� �� ������ LEV_LIM,
//������� ��������������� �������� �� 10% ���� ���������� ������������� ����.
//��� ������������ ��������� ������������� Scale1_a � Scale2_a ��������
//����� 32 (�������� +30 ��).
//������������� ����������� �������� ������ ������� ������ �� �����,
//��������� ����� ���� ��� ������������ ���� ��� �� ����� ����������
//������� �������, ����������� ��� ��������� ���� ���������.
//�������� �������� � ������� ���������� Scale1_f � Scale2_f.
//�������� ������� 0..32.000 � ������������� 0.001.
//Scale_a = SCALE_K (81960) * Scale_f (0..32000) / SCALE12 (4096)
//SCALE_K = LEV_MAX (10000) * SCALE12 (4096) / LEV_RES (1000) *
//SCALE12 (4096) / DET_MAX (2047)

//����� ������� ��� IIR-�������, ������� ��������� ����� ������������.
//��� �������� � "���������" ������.
//�������� ��� ���� �������� ��������������� ������ � ����������� ����������.
//���������� ������������ � 32-��������� ������������� Resp1_acc � Resp2_acc.
//�������� ��� ������� ��������� � ������� ����� ������������.
//������� �������� ������ ��� ���������� �������.
//���������� ������� ������������ �������������� Resp1_a � Resp2_a.
//�������� ������������� ����������� �� ������� A = 65536/375/tau.
//����� �������� tau = 0, ���� ������ A = 65536. � ���� ������ �����
//������������ ����� ����� ������� ����������.
//����� ������������ �������� � ������� ���������� Resp1_t � Resp2_t.
//��� ������ tau � RESP_TTAU = 2.3 ��� (��-�� ���������� ���������� ������
//-1 �� ��� DC �������).
//����� ������������ �������� � ��������� 0..65000 �� � ������������� 1 ��.
//Resp_a = RESP_K (401954) / Resp_t (0..65000)
//RESP_K = RESP_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536)

//����� �������� ��� ������� ������������ ��������� �������� ���� � ���������
//1 ��, �� ���������� ������ ��������, �������������� �� ������������
//�������� ���������� Integ1_acc � Integ2_acc. �� �������� � "���������" ������.
//���������� ������� �������� ������������ �������������� Decay1_a � Decay2_a.
//�������� ������������� ����������� �� ������� A = 65536/375/tau.
//����� �������� tau = 0, ���� ������ A = 65536.
//����� �������� �������� � ������� ���������� Decay1_t � Decay2_t.
//��� ������ tau � DECAY_TTAU = 2.3 ��� (��-�� ���������� ���������� ������
//-20 �� ��� DC �������).
//����� �������� �������� � ��������� 0..65000 �� � ������������� 1 ��.
//Decay_a = DECAY_K (401954) / Decay_t (0..65000)
//DECAY_K = DECAY_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536)

//��������� ���������� ����� ����� ��������� ����� ���������, ����� �����
//���� ������� ��������� �������� �����������. ��� ����� �������� � �������
//������������� Hold1_a � Hold2_a.
//�������� ������������� ����������� �� ������� A = 375*t_hold.
//���� ������ ������� �������� �������������, ��������� ��������� �� �����,
//����� ����� ���� ������� ����� ���������� �������� ���.
//����� ��������� �������� � ������� ���������� Hold1_t � Hold2_t.
//��� �������� � ��������� 0..65000 �� � ������������� 1 ��.
//Hold_a = HOLD_K (24576) * Hold_t (0..65000) / SCALE16 (65536)
//HOLD_K  = SCALE16 (65536) / TD_MS (256/96)

//����� ����� ��������� �������������, ���������� �������� ��� ����������.
//������ ��������� ���� ������������ �� ������������ �������� ������������
//Resp1_acc � Resp2_acc. �� �������� � "���������" ������. ���������� �������
//��������� ���� ������������ �������������� Fall1_a � Fall2_a.
//�������� ������������� ����������� �� ������� A = 65536/375/tau.
//����� �������� tau = 0, ���� ������ A = 65536.
//����� ��������� ���� �������� � ������� ���������� Fall1_t � Fall2_t.
//��� ������ tau � FALL_TTAU = 2.3 ��� (��-�� ���������� ���������� ������
//-20 �� ��� DC �������).
//����� ��������� ���� �������� � ��������� 0..65000 �� � ������������� 1 ��.
//Fall_a = FALL_K (401954) / Fall_t (0..65000)
//FALL_K  = FALL_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536);

//��������� ������������ ��������� ���� ��� �������� ������������ ���������
//������������ ����������� ����� �������� ��� �������� ����������,
//��� ��������� ������� ���������� ������� ����� ������ ������� �� ���������
//����������. ��� ���������� ������� ���������� ����� ��������� �����������
//������� ��� ������������� �������, ��� ������� ������������� � �������
//�������������� �������������. � ���������� ������ ����� ��������� ����
//��� �������� ������������� ����������.

//������������ �������� ����������� ������� ����� ������������ � ���������
//����� ������������. ��� ����������� ������������ ������������ �������������
//�������� Max1_acc � Max2_acc. ����� ��������� ���� �������� �������� � �������
//������������� MHold1_a � MHold2_a.
//�������� ������������� ����������� �� ������� A = 375*t_hold.
//���� ������ ������� �������� �������������, ��������� ��������� �� �����,
//����� ����� ���� ������� ����� ���������� �������� ���.
//����� ��������� �������� � ������� ���������� MHold1_t � MHold2_t.
//��� �������� � ��������� 0..65000 �� � ������������� 1 ��.
//MHold_a = HOLD_K (24576) * Hold_t (0..65000) / SCALE16 (65536)
//HOLD_K  = SCALE16 (65536) / TD_MS (256/96)

//����� ����� ��������� �������������, ���������� �������� ��� ������������
//��������. ������ ��������� ���� ������������ �� ������������ Max1_acc �
//Max2_acc. �� �������� � "���������" ������. ���������� �������
//��������� ���� ������������ �������������� MFall1_a � MFall2_a.
//�������� ������������� ����������� �� ������� A = 65536/375/tau.
//����� �������� tau = 0, ���� ������ A = 65536.
//����� ��������� ���� �������� � ������� ���������� MFall1_t � MFall2_t.
//��� ������ tau � FALL_TTAU = 2.3 ��� (��-�� ���������� ���������� ������
//-20 �� ��� DC �������).
//����� ��������� ���� �������� � ��������� 0..65000 �� � ������������� 1 ��.
//MFall_a = FALL_K (401954) / Fall_t (0..65000)
//FALL_K  = FALL_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536);

//����������� � �������������� Max1_acc � Max2_acc ������������ ��������
//����������� ������� ����������� � ��� ����� �������������: AMax1_acc �
//AMax2_acc. ��� ������������ ��� ���������� ����������. ��� ������ ����������
//�������� � ���� ������������� ����� ������ �������������. ��������� ����
//��� ������������ �� �����. � ��� ������������� ������������ ����������
//��������. ����� ������������� AMax1_acc � AMax2_acc ������������ �������
//� ������� ����������� �������.

//��������� ������� ������������ ��������� ��� ����� ����� ����������,
//�� ������������ ��� ��� ����� ���� �������. ���������� � ����� ������.
//���������� �������� ������ ����� (Bar) ������������ ����������, � ����������
//�������� ������ ����� ������������ ��������� ������� ��������� - ������.
//������������ �������� �������� ����� ����� ������������ �������.

//��� ������������ ������������� ���������� �������� tau ���������� 1.25 ��
//(����� ���������� 5 �� �� -2 ��), tau ��������� ���� 739 �� (����� ���������
//��� 1700 �� �� ������ -20 ��), tau ������������ 43..87 �� (100..200 �� ��
//-1 ��), ����� ��������� - 0 ��.

//��� �������� ���������� tau ���������� 0 ��, tau ������������ 0..87 ��
//(0..200 �� �� -1 ��), ����� ��������� ������� 2000 ��, tau ��������� ����
//�� 0 �� (����� �������� �����) �� 739 �� (����� ������ ������� �� 1700 ��
//�� ������ -20 ��).

//----------------------------------------------------------------------------

#include "main.hpp"
#include "process.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//----------------------------------------------------------------------------
//---------------------------- ����� TProcess: -------------------------------
//----------------------------------------------------------------------------

//---------------------- ����������� ����������: -----------------------------

int32_t  TProcess::Integ1_a; uint16_t TProcess::Integ1_t;
int32_t  TProcess::Decay1_a; uint16_t TProcess::Decay1_t;
int32_t  TProcess::Scale1_a; uint16_t TProcess::Scale1_f;
int32_t  TProcess::Resp1_a;  uint16_t TProcess::Resp1_t;
int32_t  TProcess::Fall1_a;  uint16_t TProcess::Fall1_t;
uint16_t TProcess::Hold1_a;  uint16_t TProcess::Hold1_t;
int32_t  TProcess::MFall1_a; uint16_t TProcess::MFall1_t;
uint16_t TProcess::MHold1_a; uint16_t TProcess::MHold1_t;

int32_t  TProcess::Integ2_a; uint16_t TProcess::Integ2_t;
int32_t  TProcess::Decay2_a; uint16_t TProcess::Decay2_t;
int32_t  TProcess::Scale2_a; uint16_t TProcess::Scale2_f;
int32_t  TProcess::Resp2_a;  uint16_t TProcess::Resp2_t;
int32_t  TProcess::Fall2_a;  uint16_t TProcess::Fall2_t;
uint16_t TProcess::Hold2_a;  uint16_t TProcess::Hold2_t;
int32_t  TProcess::MFall2_a; uint16_t TProcess::MFall2_t;
uint16_t TProcess::MHold2_a; uint16_t TProcess::MHold2_t;

//------------------------------ �����������: --------------------------------

TProcess::TProcess(chan_t chan)
{
  Adc = new TAdc(chan, Buff);
  //DC removal filter:
  DCRF_acc = 0;
  DCRF_in_1 = 0;
  DCRF_out_1 = 0;
  ResetFilters();
}

//----------------------------------------------------------------------------
//-------------------------- ���������� ���������: ---------------------------
//----------------------------------------------------------------------------

bool TProcess::Execute(void)
{
  int16_t Offs = Adc->State();
  if(Offs >= 0)
  {

//------------------------------ TS_MS domain: -------------------------------
//----------------------------------------------------------------------------

    uint16_t *Pnt = Buff + Offs;
    for(int i = 0; i < ADC_DCM; i++)
    {

//---------------------------- DC removal filter: ----------------------------

      int16_t DCRF_in = *Pnt++;
      DCRF_acc = LO_W(DCRF_acc) + (int32_t)DCRF_out_1 * DCRF_A;
      DCRF_out_1 = DCRF_in - DCRF_in_1 + HI_W(DCRF_acc);
      DCRF_in_1 = DCRF_in;

//-------------------------------- Detector: ---------------------------------

      DET_out = ABS(DCRF_out_1);

//------------------------ Integration filter (bar): -------------------------

      int16_t Integ1_d = DET_out - HI_W(Integ1_acc);
      if(Integ1_d > 0) Integ1_acc = Integ1_acc + Integ1_d * Integ1_a;

//----------------------- Integration filter (peak): -------------------------

      int16_t Integ2_d = DET_out - HI_W(Integ2_acc);
      if(Integ2_d > 0) Integ2_acc = Integ2_acc + Integ2_d * Integ2_a;
    }

//--------------------------- TD_MS domain (bar): ----------------------------
//----------------------------------------------------------------------------

//-------------------------------- Scaling: ----------------------------------

    int32_t a1 = HI_W(Integ1_acc) * Scale1_a;
    if(a1 > LEV_LIM) a1 = LEV_LIM;
    int16_t Resp1_in = a1 >> 12;

//---------------------------- Response filter: ------------------------------

    int16_t Resp1_d = Resp1_in - HI_W(Resp1_acc);
    if(Resp1_d >= 0) { Resp1_acc = Resp1_acc + Resp1_d * Resp1_a; Hold1_tmr = 0; }

//------------------------------ Decay filter: -------------------------------

    if(HI_W(Resp1_acc) > HI_W(Resp1_in * M1DB))
      Integ1_acc = Integ1_acc - HI_W(Integ1_acc) * Decay1_a;

//------------------------------ Fall filter: --------------------------------

    if(Hold1_tmr < Hold1_a) Hold1_tmr++;
      else Resp1_acc = Resp1_acc - HI_W(Resp1_acc) * Fall1_a;

//------------------------------- Max filter: --------------------------------

    if(MHold1_a)
    {
      if(Max1_acc < Resp1_acc) { Max1_acc = Resp1_acc; MHold1_tmr = 0; }

//----------------------------- Max fall filter: -----------------------------

      if(MHold1_tmr < MHold1_a) MHold1_tmr++;
        else Max1_acc = Max1_acc - HI_W(Max1_acc) * MFall1_a;
    } else Max1_acc = 0;

//----------------------------- Abs max filter: ------------------------------

    if(AMax1_acc < Resp1_acc) AMax1_acc = Resp1_acc;

//----------------------------- TD_MS domain (peak): -------------------------
//----------------------------------------------------------------------------

//-------------------------------- Scaling: ----------------------------------

    int32_t a2 = HI_W(Integ2_acc) * Scale2_a;
    if(a2 > LEV_LIM) a2 = LEV_LIM;
    int16_t Resp2_in = a2 >> 12;

//---------------------------- Response filter: ------------------------------

    int16_t Resp2_d = Resp2_in - HI_W(Resp2_acc);
    if(Resp2_d >= 0) { Resp2_acc = Resp2_acc + Resp2_d * Resp2_a; Hold2_tmr = 0; }

//------------------------------ Decay filter: -------------------------------

    if(HI_W(Resp2_acc) > HI_W(Resp2_in * M1DB))
      Integ2_acc = Integ2_acc - HI_W(Integ2_acc) * Decay2_a;

//------------------------------ Fall filter: --------------------------------

    if(Hold2_tmr < Hold2_a) Hold2_tmr++;
      else Resp2_acc = Resp2_acc - HI_W(Resp2_acc) * Fall2_a;

//------------------------------- Max filter: --------------------------------

    if(MHold2_a)
    {
      if(Max2_acc < Resp2_acc) { Max2_acc = Resp2_acc; MHold2_tmr = 0; }

//----------------------------- Max fall filter: -----------------------------

      if(MHold2_tmr < MHold2_a) MHold2_tmr++;
        else Max2_acc = Max2_acc - HI_W(Max2_acc) * MFall2_a;
    } else Max2_acc = 0;

//----------------------------- Abs max filter: ------------------------------

    if(AMax2_acc < Resp2_acc) AMax2_acc = Resp2_acc;

    return(1);
  }
  return(0);
}

//----------------------------------------------------------------------------
//------------------------- ������������ ������: -----------------------------
//----------------------------------------------------------------------------

//------------------------- ����� ���� ��������: -----------------------------

void TProcess::ResetFilters(void)
{
  //bar filters:
  Integ1_acc = 0;
  Resp1_acc  = 0;
  Hold1_tmr  = 0;
  Max1_acc   = 0;
  MHold1_tmr = 0;
  AMax1_acc  = 0;
  //peak filters:
  Integ2_acc = 0;
  Resp2_acc  = 0;
  Hold2_tmr  = 0;
  Max2_acc   = 0;
  MHold2_tmr = 0;
  AMax2_acc  = 0;
}

//--------------------- ����� ���������� ����������: -------------------------

void TProcess::ResetAbsMax(void)
{
  AMax1_acc = 0;
  AMax2_acc = 0;
}

//------------------ ������� ���������� � ������������: ----------------------

void TProcess::ApplyParams(void)
{
  Integ1_a = Integ1_t? (INTEG_K / Integ1_t) : SCALE16;
  Decay1_a = Decay1_t? (DECAY_K / Decay1_t) : SCALE16;
  Scale1_a = SCALE_K * Scale1_f / SCALE12;
  Resp1_a  = Resp1_t? (RESP_K / Resp1_t) : SCALE16;
  Fall1_a  = Fall1_t? (FALL_K / Fall1_t) : SCALE16;
  Hold1_a  = HOLD_K * Hold1_t / SCALE16;
  MFall1_a = MFall1_t? (FALL_K / MFall1_t) : SCALE16;
  MHold1_a = HOLD_K * MHold1_t / SCALE16;

  Integ2_a = Integ2_t? (INTEG_K / Integ2_t) : SCALE16;
  Decay2_a = Decay2_t? (DECAY_K / Decay2_t) : SCALE16;
  Scale2_a = SCALE_K * Scale2_f / SCALE12;
  Resp2_a  = Resp2_t? (RESP_K / Resp2_t) : SCALE16;
  Fall2_a  = Fall2_t? (FALL_K / Fall2_t) : SCALE16;
  Hold2_a  = HOLD_K * Hold2_t / SCALE16;
  MFall2_a = MFall2_t? (FALL_K / MFall2_t) : SCALE16;
  MHold2_a = HOLD_K * MHold2_t / SCALE16;
}

//--------------------- ������ ���������� �� EEPROM: -------------------------

void TProcess::EERead(uint8_t p)
{
  Integ1_t = Eeprom->Read(EE_INTEG1_T, INTEG1_P[p]);
  Decay1_t = Eeprom->Read(EE_DECAY1_T, DECAY1_P[p]);
  Scale1_f = Eeprom->Read(EE_SCALE1_T, SCALE1_P[p]);
  Resp1_t  = Eeprom->Read(EE_RESP1_T,  RESP1_P[p]);
  Fall1_t  = Eeprom->Read(EE_FALL1_T,  FALL1_P[p]);
  Hold1_t  = Eeprom->Read(EE_HOLD1_T,  HOLD1_P[p]);
  MFall1_t = Eeprom->Read(EE_MFALL1_T, MFALL1_P[p]);
  MHold1_t = Eeprom->Read(EE_MHOLD1_T, MHOLD1_P[p]);

  Integ2_t = Eeprom->Read(EE_INTEG2_T, INTEG2_P[p]);
  Decay2_t = Eeprom->Read(EE_DECAY2_T, DECAY2_P[p]);
  Scale2_f = Eeprom->Read(EE_SCALE2_T, SCALE2_P[p]);
  Resp2_t  = Eeprom->Read(EE_RESP2_T,  RESP2_P[p]);
  Fall2_t  = Eeprom->Read(EE_FALL2_T,  FALL2_P[p]);
  Hold2_t  = Eeprom->Read(EE_HOLD2_T,  HOLD2_P[p]);
  MFall2_t = Eeprom->Read(EE_MFALL2_T, MFALL2_P[p]);
  MHold2_t = Eeprom->Read(EE_MHOLD2_T, MHOLD2_P[p]);

  ApplyParams();
}

//------------------- ���������� ���������� � EEPROM: ------------------------

void TProcess::EESave(void)
{
  Eeprom->Write(EE_INTEG1_T, Integ1_t);
  Eeprom->Write(EE_DECAY1_T, Decay1_t);
  Eeprom->Write(EE_SCALE1_T, Scale1_f);
  Eeprom->Write(EE_RESP1_T,  Resp1_t);
  Eeprom->Write(EE_FALL1_T,  Fall1_t);
  Eeprom->Write(EE_HOLD1_T,  Hold1_t);
  Eeprom->Write(EE_MFALL1_T, MFall1_t);
  Eeprom->Write(EE_MHOLD1_T, MHold1_t);

  Eeprom->Write(EE_INTEG2_T, Integ2_t);
  Eeprom->Write(EE_DECAY2_T, Decay2_t);
  Eeprom->Write(EE_SCALE2_T, Scale2_f);
  Eeprom->Write(EE_RESP2_T,  Resp2_t);
  Eeprom->Write(EE_FALL2_T,  Fall2_t);
  Eeprom->Write(EE_HOLD2_T,  Hold2_t);
  Eeprom->Write(EE_MFALL2_T, MFall2_t);
  Eeprom->Write(EE_MHOLD2_T, MHold2_t);
}

//----------------------------------------------------------------------------
