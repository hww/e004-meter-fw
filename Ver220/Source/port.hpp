//----------------------------------------------------------------------------

//Serial port support class, header file

//----------------------------------------------------------------------------

#ifndef PORT_HPP
#define PORT_HPP

//----------------------------------------------------------------------------

#include "wakeport.hpp"

//----------------------------------------------------------------------------
//------------------------------ TPort class ---------------------------------
//----------------------------------------------------------------------------

class TPort
{
private:
  static uint32_t const BAUD_RATE = 115200; //baud rate, bauds
  static uint8_t const FRAME_SIZE = 128; //maximum frame size, bytes
  uint8_t Command;
public:
  TWakePort *WakePort;
  TPort(void);
  void Execute(void);
};

//----------------------------------------------------------------------------
//------------------------ Special command codes: ----------------------------
//----------------------------------------------------------------------------

//TX - data transmitted by the PC
//RX - data received by the PC

enum
{
  CMD_STD_LAST = CMD_GETADDR,

  CMD_SET_PRESET, //preset number set
  //TX: byte Preset
  //RX: NONE
  //Preset = 0..3 - preset number

  CMD_GET_PRESET, //preset number get
  //TX: NONE
  //RX: byte Preset
  //see CMD_SET_PRESET

  CMD_SET_MODE,   //display mode set
  //TX: byte DispMode
  //RX: NONE
  //DispMode.0 - display bars (DP_BAR)
  //DispMode.1 - display peaks (DP_PEAK)
  //DispMode.2 - display bar max (DP_BARMAX)
  //DispMode.3 - display peak max (DP_PEAKMAX)
  //DispMode.4 - display statistics (DP_ABSMAX)

  CMD_GET_MODE,   //display mode get
  //TX: NONE
  //RX: byte DispMode
  //see CMD_SET_MODE

  CMD_GET_METER,  //meter readings get
  //TX: NONE
  //RX: (word BarL, word BarR), (word PeakL, word PeakR),
  //    (word BarMaxL, word BarMaxR), (word GetPeakMaxL, word GetPeakMaxR)
  //The specified word pairs are transmitted if DispMode
  //contains DP_BAR, DP_PEAK, DP_BARMAX, DP_PEAKMAX respectively.
  //Each of the result words is a measured value code 0..LEVEL_LIM

  CMD_RES_STAT,   //statistics reset
  //TX: NONE
  //RX: NONE
  //The command resets the accumulator of absolute maximums.

  CMD_SET_PARAMS, //parameters set
  //TX: word Integ1_t, word Decay1_t, word Scale1_f, word Resp1_t,
  //    word Fall1_t, word Hold1_t, word MFall1_t, word MHold1_t,
  //    word Integ2_t, word Decay2_t, word Scale2_f, word Resp2_t,
  //    word Fall2_t, word Hold2_t, word MFall2_t, word MHold2_t
  //RX: NONE
  //Integ1_t = 0..27000 *0.1 ms (0.0..2700.0 ms) - integration time (Bar)
  //Decay1_t = 0..65000 ms - decay time (Bar)
  //Scale1_f = 0..32000 *0.001 (0.000..32.000) - scaling (Bar)
  //Resp1_t  = 0..65000 ms - response time (Bar)
  //Fall1_t  = 0..65000 ms - fall time (Bar)
  //Hold1_t  = 0..65000 ms - hold time (Bar)
  //MFall1_t = 0..65000 ms - fall time for max (Bar)
  //MHold1_t = 0..65000 ms - hold time for max (Bar)
  //Integ2_t = 0..27000 *0.1 ms (0.0..2700.0 ms) - integration time (Peak)
  //Decay2_t = 0..65000 ms - decay time (Peak)
  //Scale2_f = 0..32000 *0.001 (0.000..32.000) - scaling (Peak)
  //Resp2_t  = 0..65000 ms - response time (Peak)
  //Fall2_t  = 0..65000 ms - fall time (Peak)
  //Hold2_t  = 0..65000 ms - hold time (Peak)
  //MFall2_t = 0..65000 ms - fall time for max (Peak)
  //MHold2_t = 0..65000 ms - hold time for max (Peak)

  CMD_GET_PARAMS, //parameters get
  //see CMD_SET_PARAMS

  CMD_SET_TABLE,  //table load
  //TX: word DB[0], word DB[1] .. DB[49]
  //RX: NONE
  //DB[n] = -9900..+9900 *0.01 dB (-99.00..+99.00 dB)
  //The table must be monotonic. The maximum difference between the values
  //must not exceed 80 dB. All lower values are considered minus infinity.

  CMD_GET_TABLE,  //table read
  //TX: NONE
  //RX: word DB[0], word DB[1] .. DB[49]
  //see CMD_SET_TABLE

  CMD_SET_LEDS,   //LEDs control
  //TX: byte BrMode, byte BrMeter, byte BrScale, word Leds
  //RX: NONE
  //BrMode = BRM_OFF - brightness control off (min brightness)
  //BrMode = BRM_VR - brightness control with trimmer pots
  //BrMode = BRM_DAC - brightness control with built-in DACs
  //BrMode = BRM_PWM - brightness control with PWM
  //BrMeter = 0..100% - bar brightness
  //BrScale = 0..100% - backlight brightness
  //Leds.0  - LED TR0
  //Leds.1  - LED TR1
  //Leds.2  - LED TR2
  //Leds.3  - LED TR3
  //Leds.4  - LED TR4
  //Leds.5  - LED TR5
  //Leds.6  - LED TR6
  //Leds.7  - LED TR7
  //Leds.8  - LED TR8
  //Leds.9  - LED TR9
  //Leds.10 - scale backlight

  CMD_GET_LEDS,   //LEDs read
  //TX: NONE
  //RX: byte BrMode, byte BrMeter, byte BrScale, word Leds
  //see CMD_SET_LEDS

  CMD_EE_SAVE,    //saving parameters in EEPROM
  //TX: NONE
  //RX: NONE
  //The command makes saving the current preset in EEPROM.
  //The execution time of this command depends on the amount of data being
  //changed in the EEPROM and the write time. In the worst case, the time
  //can reach 1800 ms.

  CMD_DEFAULT,     //load defaults
  //TX: NONE
  //RX: NONE
  //The command loads the default parameters for the current preset.
  //They are not saved to EEPROM.

  CMD_TEST         //test command
  //TX: NONE
  //RX: dword v
  //v - debug value
};

//----------------------------------------------------------------------------

#endif
