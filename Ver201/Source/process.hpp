//----------------------------------------------------------------------------

//Класс обработки данных, заголовочный файл

//----------------------------------------------------------------------------

#ifndef PROCESS_HPP
#define PROCESS_HPP

//----------------------------------------------------------------------------

#include "adc.hpp"

//----------------------------------------------------------------------------
//------------------------------- Константы: ---------------------------------
//----------------------------------------------------------------------------

//-------------------------- Основные константы: -----------------------------

static const double PI = 3.14159265358979323846;

static const double DCRF_FN = 5.0; //DC removal filter corner frequency, Hz
static const double DCRF_POLE = 1.0 - 2.0 * PI * DCRF_FN / ADC_FS;
static const uint16_t DCRF_A = (uint16_t)(DCRF_POLE * UINT16_MAX);

static const int32_t M1DB = (int32_t)(0.89125 * UINT16_MAX); //-1 dB

static const double TS_MS = 1.0 / (ADC_FS / 1000.0); //T_sample, ms
static const double TD_MS = TS_MS * ADC_DCM; //decimated T_sample, ms

static const double INTEG_TTAU = 4.0; //T_int/tau_int ratio for -2 dB (Sin)
static const double DECAY_TTAU = 2.3; //T_dec/tau_dec ratio for -20 dB (DC)
static const double RESP_TTAU  = 2.3; //T_resp/tau_resp ratio for -1 dB (DC)
static const double FALL_TTAU  = 2.3; //T_fall/tau_fall ratio for -20 dB (DC)

static const int32_t SCALE16 = 65536; //сдвиг на 16 разрядов
static const int32_t SCALE12 = 4096;  //сдвиг на 12 разрядов
static const int16_t DET_MAX = 2047;  //максимальный сигнал детектора
static const int16_t LEV_MAX = 10000; //максимальный код уровня
static const int16_t INT_RES = 10;    //разрешение кода времени интеграции
static const int16_t LEV_RES = 1000;  //разрешение кода уровня

//Integ_a = INTEG_K (27306) / Integ_t (0..27000) [0.1 ms]
static const uint32_t INTEG_K = (uint32_t)(INTEG_TTAU * TS_MS * SCALE16 * INT_RES);
//Decay_a = DECAY_K (401954) / Decay_t (0..65000) [1 ms]
static const uint32_t DECAY_K = (uint32_t)(DECAY_TTAU * TD_MS * SCALE16);
//Resp_a = RESP_K (401954) / Resp_t (0..65000) [1 ms]
static const uint32_t RESP_K  = (uint32_t)(RESP_TTAU * TD_MS * SCALE16);
//Fall_a = FALL_K (401954) / Fall_t (0..65000) [1 ms]
static const uint32_t FALL_K  = (uint32_t)(FALL_TTAU * TD_MS * SCALE16);
//Hold_a = HOLD_K (24576) * Hold_t (0..65000) [1 ms] / SCALE16
static const uint32_t HOLD_K  = (uint32_t)(SCALE16 / TD_MS);
//Scale_a = SCALE_K (81960) * Scale_f (0..32000)[0..32.000] / SCALE12
static const uint32_t SCALE_K = (uint32_t)(LEV_MAX * SCALE12 / LEV_RES * SCALE12 / DET_MAX);

static const double CLIP = 1.1; //относительный уровень ограничения сигнала
static const int32_t LEV_LIM = (int32_t)(CLIP * LEV_MAX * SCALE12);

//----------------------------------------------------------------------------
//----------------------------- Класс TProcess: ------------------------------
//----------------------------------------------------------------------------

class TProcess
{
private:
  TAdc *Adc;
  uint16_t Buff[ADC_DCM * 2]; //ADC buffer
  //DC removal filter:
  int32_t DCRF_acc;         //DC removal filter accumulator
  int16_t DCRF_in_1;        //DC removal filter input value
  int16_t DCRF_out_1;       //DC removal filter output value
  //Detector:
  int16_t DET_out;          //detector output value
  //Bar meter:
  int32_t Integ1_acc;       //integration filter accumulator
  static int32_t Integ1_a;  //integration coefficient
  static int32_t Decay1_a;  //decay coefficient
  static int32_t Scale1_a;  //scale coefficient
  int32_t Resp1_acc;        //responce filter accumulator
  static int32_t Resp1_a;   //response coefficient
  static int32_t Fall1_a;   //fall back coefficient
  uint16_t Hold1_tmr;       //hold timer
  static uint16_t Hold1_a;  //hold time, Td
  int32_t Max1_acc;         //max filter accumulator
  static int32_t MFall1_a;  //max fall back coefficient
  uint16_t MHold1_tmr;      //max hold timer
  static uint16_t MHold1_a; //max hold time, Td
  int32_t AMax1_acc;        //abs max filter accumulator
  //Peak meter:
  int32_t Integ2_acc;       //integration filter accumulator
  static int32_t Integ2_a;  //integration coefficient
  static int32_t Decay2_a;  //decay coefficient
  static int32_t Scale2_a;  //scale coefficient
  int32_t Resp2_acc;        //response filter accumulator
  static int32_t Resp2_a;   //response coefficient
  static int32_t Fall2_a;   //fall back coefficient
  uint16_t Hold2_tmr;       //hold timer
  static uint16_t Hold2_a;  //hold time
  int32_t Max2_acc;         //max filter accumulator
  static int32_t MFall2_a;  //max fall back coefficient
  uint16_t MHold2_tmr;      //max hold timer
  static uint16_t MHold2_a; //max hold time
  int32_t AMax2_acc;        //abs max filter accumulator
public:
  TProcess(chan_t chan);
  bool Execute(void);
  void ResetFilters(void);
  void ResetAbsMax(void);
  //read result:
  inline int16_t GetBar(void)         { return(HI_W(Resp1_acc)); }
  inline int16_t GetPeak(void)        { return(HI_W(Resp2_acc)); }
  inline int16_t GetBarMax(void)      { return(HI_W(Max1_acc)); }
  inline int16_t GetPeakMax(void)     { return(HI_W(Max2_acc)); }
  inline int16_t GetBarAbsMax(void)   { return(HI_W(AMax1_acc)); }
  inline int16_t GetPeakAbsMax(void)  { return(HI_W(AMax2_acc)); }
  //parameters (bar):
  static uint16_t Integ1_t; //integration time, 0.. *0.1 ms
  static uint16_t Decay1_t; //decay time, 0..ms
  static uint16_t Scale1_f; //scale factor, 0..32000 (0.000..32.000)
  static uint16_t Resp1_t;  //response time, 0..ms
  static uint16_t Fall1_t;  //fall back time, 0..ms
  static uint16_t Hold1_t;  //hold time, 0..ms
  static uint16_t MFall1_t; //max fall back time, 0..ms
  static uint16_t MHold1_t; //max hold time, 0..ms
  //parameters (peak):
  static uint16_t Integ2_t; //integration time, 0.. *0.1 ms
  static uint16_t Decay2_t; //decay time, 0..ms
  static uint16_t Scale2_f; //scale factor, 0..32000 (0.000..32.000)
  static uint16_t Resp2_t;  //response time, 0..ms
  static uint16_t Fall2_t;  //fall back time, 0..ms
  static uint16_t Hold2_t;  //hold time, 0..ms
  static uint16_t MFall2_t; //max fall back time, 0..ms
  static uint16_t MHold2_t; //max hold time, 0..ms
  //update coefficients:
  static void ApplyParams(void);
  //EEPROM read/write:
  static void EERead(uint8_t p);
  static void EESave(void);
};

//----------------------------------------------------------------------------

#endif
