//----------------------------------------------------------------------------

//Presets

//----------------------------------------------------------------------------

#ifndef PRESETS_HPP
#define PRESETS_HPP

//----------------------------------------------------------------------------

#include "control.hpp"
#include "process.hpp"
#include "display.hpp"
#include "bright.hpp"

//----------------------------------------------------------------------------

//The integration time INTEG1_Px is set in ms with a resolution of 0.1 ms.
//It is greater than the time constant of the integrating RC circuit in
//INTEG_TTAU = 4.0 times (due to the requirement to reach -2 dB with a sine wave).
//This time can be set in the range 0 - 2700.0 ms.

//The decay time of the integrating filter DECAY1_Px is set in ms.
//It is greater than the time constant of the integrating RC circuit in
//DECAY_TTAU = 2.3 times (due to the requirement to reach -20 dB with DC signal).
//This time can be set in the range 0 - 65000 ms.

//The response time of RESP1_Px is set in ms.
//It is greater than the time constant of the integrating RC circuit in
//RESP_TTAU = 2.3 times (due to the requirement to reach -1 dB with DC signal).
//This time can be set in the range 0 - 65000 ms.

//The fall time of FALL1_Px and MFALL1_Px (for max. value) is set in ms.
//It is greater than the time constant of the integrating RC circuit in
//FALL_TTAU = 2.3 times (due to the requirement to reach -20 dB with DC signal).
//This time can be set in the range 0 - 65000 ms.

//The hold time of HOLD1_Px and MHOLD1_Px (for max. value) is set in ms.
//This time can be set in the range 0 - 65000 ms.

//The scaling factor SCALE1_Px has a nominal value of 1.000.
//It can be set in the range 0 - 32.000 with a resolution of 0.001.
//The maximum gain is approximately 30 dB. You can use the gain to shift
//the scale when you need to measure small signals more accurately.
//In normal operation, you should use the full scale of the ADC:
//the value of SCALE1_Px should be as small as possible, but the maximum reading
//of the meter should be achieved. The value of SCALE1_Px depends on the ratio
//of the discharge time of the integrating filter to the integration time.

//------------ Macros for setting tau instead of time constant: --------------

#define INTEG_TAU(x) (x * INTEG_TTAU)
#define DECAY_TAU(x) ((uint16_t)(x * DECAY_TTAU))
#define RESP_TAU(x)  ((uint16_t)(x * RESP_TTAU))
#define FALL_TAU(x)  ((uint16_t)(x * FALL_TTAU))

//---------- Value conversion macros with resolution less than 1: ------------

#define IRES(x) ((uint16_t)(x * INT_RES))
#define LRES(x) ((uint16_t)(x * LEV_RES))

//----------------------------------------------------------------------------
//-------------------------------- Preset 1: ---------------------------------
//----------------------------------------------------------------------------

//The bars represent the quasi-peak level with standard integration and decay
//times (5 ms and 1700 ms).
//The dots represent the peak (true peak) level.

static const double   INTEG1_P1 = 5.0;   //integration time 1, ms
static const uint16_t DECAY1_P1 = 1700;  //decay time 1, ms
static const double   SCALE1_P1 = 1.100; //scale 1
static const uint16_t RESP1_P1  = 100;   //response time 1, ms
static const uint16_t FALL1_P1  = 1700;  //fall time 1, ms
static const uint16_t HOLD1_P1  = 20;    //hold time 1, ms
static const uint16_t MFALL1_P1 = 0;     //max fall time 1, ms
static const uint16_t MHOLD1_P1 = 0;     //max hold time 1, ms

static const double   INTEG2_P1 = 0.0;   //integration time 2, ms
static const uint16_t DECAY2_P1 = 0;     //decay time 2, ms
static const double   SCALE2_P1 = 1.050; //scale 2
static const uint16_t RESP2_P1  = 100;   //response time 2, ms
static const uint16_t FALL2_P1  = 600;   //fall time 2, ms
static const uint16_t HOLD2_P1  = 1000;  //hold time 2, ms
static const uint16_t MFALL2_P1 = 0;     //max fall time 2, ms
static const uint16_t MHOLD2_P1 = 0;     //max hold time 2, ms

