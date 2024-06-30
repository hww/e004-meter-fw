//----------------------------------------------------------------------------

//ADC service class

//-------------------------- Resources Used: ---------------------------------

//The TAdc class implements reading data from the ADC and storing it in a buffer.
//In total, 2 instances of this class can be created - for the left and right
//channels. As parameters, the class constructor is passed the channel number,
//the ADC input number, and a pointer to the buffer. The half buffer size is set
//by the ADC_DCM constant, the sample rate is set by the ADC_FS constant. The ADC
//is used in the mode of converting an injected channel group consisting of 2 channels.
//Group conversion is triggered by the TIM2 TRGO event.
//The results of the conversion are stored in a buffer using DMA.
//Channel 0 (LEFT) uses DMA channel 5, channel 1 (RIGHT) uses DMA channel 7.
//DMA operates in a cyclic mode, data is read in halves of the buffer.
//First half buffer ready flag - DMA_ISR_HTIF5 or DMA_ISR_HTIF7.
//Full buffer ready flag - DMA_ISR_TCIF5 or DMA_ISR_TCIF7.
//Data ready is checked using method State().
//If data not ready, method State() returns -1.
//If data ready, method State() returns offset 0 for first buffer half
//or offset ADC_DCM for second buffer half.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "adc.hpp"

//----------------------------------------------------------------------------
//------------------------------ TAdc class: ---------------------------------
//----------------------------------------------------------------------------

//--------------------------------- Init: ------------------------------------

TAdc::TAdc(chan_t chan, uint16_t *buff)
{
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4;  //ADC prescaler (APB2 / 4)
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //ADC clock enable

  if(chan == CHANL)
  {
    TGpio<PORTA, ADCL> Pin_ADC;
    Pin_ADC.Init(IN_ANALOG);          //ADC port setup
    ADC1->SMPR2 |= ADC_SMPR2_SMP0_0 * SMP13T5 << ADCL * 3; //sample time
    ADC1->JSQR |=
      ADC_JSQR_JL_0   * 1 |           //2 conversions in inj. group
      ADC_JSQR_JSQ3_0 * ADCL;         //inj. channel select
    ADC1->JOFR1 = (ADC_MAX_CODE + 1) / 2;  //inj. channel offset
  }
  if(chan == CHANR)
  {
    TGpio<PORTA, ADCR> Pin_ADC;
    Pin_ADC.Init(IN_ANALOG);          //ADC port setup
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

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //TIM2 clock enable
  TIM2->CR1 &= ~TIM_CR1_CEN;          //timer disable
  TIM2->PSC = 0;                      //prescaler load
  TIM2->ARR = (SYSTEM_CORE_CLOCK / ADC_FS) - 1; //timer period

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

//----------------------------- Read state: ----------------------------------

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
