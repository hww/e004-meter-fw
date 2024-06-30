//----------------------------------------------------------------------------

//Класс работы с АЦП

//----------------------- Используемые ресурсы: ------------------------------

//Класс TAdc реализует считывание данных с АЦП и сохранение их в буфер.
//Всего могут быть созданы 2 экземпляра этого класса - для левого и правого
//каналов. В качестве параметров конструктору класса передается номер канала,
//номер входа АЦП и указатель на буфер. Размер половины буфера задан константой
//ADC_DCM, частота дискретизации - константой ADC_FS. АЦП используется в режиме
//преобразования инжектированной группы каналов, состоящей из 2-х каналов.
//Запуск преобразования группы осуществляется событием TIM2 TRGO.
//Результаты преобразования сохраняются в буфере с помощью DMA.
//Для канала 0 (LEFT) используется канал DMA 5, для канала 1 (RIGHT) -
//канал DMA 7. DMA работает в циклическом режиме, данные считываются
//по половинкам буфера.
//Флаг готовности первой половины буфера - DMA_ISR_HTIF5 или DMA_ISR_HTIF7.
//Флаг готовности всего объема буфера - DMA_ISR_TCIF5 или DMA_ISR_TCIF7.
//Для проверки готовности данных служит метод State().
//Если данные не готовы, метод State() возвращает значение -1.
//Если данные готовы, метод State() возвращает смещение 0 для первой
//половины буфера, или смещение ADC_DCM для второй половины буфера.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "adc.hpp"

//----------------------------------------------------------------------------
//------------------------------ Класс TAdc: ---------------------------------
//----------------------------------------------------------------------------

//---------------------------- Инициализация: --------------------------------

