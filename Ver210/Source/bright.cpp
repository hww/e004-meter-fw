//----------------------------------------------------------------------------

//Класс управления яркостью

//----------------------- Используемые ресурсы: ------------------------------

//Регулировка яркости может осуществляться тремя способами:

//1. Регулировка с помощью подстроечных резисторов (BrMode = BRM_VR).
//На вход регуляторов поступают уровни с выводов DAC1 и DAC2, а выход
//подключен к выводам PWM1 и PWM2, откуда он поступает на фильтры
//регулируемых стабилизаторов. Поэтому на выводы DAC1 и DAC2 необходимо
//вывести логическую единицу, а выводы PWM1 и PWM2 перевести в Z-состояние.
//Яркость линеек регулируется с помощью резистора R180.
//Яркость подсветки шкалы регулируется с помощью резистора R181.

//2. Регулировка с помощью ЦАП (BrMode = BRM_DAC). С выходов DAC1 и DAC2
//сигнал через перемычки, установленные вместо подстроечных резисторов,
//поступает на выводы PWM1 и PWM2, а дальше на фильтры регулируемых
//стабилизаторов. Выводы PWM1 и PWM2 требуется перевести в Z-состояние.
//Яркость линеек регулируется с помощью DAC2 (вывод PA5).
//Яркость подсветки шкалы регулируется с помощью DAC1 (вывод PA4).

//3. Регулировка с помощью PWM (BrMode = BRM_PWM). В этом режиме подстроечные
//резисторы не устанавливаются. С выходов PWM1 и PWM2 сигнал поступает на
//фильтры регулируемых стабилизаторов.
//Для TIM15 используется remap выводов.
//Яркость линеек регулируется с помощью PWM1 (вывод PB15, TIM15 CH2).
//Яркость подсветки шкалы регулируется с помощью PWM2 (вывод PB14, TIM15 CH1).

//----------------------------------------------------------------------------

#include "main.hpp"
#include "bright.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//----------------------------------------------------------------------------
//------------------------------ Класс TFan: ---------------------------------
//----------------------------------------------------------------------------

//----------------------------- Конструктор: ---------------------------------

TBright::TBright(void)
{
  BrMeter = 0;
  BrScale = 0;
  SetMode(BRM_OFF);
}

//----------------------------------------------------------------------------
//------------------------ Вспомогательные методы: ---------------------------
//----------------------------------------------------------------------------

//--------------------- Регулировка яркости выключена: -----------------------

void TBright::OffMode(void)
{
#ifdef STM32F10X_MD_VL
  Dac1.Off();                            //отключение DAC1
  Dac2.Off();                            //отключение DAC2
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_LO);      //отключение питания R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_LO);      //отключение питания R180
#endif
  PwmOff();                              //отключение PWM
  Pin_PWM1.Init(OUT_PP_2M, OUT_LO);      //отключение питания R181
  Pin_PWM2.Init(OUT_PP_2M, OUT_LO);      //отключение питания R180
}

//-------------- Регулировка яркости переменными резисторами: ----------------

void TBright::VrMode(void)
{
#ifdef STM32F10X_MD_VL
  Dac1.Off();                            //отключение DAC1
  Dac2.Off();                            //отключение DAC2
#endif
  PwmOff();                              //отключение PWM
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //включение питания R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //включение питания R180
}

//------------------ Регулировка яркости с помощью ЦАП: ----------------------

void TBright::DacMode(void)
{
  PwmOff();                              //отключение PWM
#ifdef STM32F10X_MD_VL
  Dac1.Init();                           //настройка DAC1
  Dac2.Init();                           //настройка DAC2
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //включение питания R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //включение питания R180
#endif
  SetMeter(BrMeter);                     //установка яркости линеек
  SetScale(BrScale);                     //установка яркости транспарантов
}

//------------------ Регулировка яркости с помощью ШИМ: ----------------------

void TBright::PwmMode(void)
{
#ifdef STM32F10X_MD_VL
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;   //включение тактирования TIM15
  TIM15->PSC = 0;                        //прескалер
  TIM15->ARR = PWM_MAX;                  //период
  TIM15->CCR1 = 0;                       //код PWM
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
  TIM15->CR1 = TIM_CR1_CEN;              //разрешение таймера

  AFIO->MAPR2 |= AFIO_MAPR2_TIM15_REMAP; //remap TIM15
  Pin_PWM1.Init(AF_PP_10M);              //настройка пина PWM1
  Pin_PWM2.Init(AF_PP_10M);              //настройка пина PWM2
#else
  Pin_DAC1.Init(OUT_PP_2M, OUT_HI);      //включение питания R181
  Pin_DAC2.Init(OUT_PP_2M, OUT_HI);      //включение питания R180
#endif

  SetMeter(BrMeter);                     //установка яркости линеек
  SetScale(BrScale);                     //установка яркости транспарантов
}

//--------------------------- Выключение ШИМ: --------------------------------

void TBright::PwmOff(void)
{
#ifdef STM32F10X_MD_VL
  TIM15->CCER = 0;                        //OC1 and OC2 disable
  TIM15->BDTR = 0;                        //main output disable
  TIM15->CR1 = 0;                         //запрещение таймера
  RCC->APB2ENR &= ~RCC_APB2ENR_TIM15EN;   //выключение тактирования TIM15
#endif
  Pin_PWM1.Init(IN_ANALOG);               //настройка пина PWM1
  Pin_PWM2.Init(IN_ANALOG);               //настройка пина PWM2
}

//----------------------------------------------------------------------------
//------------------------- Интерфейсные методы: -----------------------------
//----------------------------------------------------------------------------

//------------------- Выбор способа управления яркостью: ---------------------

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

//------------------------ Установка яркости линеек: -------------------------

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

//------------------- Установка яркости подсветки шкалы: ---------------------

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

//--------------------- Чтение параметров из EEPROM: -------------------------

void TBright::EERead(uint8_t p)
{
  SetMode(Eeprom->Read(EE_BRMODE, BMD_P[p]));
  SetMeter(Eeprom->Read(EE_MBRGHT, BRM_P[p]));
  SetScale(Eeprom->Read(EE_SBRGHT, BRS_P[p]));
}

//------------------- Сохранение параметров в EEPROM: ------------------------

void TBright::EESave(void)
{
  Eeprom->Write(EE_BRMODE, BrMode);
  Eeprom->Write(EE_MBRGHT, BrMeter);
  Eeprom->Write(EE_SBRGHT, BrScale);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
