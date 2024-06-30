//----------------------------------------------------------------------------

//Main module, header file

//----------------------------------------------------------------------------

#ifndef MAIN_HPP
#define MAIN_HPP

//----------------------------------------------------------------------------

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "stm32f10x.h"
#include "gpio.hpp"
#include "systimer.hpp"

//-------------------------- Firmware Version: -------------------------------

#define VERSION "2.20"

//------------------------ Compilation Options: ------------------------------

//#define USE_KEYBOARD //use preset select buttons instead of jumpers
//#define USE_LEDS_MUX //use mux for LEDs conrol

//----------------------------------------------------------------------------

extern uint8_t *Addr; //TODO: memory profiling

//-------------------------- Processor Pin Usage: ----------------------------

//Pin1 (VBAT) +3.3V
//Pin2 (PC13) 50, 35 - not used, 40 - BKL [display]
typedef TGpio<PORTC, PIN13> Pin_BKL_t;
//Pin3 (PC14) MD2 - STAT button [keyboard]
typedef TGpio<PORTC, PIN14> Pin_STAT_t;
//Pin4 (PC15) MD1 - MODE button [keyboard]
typedef TGpio<PORTC, PIN15> Pin_MODE_t;
//Pin5 (PD0/OSC_IN) OSC_IN
//Pin6 (PD1/OSC_OUT) OSC_OUT
//Pin7 (NRST) RESET
//Pin8 (VSSA) GND
//Pin9 (VDDA) +3.3VA
//Pin10 (PA0/ADC0) - ADCL [adc]
typedef TGpio<PORTA, PIN0> Pin_ADCL_t;
//Pin11 (PA1/ADC1) - ADCR [adc]
typedef TGpio<PORTA, PIN1> Pin_ADCR_t;
//Pin12 (PA2/USART2 TXD) - TXD2 [link]
typedef TGpio<PORTA, PIN2> Pin_TXD2_t;
//Pin13 (PA3/USART2 RXD) - RXD2 [link]
typedef TGpio<PORTA, PIN3> Pin_RXD2_t;
//Pin14 (PA4/DAC1) - DAC1 [control]
typedef TGpio<PORTA, PIN4> Pin_DAC1_t;
//Pin15 (PA5/DAC2) - DAC2 [control]
typedef TGpio<PORTA, PIN5> Pin_DAC2_t;
//Pin16 (PA6) - DIR [link]
typedef TGpio<PORTA, PIN6> Pin_DIR_t;
//Pin17 (PA7) not used
//Pin18 (PB0) not used
//Pin19 (PB1) not used
//Pin20 (PB2/BOOT1) GND
//Pin21 (PB10) not used
//Pin22 (PB11) not used
//Pin23 (VSS) GND
//Pin24 (VDD) +3.3V
//Pin25 (PB12) not used
//Pin26 (PB13) not used
//Pin27 (PB14/TIM15 CH1) - PWM2 [control]
typedef TGpio<PORTB, PIN14> Pin_PWM2_t;
//Pin28 (PB15/TIM15 CH2) - PWM1 [control]
typedef TGpio<PORTB, PIN15> Pin_PWM1_t;
//Pin29 (PA8) not used
//Pin30 (PA9/USART1 TXD) - TXD1 [port]
typedef TGpio<PORTA, PIN9> Pin_TXD1_t;
//Pin31 (PA10/USART1 RXD) - RXD1 [port]
typedef TGpio<PORTA, PIN10> Pin_RXD1_t;
//Pin32 (PA11) not used
//Pin33 (PA12) not used
//Pin34 (PA13/SWDIO) SWDIO
//Pin35 (VSS) GND
//Pin36 (VDD) +3.3V
//Pin37 (PA14/SWCL) SWCL
//Pin38 (PA15) not used
//Pin39 (PB3/SPI1 SCK) - SCLK [sreg]
typedef TGpio<PORTB, PIN3> Pin_SCLK_t;
//Pin40 (PB4) not used
//Pin41 (PB5/SPI1 MOSI) - SDATA [sreg]
typedef TGpio<PORTB, PIN5> Pin_SDATA_t;
//Pin42 (PB6) - LOAD [sreg]
typedef TGpio<PORTB, PIN6> Pin_LOAD_t;
//Pin43 (PB7) - OE [sreg]
typedef TGpio<PORTB, PIN7> Pin_OE_t;
//Pin44 (BOOT0) BOOT0
//Pin45 (PB8) - SCL [eeprom]
typedef TGpio<PORTB, PIN8> Pin_SCL_t;
//Pin46 (PB9) - SDA [eeprom]
typedef TGpio<PORTB, PIN9> Pin_SDA_t;
//Pin47 (VSS) GND
//Pin48 (VDD) +3.3V

//----------------------- Using Processor Peripherals: -----------------------

enum adcin_t { ADCL = PIN0, ADCR = PIN1 }; //ADC inputs numbers
//ADC0 + DMA CH5 + TIM2: LEFT input
//ADC1 + DMA CH7 + TIM2: RIGHT input
//DAC1: optional backlight brightness
//DAC2: optional bars brightness
//TIM15 CH1: PWM2 optional backlight brightness
//TIM15 CH2: PWM1 optional bars brightness
//TIM16: I2C timing
//SPI1: display registers load
//USART1: PC link PORT (and FW download)
//USART2: LINK RS-485

//-------------------------------- Macros: -----------------------------------

#define LO(x)    ((uint8_t)((x) & 0xFF))
#define HI(x)    ((uint8_t)(((x) >> 8) & 0xFF))
#define LO_W(x)  ((uint16_t)((x) & 0xFFFF))
#define HI_W(x)  ((uint16_t)(((x) >> 16) & 0xFFFF))
#define SHL16(x) ((uint32_t)(x) << 16)
#define BYTE1(x) (LO(x))
#define BYTE2(x) (HI(x))
#define BYTE3(x) ((uint8_t)(((uint32_t)(x) >> 16) & 0xFF))
#define BYTE4(x) ((uint8_t)(((uint32_t)(x) >> 24) & 0xFF))
#define BYTE5(x) ((uint8_t)(((uint64_t)(x) >> 32) & 0xFF))
#define BYTE6(x) ((uint8_t)(((uint64_t)(x) >> 40) & 0xFF))
#define BYTE7(x) ((uint8_t)(((uint64_t)(x) >> 48) & 0xFF))
#define BYTE8(x) ((uint8_t)(((uint64_t)(x) >> 56) & 0xFF))

#define WORD(b1,b0)        (((uint16_t)(b1) << 8) | (b0))
#define DWORD(b3,b2,b1,b0) (((uint32_t)WORD(b3,b2) << 16) | WORD(b1,b0))

#define ABS(x) ((x < 0)? -x : x)

#ifndef NULL
  #define NULL _NULL
#endif

//----------------------------------------------------------------------------

#endif