static const uint8_t  DMD_P1 = DP_BAR + DP_PEAK; //display mode
static const uint8_t  BMD_P1 = BRM_VR;   //brightness mode
static const uint8_t  BRM_P1 = 100;      //meter brightness
static const uint8_t  BRS_P1 = 100;      //scale brightness
#if defined(USE_KEYBOARD)
static const uint16_t LEDS_P1 = TR1 + TR6 + TR7 + SCALE; //leds set
#elif defined(USE_LEDS_MUX)
static const uint16_t LEDS_P1 = TR1 + TR4 + TR8 + SCALE; //leds set
#else
static const uint16_t LEDS_P1 = TR1 + TR3 + TR7 + SCALE; //leds set
#endif

//----------------------------------------------------------------------------
//-------------------------------- Preset 2: ---------------------------------
//----------------------------------------------------------------------------

//The bars represent the quasi-peak level with standard integration and decay
//times (5 ms and 1700 ms).
//The dots represent the maximum bar level.

static const double   INTEG1_P2 = 5.0;   //integration time 1, ms
static const uint16_t DECAY1_P2 = 1700;  //decay time 1, ms
static const double   SCALE1_P2 = 1.100; //scale 1
static const uint16_t RESP1_P2  = 100;   //response time 1, ms
static const uint16_t FALL1_P2  = 1700;  //fall time 1, ms
static const uint16_t HOLD1_P2  = 20;    //hold time 1, ms
static const uint16_t MFALL1_P2 = 600;   //max fall time 1, ms
static const uint16_t MHOLD1_P2 = 1000;  //max hold time 1, ms

static const double   INTEG2_P2 = 0.0;   //integration time 2, ms
static const uint16_t DECAY2_P2 = 0;     //decay time 2, ms
static const double   SCALE2_P2 = 0.000; //scale 2
static const uint16_t RESP2_P2  = 0;     //response time 2, ms
static const uint16_t FALL2_P2  = 0;     //fall time 2, ms
static const uint16_t HOLD2_P2  = 0;     //hold time 2, ms
static const uint16_t MFALL2_P2 = 0;     //max fall time 2, ms
static const uint16_t MHOLD2_P2 = 0;     //max hold time 2, ms

static const uint8_t  DMD_P2 = DP_BAR + DP_BARMAX; //display mode
static const uint8_t  BMD_P2 = BRM_VR;    //brightness mode
static const uint8_t  BRM_P2 = 100;       //meter brightness
static const uint8_t  BRS_P2 = 100;       //scale brightness
#if defined(USE_KEYBOARD)
static const uint16_t LEDS_P2 = TR1 + TR6 + TR7 + SCALE; //leds set
#elif defined(USE_LEDS_MUX)
static const uint16_t LEDS_P2 = TR1 + TR4 + TR8 + SCALE; //leds set
#else
static const uint16_t LEDS_P2 = TR1 + TR3 + TR7 + SCALE; //leds set
#endif

//----------------------------------------------------------------------------
//-------------------------------- Preset 3: ---------------------------------
//----------------------------------------------------------------------------

//The bars represent the quasi-peak level with the integration and decay times
//for the E-004 R-R tape deck meter.
//The tau values are given here, so they are multiplied by the coefficients.

static const double   INTEG1_P3 = INTEG_TAU(12.0); //integration time 1, ms
static const uint16_t DECAY1_P3 = DECAY_TAU(1500); //decay time 1, ms
static const double   SCALE1_P3 = 1.140; //scale 1
static const uint16_t RESP1_P3  = 0;     //response time 1, ms
static const uint16_t FALL1_P3  = FALL_TAU(1500); //fall time 1, ms
static const uint16_t HOLD1_P3  = 0.0;   //hold time 1, ms
static const uint16_t MFALL1_P3 = 0;     //max fall time 1, ms
static const uint16_t MHOLD1_P3 = 0.0;   //max hold time 1, ms

static const double   INTEG2_P3 = 0.0;   //integration time 2, ms
static const uint16_t DECAY2_P3 = 0;     //decay time 2, ms
static const double   SCALE2_P3 = 0.000; //scale 2
static const uint16_t RESP2_P3  = 0;     //response time 2, ms
static const uint16_t FALL2_P3  = 0;     //fall time 2, ms
static const uint16_t HOLD2_P3  = 0.0;   //hold time 2, ms
static const uint16_t MFALL2_P3 = 0;     //max fall time 2, ms
static const uint16_t MHOLD2_P3 = 0.0;   //max hold time 2, ms

