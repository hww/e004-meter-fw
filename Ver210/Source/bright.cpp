//----------------------------------------------------------------------------

//����� ���������� ��������

//----------------------- ������������ �������: ------------------------------

//����������� ������� ����� �������������� ����� ���������:

//1. ����������� � ������� ������������ ���������� (BrMode = BRM_VR).
//�� ���� ����������� ��������� ������ � ������� DAC1 � DAC2, � �����
//��������� � ������� PWM1 � PWM2, ������ �� ��������� �� �������
//������������ ��������������. ������� �� ������ DAC1 � DAC2 ����������
//������� ���������� �������, � ������ PWM1 � PWM2 ��������� � Z-���������.
//������� ������ ������������ � ������� ��������� R180.
//������� ��������� ����� ������������ � ������� ��������� R181.

//2. ����������� � ������� ��� (BrMode = BRM_DAC). � ������� DAC1 � DAC2
//������ ����� ���������, ������������� ������ ������������ ����������,
//��������� �� ������ PWM1 � PWM2, � ������ �� ������� ������������
//��������������. ������ PWM1 � PWM2 ��������� ��������� � Z-���������.
//������� ������ ������������ � ������� DAC2 (����� PA5).
//������� ��������� ����� ������������ � ������� DAC1 (����� PA4).

//3. ����������� � ������� PWM (BrMode = BRM_PWM). � ���� ������ ������������
//��������� �� ���������������. � ������� PWM1 � PWM2 ������ ��������� ��
//������� ������������ ��������������.
//��� TIM15 ������������ remap �������.
//������� ������ ������������ � ������� PWM1 (����� PB15, TIM15 CH2).
//������� ��������� ����� ������������ � ������� PWM2 (����� PB14, TIM15 CH1).

//----------------------------------------------------------------------------

#include "main.hpp"
#include "bright.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//----------------------------------------------------------------------------
//------------------------------ ����� TFan: ---------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------

TBright::TBright(void)
{
  BrMeter = 0;
  BrScale = 0;
  SetMode(BRM_OFF);
}

//----------------------------------------------------------------------------
//------------------------ ��������������� ������: ---------------------------
//----------------------------------------------------------------------------

//--------------------- ����������� ������� ���������: -----------------------

void TBright::OffMode(void)
{
#ifdef STM32F10X_MD_VL
  Dac1.Off();                            //���������� DAC1
  Dac2.Off();                            //���������� DAC2
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_LO);      //���������� ������� R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_LO);      //���������� ������� R180
#endif
  PwmOff();                              //���������� PWM
  Pin_PWM1.Init(OUT_PP_2M, OUT_LO);      //���������� ������� R181
  Pin_PWM2.Init(OUT_PP_2M, OUT_LO);      //���������� ������� R180
}

//-------------- ����������� ������� ����������� �����������: ----------------

void TBright::VrMode(void)
{
#ifdef STM32F10X_MD_VL
  Dac1.Off();                            //���������� DAC1
  Dac2.Off();                            //���������� DAC2
#endif
  PwmOff();                              //���������� PWM
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //��������� ������� R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //��������� ������� R180
}

//------------------ ����������� ������� � ������� ���: ----------------------

void TBright::DacMode(void)
{
  PwmOff();                              //���������� PWM
#ifdef STM32F10X_MD_VL
  Dac1.Init();                           //��������� DAC1
  Dac2.Init();                           //��������� DAC2
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //��������� ������� R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //��������� ������� R180
#endif
  SetMeter(BrMeter);                     //��������� ������� ������
  SetScale(BrScale);                     //��������� ������� �������������
}

//------------------ ����������� ������� � ������� ���: ----------------------

