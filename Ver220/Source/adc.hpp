//----------------------------------------------------------------------------

//ADC service class, header file

//----------------------------------------------------------------------------

#ifndef ADC_HPP
#define ADC_HPP

//----------------------------------------------------------------------------

#include "main.hpp"

//------------------------------- Constants: ---------------------------------

enum chan_t { CHANL, CHANR };         //Channels codes
static const uint32_t ADC_FS = 96000; //Sampling frequency, Hz
static const uint16_t ADC_DCM = 256;  //Decimation ratio (buffer halfsize)
static const uint8_t  ADC_RES = 12;   //ADC resolution, bits
static const uint16_t ADC_MAX_CODE = (1 << ADC_RES) - 1; //ADC max code

//----------------------------------------------------------------------------
//------------------------------ TAdc class: ---------------------------------
//----------------------------------------------------------------------------

//chan = 0, 1 - input channel number (LEFT, RIGHT)
//adcin = 0..9 - ADC channel number

class TAdc
{
private:
  enum smp_t //ADC sample time codes
  { SMP1T5, SMP7T5, SMP13T5, SMP28T5, SMP41T5, SMP55T5, SMP71T5, SMP239T5 };
  chan_t Chan;
public:
  TAdc(chan_t chan, uint16_t *buff);
  int16_t State(void);
};

//----------------------------------------------------------------------------

#endif