static const uint8_t  DMD_P3 = DP_BAR;   //display mode
static const uint8_t  BMD_P3 = BRM_VR;   //brightness mode
static const uint8_t  BRM_P3 = 100;      //meter brightness
static const uint8_t  BRS_P3 = 100;      //scale brightness
#if defined(USE_KEYBOARD)
static const uint16_t LEDS_P3 = TR1 + TR6 + TR7 + SCALE; //leds set
#elif defined(USE_LEDS_MUX)
static const uint16_t LEDS_P3 = TR1 + TR4 + TR8 + SCALE; //leds set
#else
static const uint16_t LEDS_P3 = TR1 + TR3 + TR7 + SCALE; //leds set
#endif

//----------------------------------------------------------------------------
//-------------------------------- Preset 4: ---------------------------------
//----------------------------------------------------------------------------

//The parameters correspond to the Dorrough 40-A2 meter.
//The tau values are given here, so they are multiplied by the coefficients.

static const double   INTEG1_P4 = INTEG_TAU(300.0); //integration time 1, ms
static const uint16_t DECAY1_P4 = DECAY_TAU(300); //decay time 1, ms
static const double   SCALE1_P4 = 3.5124; //scale 1
static const uint16_t RESP1_P4  = 0;     //response time 1, ms
static const uint16_t FALL1_P4  = FALL_TAU(300); //fall time 1, ms
static const uint16_t HOLD1_P4  = 0;     //hold time 1, ms
static const uint16_t MFALL1_P4 = 0;     //max fall time 1, ms
static const uint16_t MHOLD1_P4 = 0;     //max hold time 1, ms

static const double   INTEG2_P4 = 0.0;   //integration time 2, ms
static const uint16_t DECAY2_P4 = DECAY_TAU(200); //decay time 2, ms
static const double   SCALE2_P4 = 1.076; //scale 2
static const uint16_t RESP2_P4  = 25;    //response time 2, ms
static const uint16_t FALL2_P4  = FALL_TAU(300); //fall time 2, ms
static const uint16_t HOLD2_P4  = 25;    //hold time 2, ms
static const uint16_t MFALL2_P4 = 0;     //max fall time 2, ms
static const uint16_t MHOLD2_P4 = 2000;  //max hold time 2, ms

static const uint8_t  DMD_P4 = DP_BAR + DP_PEAK + DP_PEAKMAX; //display mode
static const uint8_t  BMD_P4 = BRM_VR;   //brightness mode
static const uint8_t  BRM_P4 = 100;      //meter brightness
static const uint8_t  BRS_P4 = 100;      //scale brightness
#if defined(USE_KEYBOARD)
static const uint16_t LEDS_P4 = TR1 + TR6 + TR7 + SCALE; //leds set
#elif defined(USE_LEDS_MUX)
static const uint16_t LEDS_P4 = TR1 + TR4 + TR8 + SCALE; //leds set
#else
static const uint16_t LEDS_P4 = TR1 + TR3 + TR7 + SCALE; //leds set
#endif

//----------------------------------------------------------------------------
//--------------------------------- Scale: -----------------------------------
//----------------------------------------------------------------------------

//The scale is stored in each preset separately, so you can make individual
//scales for them. By default, one scale is used for all presets.
//The scale is set in the form of a table, where for each segment the turn-on
//level is set in dB with a resolution of 0.01 dB. The table contains the level
//values in dB multiplied by 100. When the measured signal is equal to or higher
//than this level, the segment turns on. The values in the table must be monotonic.
//The last element of the table is the maximum level. The difference between
//the maximum and minimum levels in the table can be up to 80 dB. All levels
//below -80 dB relative to the maximum level are considered minus infinity. This
//level can be set for the first segment of the bar so that it is constantly lit.

#ifdef METER50
  #ifdef USE_KEYBOARD
static const int16_t DBTABLE[SEGS] =
{
   -9900,  -4000,  -3500, -3000,  -2800,  -2600,  -2400,  -2200, -2000,  -1800,
   -1600,  -1400,  -1200, -1000,   -900,   -800,   -700,   -600,  -500,   -450,
    -400,   -350,   -300,  -250,   -200,   -150,   -100,    -75,   -50,    -25,
       0,     25,     50,    75,    100,    125,    150,    175,   200,    250,
     300,    350,    400,   500,    600,    700,    800,    900,  1000,   1200
};
  #else
static const int16_t DBTABLE[SEGS] =
{
   -9900,  -3400,  -3200, -3000,  -2800,  -2600,  -2400,  -2200, -2000,  -1800,
   -1600,  -1400,  -1200, -1000,   -900,   -800,   -700,   -600,  -500,   -450,
    -400,   -350,   -300,  -250,   -200,   -150,   -100,    -75,   -50,    -25,
       0,     25,     50,    75,    100,    125,    150,    175,   200,    250,
     300,    350,    400,   500,    600,    700,    800,    900,  1000,   1100
};
  #endif