TAdc::TAdc(chan_t chan, uint16_t *buff)
{
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4;  //прескалер АЦП (APB2 / 4)
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //включение тактирования ADC

  if(chan == CHANL)
  {
    TGpio<PORTA, ADCL> Pin_ADC;
    Pin_ADC.Init(IN_ANALOG);          //настройка порта ADC
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 * SMP13T5 << ADCL * 3; //sample time
    ADC1->JSQR |=
      ADC_JSQR_JL_0   * 1 |           //2 conversions in inj. group
      ADC_JSQR_JSQ3_0 * ADCL;         //inj. channel select
    ADC1->JOFR1 = (ADC_MAX_CODE + 1) / 2;  //inj. channel offset
  }
  if(chan == CHANR)
  {
    TGpio<PORTA, ADCR> Pin_ADC;
    Pin_ADC.Init(IN_ANALOG);          //настройка порта ADC
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 * SMP13T5 << ADCR * 3; //sample time
    ADC1->JSQR |=
      ADC_JSQR_JL_0   * 1 |           //2 conversions in inj. group
      ADC_JSQR_JSQ4_0 * ADCR;         //inj. channel select
    ADC1->JOFR2 = (ADC_MAX_CODE + 1) / 2;  //inj. channel offset
  }

  ADC1->CR1 =
    ADC_CR1_AWDEN      * 0 |          //reg. analog watchdog disable
    ADC_CR1_JAWDEN     * 0 |          //inj. analog watchdog disable
    ADC_CR1_DISCNUM_0  * 0 |          //no disc. mode channels
    ADC_CR1_JDISCEN    * 0 |          //inj. disc. mode disable
    ADC_CR1_DISCEN     * 0 |          //reg. disc. mode disable
    ADC_CR1_JAUTO      * 0 |          //auto injected group conversion disable
    ADC_CR1_AWDSGL     * 0 |          //watchdog single scan channel disable
    ADC_CR1_SCAN       * 1 |          //scan mode
    ADC_CR1_JEOCIE     * 0 |          //inj. channels interrupt disable
    ADC_CR1_AWDIE      * 0 |          //analog watchdog interrupt disable
    ADC_CR1_EOCIE      * 0 |          //EOC interrupt disable
    ADC_CR1_AWDCH_0    * 0;           //analog watchdog channel

  ADC1->CR2 =
    ADC_CR2_TSVREFE    * 0 |          //temp. sensor disable
    ADC_CR2_SWSTART    * 0 |          //reg. start conversion
    ADC_CR2_JSWSTART   * 0 |          //inj. start conversion
    ADC_CR2_EXTTRIG    * 0 |          //reg. ext. start
    ADC_CR2_EXTSEL_0   * 0 |          //reg. ext. event select
    ADC_CR2_JEXTTRIG   * 1 |          //inj. ext. start
    ADC_CR2_JEXTSEL_0  * 2 |          //inj. ext. event select - TIM2 TRGO
    ADC_CR2_ALIGN      * 0 |          //right data alignment
    ADC_CR2_DMA        * 0 |          //DMA disable
    ADC_CR2_RSTCAL     * 0 |          //calibration reset
    ADC_CR2_CAL        * 0 |          //calibration
    ADC_CR2_CONT       * 0 |          //continuous conversion
    ADC_CR2_ADON       * 1;           //ADC enable

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //включение тактирования TIM2
  TIM2->CR1 &= ~TIM_CR1_CEN;          //запрещение таймера
  TIM2->PSC = 0;                      //загрузка прескалера
  TIM2->ARR = (SYSTEM_CORE_CLOCK / ADC_FS) - 1; //период таймера

  TIM2->CR2 =
    TIM_CR2_TI1S       * 0 |
    TIM_CR2_MMS_0      * 2 |          //Update -> TRGO
    TIM_CR2_CCDS       * 0;           //DMA request when CC event

  ADC1->CR2 |= ADC_CR2_RSTCAL;        //ADC calibration reset
  while(ADC1->CR2 & ADC_CR2_RSTCAL);
  ADC1->CR2 |= ADC_CR2_CAL;           //ADC calibration
  while(ADC1->CR2 & ADC_CR2_CAL);

  if(chan == CHANL)
  {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CPAR = (uint32_t)&ADC1->JDR1; //periph. address
    DMA1_Channel5->CMAR = (uint32_t)buff;        //memory address
    DMA1_Channel5->CNDTR = ADC_DCM * 2;          //buffer size

    DMA1_Channel5->CCR =
      DMA_CCR5_MEM2MEM * 0 |          //memory to memory off
      DMA_CCR5_PL_0    * 2 |          //high priority
      DMA_CCR5_MSIZE_0 * 1 |          //mem. size 16 bit
      DMA_CCR5_PSIZE_0 * 1 |          //periph. size 16 bit
      DMA_CCR5_MINC    * 1 |          //memory increment enable
      DMA_CCR5_PINC    * 0 |          //periph. increment disable
      DMA_CCR5_CIRC    * 1 |          //circular mode enable
      DMA_CCR5_DIR     * 0 |          //direction - from periph.
      DMA_CCR5_TEIE    * 0 |          //transfer error interrupt disable
      DMA_CCR5_HTIE    * 0 |          //half transfer interrupt disable
      DMA_CCR5_TCIE    * 0 |          //transfer complete interrupt disable
      DMA_CCR5_EN      * 1;           //DMA enable

    TIM2->CCR1 = TIM2->ARR / 2;       //CC1 register load
    TIM2->DIER |= TIM_DIER_CC1DE;     //CC1 DMA request enable
  }
  if(chan == CHANR)
  {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel7->CPAR = (uint32_t)&ADC1->JDR2; //periph. address
    DMA1_Channel7->CMAR = (uint32_t)buff;        //memory address
    DMA1_Channel7->CNDTR = ADC_DCM * 2;          //buffer size

    DMA1_Channel7->CCR =
      DMA_CCR7_MEM2MEM * 0 |          //memory to memory off
      DMA_CCR7_PL_0    * 2 |          //high priority
      DMA_CCR7_MSIZE_0 * 1 |          //mem. size 16 bit
      DMA_CCR7_PSIZE_0 * 1 |          //periph. size 16 bit
      DMA_CCR7_MINC    * 1 |          //memory increment enable
      DMA_CCR7_PINC    * 0 |          //periph. increment disable
      DMA_CCR7_CIRC    * 1 |          //circular mode enable
      DMA_CCR7_DIR     * 0 |          //direction - from periph.
      DMA_CCR7_TEIE    * 0 |          //transfer error interrupt disable
      DMA_CCR7_HTIE    * 0 |          //half transfer interrupt disable
      DMA_CCR7_TCIE    * 0 |          //transfer complete interrupt disable
      DMA_CCR7_EN      * 1;           //DMA enable

    TIM2->CCR2 = TIM2->ARR;           //CC2 register load
    TIM2->DIER |= TIM_DIER_CC2DE;     //CC2 DMA request enable
  }
  TIM2->CR1 |= TIM_CR1_CEN;           //timer 2 enable
  Chan = chan;
}

//-------------------------- Чтение состояния: -------------------------------

int16_t TAdc::State(void)
{
  if(Chan == CHANL)
  {
    if(DMA1->ISR & DMA_ISR_HTIF5)     //check half transfer flag
    {
      DMA1->IFCR = DMA_IFCR_CHTIF5;   //flag clear
      return(0);                      //buffer offset
    }
    if(DMA1->ISR & DMA_ISR_TCIF5)     //check transfer complete flag
    {
      DMA1->IFCR = DMA_IFCR_CTCIF5;   //flag clear
      return(ADC_DCM);                //buffer offset
    }
    return(-1);                       //not ready
  }
  if(Chan == CHANR)
  {
    if(DMA1->ISR & DMA_ISR_HTIF7)     //check half transfer flag
    {
      DMA1->IFCR = DMA_IFCR_CHTIF7;   //flag clear
      return(0);                      //buffer offset
    }
    if(DMA1->ISR & DMA_ISR_TCIF7)     //check transfer complete flag
    {
      DMA1->IFCR = DMA_IFCR_CTCIF7;   //flag clear
      return(ADC_DCM);                //buffer offset
    }
    return(-1);                       //not ready
  }
  return(-1);                         //illegal channel
}

//----------------------------------------------------------------------------