void TBright::PwmMode(void)
{
#ifdef STM32F10X_MD_VL
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;   //��������� ������������ TIM15
  TIM15->PSC = 0;                        //���������
  TIM15->ARR = PWM_MAX;                  //������
  TIM15->CCR1 = 0;                       //��� PWM
  TIM15->CCMR1 =
    TIM_CCMR1_OC2M_0 * 6 |               //OC2 PWM mode 1
    TIM_CCMR1_OC2PE  * 1 |               //CCR2 preload enable
    TIM_CCMR1_OC1M_0 * 6 |               //OC1 PWM mode 1
    TIM_CCMR1_OC1PE  * 1;                //CCR1 preload enable
  TIM15->CCER =
    TIM_CCER_CC2P    * 0 |               //OC2 high active
    TIM_CCER_CC2E    * 1 |               //OC2 enable
    TIM_CCER_CC1P    * 0 |               //OC1 high active
    TIM_CCER_CC1E    * 1;                //OC1 enable
  TIM15->EGR = TIM_EGR_UG;               //update regs from shadow regs
  TIM15->BDTR = TIM_BDTR_MOE;            //main output enable
  TIM15->CR1 = TIM_CR1_CEN;              //���������� �������

  AFIO->MAPR2 |= AFIO_MAPR2_TIM15_REMAP; //remap TIM15
  Pin_PWM1.Init(AF_PP_10M);              //��������� ���� PWM1
  Pin_PWM2.Init(AF_PP_10M);              //��������� ���� PWM2
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //��������� ������� R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //��������� ������� R180
#endif

  SetMeter(BrMeter);                     //��������� ������� ������
  SetScale(BrScale);                     //��������� ������� �������������
}

//--------------------------- ���������� ���: --------------------------------

void TBright::PwmOff(void)
{
#ifdef STM32F10X_MD_VL
  TIM15->CCER = 0;                        //OC1 and OC2 disable
  TIM15->BDTR = 0;                        //main output disable
  TIM15->CR1 = 0;                         //���������� �������
  RCC->APB2ENR &= ~RCC_APB2ENR_TIM15EN;   //���������� ������������ TIM15
#endif
  Pin_PWM1.Init(IN_ANALOG);               //��������� ���� PWM1
  Pin_PWM2.Init(IN_ANALOG);               //��������� ���� PWM2
}

//----------------------------------------------------------------------------
//------------------------- ������������ ������: -----------------------------
//----------------------------------------------------------------------------

//------------------- ����� ������� ���������� ��������: ---------------------

void TBright::SetMode(uint8_t m)
{
  if(BrMode != m)
  {
    switch(m)
    {
    case BRM_OFF: OffMode(); break;
    case BRM_VR:  VrMode();  break;
    case BRM_DAC: DacMode(); break;
    case BRM_PWM: PwmMode(); break;
    }
    BrMode = m;
  }
}

//------------------------ ��������� ������� ������: -------------------------

void TBright::SetMeter(uint8_t b)
{
  if(b > PER_MAX) b = PER_MAX;
  BrMeter = b;
#ifdef STM32F10X_MD_VL
  uint16_t v = b * PWM_MAX / PER_MAX;
  if(BrMode == BRM_DAC) Dac2 = v << 4;
  if(BrMode == BRM_PWM) TIM15->CCR2 = v;
#endif
}

//------------------- ��������� ������� ��������� �����: ---------------------

void TBright::SetScale(uint8_t b)
{
  if(b > PER_MAX) b = PER_MAX;
  BrScale = b;
#ifdef STM32F10X_MD_VL
  uint16_t v = b * PWM_MAX / PER_MAX;
  if(BrMode == BRM_DAC) Dac1 = v << 4;
  if(BrMode == BRM_PWM) TIM15->CCR1 = v;
#endif
}

//--------------------- ������ ���������� �� EEPROM: -------------------------

void TBright::EERead(uint8_t p)
{
  SetMode(Eeprom->Read(EE_BRMODE, BMD_P[p]));
  SetMeter(Eeprom->Read(EE_MBRGHT, BRM_P[p]));
  SetScale(Eeprom->Read(EE_SBRGHT, BRS_P[p]));
}

//------------------- ���������� ���������� � EEPROM: ------------------------

void TBright::EESave(void)
{
  Eeprom->Write(EE_BRMODE, BrMode);
  Eeprom->Write(EE_MBRGHT, BrMeter);
  Eeprom->Write(EE_SBRGHT, BrScale);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