#endif

#ifdef METER40
static const int16_t DBTABLE[SEGS] =
{
   -9900,  -2200,  -2100, -2000,  -1900,  -1800,  -1700,  -1600, -1500,  -1400,
   -1300,  -1200,  -1133, -1067,  -1000,   -933,   -867,   -800,  -733,   -667,
    -600,   -533,   -467,  -400,   -333,   -267,   -200,   -150,  -100,    -50,
       0,     50,    100,   150,    200,    267,    333,    400,   500,    600
};
#endif

#ifdef METER35
static const int16_t DBTABLE[SEGS] =
{
   -9900,  -1900,  -1800, -1700,  -1600,  -1500,  -1400,  -1300, -1200,  -1100,
   -1000,   -900,   -800,  -700,   -600,   -500,   -400,   -300,  -200,   -100,
       0,     50,    100,   150,    200,    250,    300,    350,   400,    450,
     500,    600,    700,   800,    900
};
#endif

//----------------------------------------------------------------------------
//-------------------- Filling arrays of coefficients: -----------------------
//----------------------------------------------------------------------------

static const uint16_t INTEG1_P[PRESETS] =
{ IRES(INTEG1_P1), IRES(INTEG1_P2), IRES(INTEG1_P3), IRES(INTEG1_P4) };
static const uint16_t DECAY1_P[PRESETS] =
{ DECAY1_P1, DECAY1_P2, DECAY1_P3, DECAY1_P4 };
static const uint16_t SCALE1_P[PRESETS] =
{ LRES(SCALE1_P1), LRES(SCALE1_P2), LRES(SCALE1_P3), LRES(SCALE1_P4) };
static const uint16_t RESP1_P[PRESETS] =
{ RESP1_P1, RESP1_P2, RESP1_P3, RESP1_P4 };
static const uint16_t FALL1_P[PRESETS] =
{ FALL1_P1, FALL1_P2, FALL1_P3, FALL1_P4 };
static const uint16_t HOLD1_P[PRESETS] =
{ HOLD1_P1, HOLD1_P2, HOLD1_P3, HOLD1_P4 };
static const uint16_t MFALL1_P[PRESETS] =
{ MFALL1_P1, MFALL1_P2, MFALL1_P3, MFALL1_P4 };
static const uint16_t MHOLD1_P[PRESETS] =
{ MHOLD1_P1, MHOLD1_P2, MHOLD1_P3, MHOLD1_P4 };

static const uint16_t INTEG2_P[PRESETS] =
{ IRES(INTEG2_P1), IRES(INTEG2_P2), IRES(INTEG2_P3), IRES(INTEG2_P4) };
static const uint16_t DECAY2_P[PRESETS] =
{ DECAY2_P1, DECAY2_P2, DECAY2_P3, DECAY2_P4 };
static const uint16_t SCALE2_P[PRESETS] =
{ LRES(SCALE2_P1), LRES(SCALE2_P2), LRES(SCALE2_P3), LRES(SCALE2_P4) };
static const uint16_t RESP2_P[PRESETS] =
{ RESP2_P1, RESP2_P2, RESP2_P3, RESP2_P4 };
static const uint16_t FALL2_P[PRESETS] =
{ FALL2_P1, FALL2_P2, FALL2_P3, FALL2_P4 };
static const uint16_t HOLD2_P[PRESETS] =
{ HOLD2_P1, HOLD2_P2, HOLD2_P3, HOLD2_P4 };
static const uint16_t MFALL2_P[PRESETS] =
{ MFALL2_P1, MFALL2_P2, MFALL2_P3, MFALL2_P4 };
static const uint16_t MHOLD2_P[PRESETS] =
{ MHOLD2_P1, MHOLD2_P2, MHOLD2_P3, MHOLD2_P4 };

static const uint8_t DMD_P[PRESETS] =
{ DMD_P1, DMD_P2, DMD_P3, DMD_P4};
static const uint8_t BMD_P[PRESETS] =
{ BMD_P1, BMD_P2, BMD_P3, BMD_P4 };
static const uint8_t BRM_P[PRESETS] =
{ BRM_P1, BRM_P2, BRM_P3, BRM_P4 };
static const uint8_t BRS_P[PRESETS] =
{ BRS_P1, BRS_P2, BRS_P3, BRS_P4 };
static const uint16_t LEDS_P[PRESETS] =
{ LEDS_P1, LEDS_P2, LEDS_P3, LEDS_P4 };

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif
