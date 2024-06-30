//----------------------------------------------------------------------------

//Project:  Audio Level Meter
//Compiler: IAR EWARM 6.50
//Target:   STM32F100C8T6B

//Linker extra options: --basic_heap
//Stack size: 0x400
//Heap size: 0xFFF

//------------------------ Program Description: ------------------------------

//The level meter uses digital signal processing. The audio signal of the left
//and right channels is fed through a differential amplifier to the inputs of
//the built-in ADC of the microcontroller. Diff. amplifiers provide balanced
//inputs and shift the signal level by half the ADC scale. To eliminate the
//need for an anti-alias filter, the sample rate is relatively high (96 kHz).
//It also allows for more accurate peak level measurement. Each of the meter
//channels has two independent processing branches, each of which can measure
//average, quasi-peak or peak level. The measurement result of the first branch
//is displayed in the form of light bars, the second branch - in the form of
//light dots. Each of the branches has the ability to hold the maximum values
//with an indication in the form of dots.

//The readings are output to an LED display assembled from LED bar graph arrays.
//To reduce the noise level, a static indication is used - the LEDs are
//connected to the outputs of the 74HC595 register chain. Separate SMD LEDs are
//used to illuminate the scale. Additionally, there are LEDs that illuminate
//stencils with transparent symbols of operating modes. The brightness of these
//LEDs is adjustable separately.

//Change of all parameters of the level meter is done with the help of PC
//service software via a USB-UART adapter. Meter readings can also be displayed
//on a PC. Additionally, there is a RS-485 port for connecting the meter with
//other units.

//------------------------ Program Structure: --------------------------------
//
//                              [main]
//                                 |
//     |------------|----------|------------------------|
// [systimer]    [gpio]    [control]                  [port]
//                             |                        |
//     |---------|--------|-----------|             [wakeport]
// [process] [display] [bright]   [eeprom]              |
//     |         |        |           |               [wake]
//   [adc]     [sreg]   [dac]    |-------|--------|
//                            [i2csw] [eemap] [presets]

//----------------------------------------------------------------------------

#include "main.hpp"
#include "control.hpp"
#include "port.hpp"

//------------------------------ Variables: ----------------------------------

TControl *Control;
TPort *Port;
uint8_t *Addr; //TODO: memory profiling

//----------------------------------------------------------------------------
//---------------------------- Main Program: ---------------------------------
//----------------------------------------------------------------------------

int main(void)
{
  TSysTimer::Init();        //system timer init
  Control = new TControl(); //creating Control object
  Port = new TPort();       //creating Port object

  do                        //main loop
  {
    TSysTimer::Sync();      //system ticks and main loop sync
    Control->Execute();     //local control execute
    Port->Execute();        //PC control execute
  }
  while(1);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
