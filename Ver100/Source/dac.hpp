//----------------------------------------------------------------------------

//Реализация шаблонного класса TDac

//----------------------------------------------------------------------------

#ifndef DAC_HPP
#define DAC_HPP

//----------------------------- Константы: -----------------------------------

enum dacs_t { DAC1, DAC2 }; //коды каналов DAC
#define DAC_RES      12 //DAC resolution, bits
#define DAC_MAX_CODE ((1 << DAC_RES) - 1) //DAC max code
#define DAC_BUFFER   1  //DAC buffer on/off

//----------------------------------------------------------------------------
//-------------------------- Шаблонный класс TDac: ---------------------------
//----------------------------------------------------------------------------

template<dacs_t DacN>
class TDac
{
private:
  TGpio<PORTA, (DacN == DAC1)? PIN4 : PIN5> Pin_DAC;
public:
  TDac(void) {};
  void Init(void);
  void operator = (uint16_t Value);
};

//---------------------------- Инициализация: --------------------------------

template<dacs_t DacN>
void TDac<DacN>::Init(void)
{
  Pin_DAC.Init(IN_ANALOG);            //настройка порта DAC
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;  //включение тактирования DAC

  if(DacN == DAC1) //сравнение констант, оптимизатор уберет ненужное
  {
    DAC->CR |=
      DAC_CR_DMAEN1    * 0 | //DMA disable
      DAC_CR_MAMP1_0   * 0 | //mask/amplitude select
      DAC_CR_WAVE1_0   * 0 | //wave generation disabled
      DAC_CR_TSEL1_0   * 0 | //trigger select
      DAC_CR_TEN1      * 0 | //trigger disable
      DAC_CR_BOFF1     * DAC_BUFFER | //buffer on/off
      DAC_CR_EN1       * 1;  //DAC1 enable
  }
  if(DacN == DAC2) //сравнение констант, оптимизатор уберет ненужное
  {
    DAC->CR |=
      DAC_CR_DMAEN2    * 0 | //DMA disable
      DAC_CR_MAMP2_0   * 0 | //mask/amplitude select
      DAC_CR_WAVE2_0   * 0 | //wave generation disabled
      DAC_CR_TSEL2_0   * 0 | //trigger select
      DAC_CR_TEN2      * 0 | //trigger disable
      DAC_CR_BOFF2     * DAC_BUFFER | //buffer on/off
      DAC_CR_EN2       * 1;  //DAC2 enable
  }
}

//------------------------------ Загрузка: -----------------------------------

template<dacs_t DacN>
void TDac<DacN>::operator = (uint16_t Value)
{
  //ограничение диапазона:
  if(Value > DAC_MAX_CODE) Value = DAC_MAX_CODE;
  //загрузка:
  if(DacN == DAC1)
  {
    DAC->DHR12R1 = Value;
  }
  if(DacN == DAC2)
  {
    DAC->DHR12R2 = Value;
  }
}

//----------------------------------------------------------------------------

#endif
