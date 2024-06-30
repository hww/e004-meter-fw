//----------------------------------------------------------------------------

//Класс обработки данных, заголовочный файл

//----------------------------------------------------------------------------

#ifndef PROCESS_HPP
#define PROCESS_HPP

//----------------------------------------------------------------------------

#include "adc.hpp"
#include "display.hpp"
#include "dac.hpp" //TODO
#include "eeprom.hpp"
#include "eemap.hpp"

//------------------------------- Константы: ---------------------------------

//Основные константы:
static const double PI = 3.14159265358979323846;
static const double DCRF_FN = 5.0; //DC removal filter corner frequency, Hz
static const double DCRF_POLE = 1.0 - 2.0 * PI * DCRF_FN / ADC_FS;
static const double TS_MS = 1.0 / (ADC_FS / 1000.0); //T_sample, ms
static const double TD_MS = TS_MS * ADC_DCM; //decimated T_sample, ms
static const double INT_R = 4.0; //T_int/tau_int ratio for -2 dB
static const double DEC_R = 2.3; //T_resp/tau_resp, T_dec/tau_dec ratio for -1 dB
static const double INT_ZERO = TS_MS * INT_R; //T_int zero value
static const double DEC_ZERO = TD_MS * DEC_R; //T_resp, T_dec zero value

//Default values (TS_MS domain):
static const double INT1_T = 5.0;      //integration time 1, ms
static const double INT2_T = INT_ZERO; //integration time 2, ms
//Default values (TD_MS domain):
static const double DEC1_T = 1700.0;   //decay time 1, ms
static const double DEC2_T = DEC_ZERO; //decay time 2, ms
static const double RES1_T = 100.0;    //response time 1, ms
static const double RES2_T = 100.0;    //response time 2, ms
static const double FAL1_T = 1700.0;   //decay time 1, ms
static const double FAL2_T = DEC_ZERO; //decay time 2, ms
static const double HLD1_T = 20.0;     //hold time 1, ms
static const double HLD2_T = 2000.0;   //hold time 2, ms
//Default values (scales):
static const double SCALE1 = 1.0;      //scale 1
static const double SCALE2 = 1.0;      //scale 2

//Вычисление целочисленных коэффициентов:
static const uint16_t DCRF_A = (uint16_t)(DCRF_POLE * UINT16_MAX);
static const uint16_t INF1_A = (uint16_t)(TS_MS * INT_R / INT1_T * UINT16_MAX);
static const uint16_t INF2_A = (uint16_t)(TS_MS * INT_R / INT2_T * UINT16_MAX);
static const uint16_t DEF1_A = (uint16_t)(TD_MS * DEC_R / DEC1_T * UINT16_MAX);
static const uint16_t DEF2_A = (uint16_t)(TD_MS * DEC_R / DEC2_T * UINT16_MAX);
static const uint16_t RSF1_A = (uint16_t)(TD_MS * DEC_R / RES1_T * UINT16_MAX);
static const uint16_t RSF2_A = (uint16_t)(TD_MS * DEC_R / RES2_T * UINT16_MAX);
static const uint16_t FLF1_A = (uint16_t)(TD_MS * DEC_R / FAL1_T * UINT16_MAX);
static const uint16_t FLF2_A = (uint16_t)(TD_MS * DEC_R / FAL2_T * UINT16_MAX);
static const uint16_t SCL1_A = (uint16_t)(SCALE1 * 0x04FF);
static const uint16_t SCL2_A = (uint16_t)(SCALE2 * 0x04FF);
static const uint16_t HLD1_A = (uint16_t)(HLD1_T / TD_MS);
static const uint16_t HLD2_A = (uint16_t)(HLD2_T / TD_MS);

static const int32_t M1DB  = (int32_t)(0.89125 * UINT16_MAX); //-1 dB
//static const int32_t M20DB = (int32_t)(0.1 * UINT16_MAX);     //-20 dB

//TODO: проверить размер кода с константами double и без

//----------------------------------------------------------------------------
//----------------------------- Класс TProcess: ------------------------------
//----------------------------------------------------------------------------

class TProcess
{
private:
  TAdc *Adc;
  Pin_DIR_t Pin_TEST; //TODO
  TDac<DAC1> Dac1; //TODO
  TDac<DAC2> Dac2; //TODO
  chan_t Chan; //TODO
  uint16_t Buff[ADC_DCM * 2]; //ADC buffer
  //DC removal filter:
  int32_t DCRF_acc;      //DC removal filter accumulator
  int16_t DCRF_in_1;     //DC removal filter input value
  int16_t DCRF_out_1;    //DC removal filter output value
  //Detector:
  int16_t DET_out;       //detector output value
  //Integration filter 1:
  int32_t INF1_acc;      //integration filter accumulator 1
  static int32_t INF1_a; //integration coefficient 1
  static int32_t DEF1_a; //decay coefficient 1
  //Integration filter 2:
  int32_t INF2_acc;      //integration filter accumulator 2
  static int32_t INF2_a; //integration coefficient 2
  static int32_t DEF2_a; //decay coefficient 2
  //Response filter 1:
  int32_t RSF1_acc;      //responce filter accumulator 1
  static int32_t RSF1_a; //responce coefficient 1
  static int32_t FLF1_a; //fall back coefficient 1
  //Response filter 2:
  int32_t RSF2_acc;      //responce filter accumulator 2
  static int32_t RSF2_a; //responce coefficient 2
  static int32_t FLF2_a; //fall back coefficient 2
  //Hold 1:
  uint16_t HLD1_acc;      //hold accumulator 1
  static uint16_t HLD1_a; //hold interval 1
  //Hold 2:
  uint16_t HLD2_acc;      //hold accumulator 2
  static uint16_t HLD2_a; //hold interval 2
  //Scale 1:
  static int32_t SCL1_a;  //scale coefficient 1
  //Scale 2:
  static int32_t SCL2_a;  //scale coefficient 2
public:
  TProcess(chan_t chan);
  bool Execute(void);
  //read result:
  int16_t GetBar(void);
  int16_t GetPeak(void);
  //coefficients setters and getters:
  static void SetINF1_a(uint16_t v); static uint16_t GetINF1_a(void);
  static void SetINF2_a(uint16_t v); static uint16_t GetINF2_a(void);
  static void SetDEF1_a(uint16_t v); static uint16_t GetDEF1_a(void);
  static void SetDEF2_a(uint16_t v); static uint16_t GetDEF2_a(void);
  static void SetRSF1_a(uint16_t v); static uint16_t GetRSF1_a(void);
  static void SetRSF2_a(uint16_t v); static uint16_t GetRSF2_a(void);
  static void SetFLF1_a(uint16_t v); static uint16_t GetFLF1_a(void);
  static void SetFLF2_a(uint16_t v); static uint16_t GetFLF2_a(void);
  static void SetSCL1_a(uint16_t v); static uint16_t GetSCL1_a(void);
  static void SetSCL2_a(uint16_t v); static uint16_t GetSCL2_a(void);
  static void SetHLD1_a(uint16_t v); static uint16_t GetHLD1_a(void);
  static void SetHLD2_a(uint16_t v); static uint16_t GetHLD2_a(void);
  //EEPROM read/write:
  static void EERead(void); static void EESave(void);
};

//----------------------------------------------------------------------------

#endif
