//----------------------------------------------------------------------------

//Data processing class

//-------------------------- Resources Used: ---------------------------------

//The TProcess class performs data processing for one meter channel.
//The TAdc class is used to receive data. The Execute() method must be called
//in the main loop. If the ADC data is ready during the next call, then it is
//processed and the method returns 1. If the data is not ready, the method
//returns 0. Processing is performed in two clock domains: at a sampling
//frequency of 96 kHz (TS_MS domain) and after decimation at a frequency
//of 96000/256 = 375 Hz (TD_MS domain).

//Processing starts in the 96 kHz domain (TS_MS domain).

//First, the DC component of the input signal is removed.
//For this purpose, a DC Removal Filter is used, consisting of a
//series-connected differentiator and a Leaky Integrator.
//Requantization error correction is used. The 12-bit input to the DCRF_in
//filter comes from the ADC array. Accumulation takes place in a 32-bit
//accumulator DCRF_acc. The cutoff frequency of the filter is given by the
//DCRF_A coefficient and is set to 5 Hz. The filter has a zero at z = 1 and
//a pole at z = 0.999673. Filter output DCRF_out_1 is also 12-bit.

//The next processing unit is a digital full-wave detector.
//The absolute value of the number DCRF_out_1 is calculated.
//Detector output code DET_out is 11-bit.

//Two IIR filters follow, which form the integration time to indicate the
//average (or quasi-peak) value (displayed as bars), and the peak value
//(displayed as a single segment - a dot).
//Accumulation is performed in 32-bit accumulators Integ1_acc and Integ2_acc.
//The output code of the filter is in the most significant word of the accumulator.
//The filters only work when the signal is rising.
//The time constants are determined by the coefficients Integ1_a and Integ2_a.
//Coefficient values are calculated using the formula Integ_a = 65536/96000/tau.
//To get tau = 0 (True Peak), you need to set Integ_a = 65536.
//The integration time is set using the parameters Integ1_t and Integ2_t.
//It is greater than tau by INTEG_TTAU = 4.0 times (due to the requirement
//to reach -2 dB with a sine signal).
//The integration time is set in the range 0..2700.0 ms with a resolution of 0.1 ms.
//Integ_a = INTEG_K (27306) / Integ_t (0..27000)
//INTEG_K = INTEG_TTAU (4) * TS_MS (1/96) * SCALE16 (65536) * INT_RES (10)

//Further processing is done in the 96000/256 = 375 Ãö domain(TD_MS domain).

//First, the scaling of the signal is done, which is necessary to align the
//readings of bars and points on a stationary signal at different integration
//and decay time constants.
//The output 11-bit code of the previous filters is contained in the most
//significant words of the accumulators Integ1_acc and Integ2_acc.
//It is multiplied by the Scale1_a and Scale2_a coefficients, then divided
//by 4096. The maximum output code is 10000, which is the range used in the
//lookup table. After multiplying by the coefficients, the code is limited
//to LEV_LIM, which is set to about 10% above the selected maximum code.
//At maximum Scale1_a and Scale2_a coefficients, the gain is 32
//(approximately +30 dB).
//It makes no sense to set the gain less than one, because then even with
//the maximum ADC code, the signal will be insufficient to turn on all segments.
//The gain is set using the Scale1_f and Scale2_f parameters.
//Setting range 0..32.000 with resolution 0.001.
//Scale_a = SCALE_K (81960) * Scale_f (0..32000) / SCALE12 (4096)
//SCALE_K = LEV_MAX (10000) * SCALE12 (4096) / LEV_RES (1000) *
//SCALE12 (4096) / DET_MAX (2047)

//Two IIR filters follow, which form the response time.
//They work in the "slow" domain.
//The output code of these filters is directly related to the meter readings.
//Accumulation is performed in 32-bit accumulators Resp1_acc and Resp2_acc.
//The output code of the filter is in the most significant word of the accumulator.
//The filters only work when the signal is rising.
//The time constants are determined by the coefficients Resp1_a and Resp2_a.
//The coefficient values are calculated according to the formula A = 65536/375/tau.
//To get tau = 0, you need to set A = 65536. In this case, the response time
//will be equal to the integration time.
//The response time is set using the parameters Resp1_t and Resp2_t.
//It is greater than tau by a factor of RESP_TTAU = 2.3 times (due to the
//requirement to reach -1 dB on a DC signal).
//The response time is set in the range of 0..65000 ms with a resolution of 1 ms.
//Resp_a = RESP_K (401954) / Resp_t (0..65000)
//RESP_K = RESP_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536)

//When the output code of the response filter reaches the input code with
//an accuracy of 1 dB, then the decay filter is turned on, affecting the
//accumulators of the integration filters Integ1_acc and Integ2_acc.
//It works in the "slow" domain.
//The decay time constants are determined by the Decay1_a and Decay2_a coefficients.
//The coefficient values are calculated according to the formula A = 65536/375/tau.
//To get tau = 0, you need to set A = 65536.
//The decay time is set using the parameters Decay1_t and Decay2_t.
//It is greater than tau in DECAY_TTAU = 2.3 times (due to the requirement
//to reach -20 dB on a DC signal).
//The decay time is set in the range of 0..65000 ms with a resolution of 1 ms.
//Decay_a = DECAY_K (401954) / Decay_t (0..65000)
//DECAY_K = DECAY_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536)

//The meter reading may have some hold time where the reading stays the same
//after the peak of the signal. This time is set using the Hold1_a and Hold2_a
//coefficients.
//The values of the coefficients are calculated by the formula A = 375*t_hold.
//If you set the coefficients to zero, there will be no holding of readings,
//immediately after the peak of the signal, the fall will begin.
//The hold time is set using the parameters Hold1_t and Hold2_t.
//It is set in the range 0..65000 ms with a resolution of 1 ms.
//Hold_a = HOLD_K (24576) * Hold_t (0..65000) / SCALE16 (65536)
//HOLD_K  = SCALE16 (65536) / TD_MS (256/96)

//When the hold time ends, the meter falls.
//The fall filter acts on the accumulators of the response filters Resp1_acc
//and Resp2_acc. It works in the "slow" domain. The fall time constants are
//determined by the coefficients Fall1_a and Fall2_a.
//The coefficient values are calculated according to the formula A = 65536/375/tau.
//To get tau = 0, you need to set A = 65536.
//The fall time is set using the parameters Fall1_t and Fall2_t.
//It is greater than tau by FALL_TTAU = 2.3 times (due to the requirement to
//reach -20 dB on a DC signal).
//The fall time is set in the range of 0..65000 ms with a resolution of 1 ms.
//Fall_a = FALL_K (401954) / Fall_t (0..65000)
//FALL_K  = FALL_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536);

//Separate fall implementation for response filters allows to use reduced
//decay time for integration filters, which reduces peak-to-peak time impact
//on meter readings. As the integration time decreases, the level for the
//stationary signal will decrease somewhat, which requires correction using
//scaling factors.
//In the simplest case, the decay and fall time are set to be the same.

//The maximum value of the measured signal can be stored and held for some time.
//For storage, the accumulators of the maximum value Max1_acc and Max2_acc are
//used. The holding time of these values is set using the coefficients MHold1_a
//and MHold2_a.
//The values of the coefficients are calculated by the formula A = 375*t_hold.
//If you set the coefficients to zero, there will be no holding of readings,
//immediately after the peak of the signal, the fall will begin.
//The hold time is set using the MHold1_t and MHold2_t parameters.
//It is set in the range 0..65000 ms with a resolution of 1 ms.
//MHold_a = HOLD_K (24576) * Hold_t (0..65000) / SCALE16 (65536)
//HOLD_K  = SCALE16 (65536) / TD_MS (256/96)

//When the hold time ends, the fall of the maximum values begins.
//The fall filter acts on the Max1_acc and Max2_acc accumulators.
//It works in the "slow" domain. The fall time constants are determined by
//the coefficients MFall1_a and MFall2_a.
//The coefficient values are calculated according to the formula A = 65536/375/tau.
//To get tau = 0, you need to set A = 65536.
//The fall time is set using the parameters MFall1_t and MFall2_t.
//It is greater than tau in FALL_TTAU = 2.3 times (due to the requirement to
//reach the level -20 dB with DC signal).
//The rfall time is set in the range of 0..65000 ms with a resolution of 1 ms.
//MFall_a = FALL_K (401954) / Fall_t (0..65000)
//FALL_K  = FALL_TTAU (2.3) * TD_MS (256/96) * SCALE16 (65536);

//In parallel with the accumulators Max1_acc and Max2_acc, the maximum value
//of the measured signal is stored in additional accumulators: AMax1_acc and
//AMax2_acc. They are used to accumulate statistics. When the meter is running,
//the values in these accumulators can only increase. These accumulators do not
//have reverse motion. They accumulate the maximum measured value. The AMax1_acc
//and AMax2_acc accumulators are reset manually using a special command.

//Signal processing is performed in the same way for both branches of
//calculations, but the coefficients for them may be different. The form of the
//output is also different. The measured value of the first branch is displayed
//as bar, and the measured value of the second branch is displayed as a single
//segment - a dot. The maximum values of the signals of both branches are
//displayed as dots.

//For a standard quasi-peak meter, integration tau is set to 1.25 ms (integration
//time 5 ms to -2 dB), fall tau is set to 739 ms (fall time 1700 ms to -20 dB),
//response tau is set to 43..87 ms (response time 100..200 ms to -1 dB),
//hold time - 0 ms.

//For a peak meter, integration tau is 0 ms, response tau is 0..87 ms (response
//time 0..200 ms to -1 dB), hold time is about 2000 ms, fall tau is from 0 ms
//(the point disappears immediately) to 739 ms (the point smoothly falls off
//in 1700 ms to -20 dB).

//----------------------------------------------------------------------------

#include "main.hpp"
#include "process.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"
#include "presets.hpp"

//----------------------------------------------------------------------------
//---------------------------- TProcess class: -------------------------------
//----------------------------------------------------------------------------

//--------------------------- Static variables: ------------------------------

int32_t  TProcess::Integ1_a; uint16_t TProcess::Integ1_t;
int32_t  TProcess::Decay1_a; uint16_t TProcess::Decay1_t;
int32_t  TProcess::Scale1_a; uint16_t TProcess::Scale1_f;
int32_t  TProcess::Resp1_a;  uint16_t TProcess::Resp1_t;
int32_t  TProcess::Fall1_a;  uint16_t TProcess::Fall1_t;
uint16_t TProcess::Hold1_a;  uint16_t TProcess::Hold1_t;
int32_t  TProcess::MFall1_a; uint16_t TProcess::MFall1_t;
uint16_t TProcess::MHold1_a; uint16_t TProcess::MHold1_t;

int32_t  TProcess::Integ2_a; uint16_t TProcess::Integ2_t;
int32_t  TProcess::Decay2_a; uint16_t TProcess::Decay2_t;
int32_t  TProcess::Scale2_a; uint16_t TProcess::Scale2_f;
int32_t  TProcess::Resp2_a;  uint16_t TProcess::Resp2_t;
int32_t  TProcess::Fall2_a;  uint16_t TProcess::Fall2_t;
uint16_t TProcess::Hold2_a;  uint16_t TProcess::Hold2_t;
int32_t  TProcess::MFall2_a; uint16_t TProcess::MFall2_t;
uint16_t TProcess::MHold2_a; uint16_t TProcess::MHold2_t;

//------------------------------ Constructor: --------------------------------

TProcess::TProcess(chan_t chan)
{
  Adc = new TAdc(chan, Buff);
  //DC removal filter:
  DCRF_acc = 0;
  DCRF_in_1 = 0;
  DCRF_out_1 = 0;
  ResetFilters();
}

//----------------------------------------------------------------------------
//------------------------------- Processing: --------------------------------
//----------------------------------------------------------------------------

bool TProcess::Execute(void)
{
  int16_t Offs = Adc->State();
  if(Offs >= 0)
  {

//------------------------------ TS_MS domain: -------------------------------
//----------------------------------------------------------------------------

    uint16_t *Pnt = Buff + Offs;
    for(int i = 0; i < ADC_DCM; i++)
    {

//---------------------------- DC removal filter: ----------------------------

      int16_t DCRF_in = *Pnt++;
      DCRF_acc = LO_W(DCRF_acc) + (int32_t)DCRF_out_1 * DCRF_A;
      DCRF_out_1 = DCRF_in - DCRF_in_1 + HI_W(DCRF_acc);
      DCRF_in_1 = DCRF_in;

//-------------------------------- Detector: ---------------------------------

      DET_out = ABS(DCRF_out_1);

//------------------------ Integration filter (bar): -------------------------

      int16_t Integ1_d = DET_out - HI_W(Integ1_acc);
      if(Integ1_d > 0) Integ1_acc = Integ1_acc + Integ1_d * Integ1_a;

//----------------------- Integration filter (peak): -------------------------

      int16_t Integ2_d = DET_out - HI_W(Integ2_acc);
      if(Integ2_d > 0) Integ2_acc = Integ2_acc + Integ2_d * Integ2_a;
    }

//--------------------------- TD_MS domain (bar): ----------------------------
//----------------------------------------------------------------------------

//-------------------------------- Scaling: ----------------------------------

    int32_t a1 = HI_W(Integ1_acc) * Scale1_a;
    if(a1 > LEV_LIM) a1 = LEV_LIM;
    int16_t Resp1_in = a1 >> 12;

//---------------------------- Response filter: ------------------------------

    int16_t Resp1_d = Resp1_in - HI_W(Resp1_acc);
    if(Resp1_d >= 0) { Resp1_acc = Resp1_acc + Resp1_d * Resp1_a; Hold1_tmr = 0; }

//------------------------------ Decay filter: -------------------------------

    if(HI_W(Resp1_acc) > HI_W(Resp1_in * M1DB))
      Integ1_acc = Integ1_acc - HI_W(Integ1_acc) * Decay1_a;

//------------------------------ Fall filter: --------------------------------

    if(Hold1_tmr < Hold1_a) Hold1_tmr++;
      else Resp1_acc = Resp1_acc - HI_W(Resp1_acc) * Fall1_a;

//------------------------------- Max filter: --------------------------------

    if(MHold1_a)
    {
      if(Max1_acc < Resp1_acc) { Max1_acc = Resp1_acc; MHold1_tmr = 0; }

//----------------------------- Max fall filter: -----------------------------

      if(MHold1_tmr < MHold1_a) MHold1_tmr++;
        else Max1_acc = Max1_acc - HI_W(Max1_acc) * MFall1_a;
    } else Max1_acc = 0;

//----------------------------- Abs max filter: ------------------------------

    if(AMax1_acc < Resp1_acc) AMax1_acc = Resp1_acc;

//----------------------------- TD_MS domain (peak): -------------------------
//----------------------------------------------------------------------------

//-------------------------------- Scaling: ----------------------------------

    int32_t a2 = HI_W(Integ2_acc) * Scale2_a;
    if(a2 > LEV_LIM) a2 = LEV_LIM;
    int16_t Resp2_in = a2 >> 12;

//---------------------------- Response filter: ------------------------------

    int16_t Resp2_d = Resp2_in - HI_W(Resp2_acc);
    if(Resp2_d >= 0) { Resp2_acc = Resp2_acc + Resp2_d * Resp2_a; Hold2_tmr = 0; }

//------------------------------ Decay filter: -------------------------------

    if(HI_W(Resp2_acc) > HI_W(Resp2_in * M1DB))
      Integ2_acc = Integ2_acc - HI_W(Integ2_acc) * Decay2_a;

//------------------------------ Fall filter: --------------------------------

    if(Hold2_tmr < Hold2_a) Hold2_tmr++;
      else Resp2_acc = Resp2_acc - HI_W(Resp2_acc) * Fall2_a;

//------------------------------- Max filter: --------------------------------

    if(MHold2_a)
    {
      if(Max2_acc < Resp2_acc) { Max2_acc = Resp2_acc; MHold2_tmr = 0; }

//----------------------------- Max fall filter: -----------------------------

      if(MHold2_tmr < MHold2_a) MHold2_tmr++;
        else Max2_acc = Max2_acc - HI_W(Max2_acc) * MFall2_a;
    } else Max2_acc = 0;

//----------------------------- Abs max filter: ------------------------------

    if(AMax2_acc < Resp2_acc) AMax2_acc = Resp2_acc;

    return(1);
  }
  return(0);
}

//----------------------------------------------------------------------------
//-------------------------- Interface Methods: ------------------------------
//----------------------------------------------------------------------------

//------------------------- Clearing all filters: ----------------------------

void TProcess::ResetFilters(void)
{
  //bar filters:
  Integ1_acc = 0;
  Resp1_acc  = 0;
  Hold1_tmr  = 0;
  Max1_acc   = 0;
  MHold1_tmr = 0;
  AMax1_acc  = 0;
  //peak filters:
  Integ2_acc = 0;
  Resp2_acc  = 0;
  Hold2_tmr  = 0;
  Max2_acc   = 0;
  MHold2_tmr = 0;
  AMax2_acc  = 0;
}

//-------------------------- Reset absolute max: -----------------------------

void TProcess::ResetAbsMax(void)
{
  AMax1_acc = 0;
  AMax2_acc = 0;
}

//---------------- Converting parameters to coefficients: --------------------

void TProcess::ApplyParams(void)
{
  Integ1_a = Integ1_t? (INTEG_K / Integ1_t) : SCALE16;
  Decay1_a = Decay1_t? (DECAY_K / Decay1_t) : SCALE16;
  Scale1_a = SCALE_K * Scale1_f / SCALE12;
  Resp1_a  = Resp1_t? (RESP_K / Resp1_t) : SCALE16;
  Fall1_a  = Fall1_t? (FALL_K / Fall1_t) : SCALE16;
  Hold1_a  = HOLD_K * Hold1_t / SCALE16;
  MFall1_a = MFall1_t? (FALL_K / MFall1_t) : SCALE16;
  MHold1_a = HOLD_K * MHold1_t / SCALE16;

  Integ2_a = Integ2_t? (INTEG_K / Integ2_t) : SCALE16;
  Decay2_a = Decay2_t? (DECAY_K / Decay2_t) : SCALE16;
  Scale2_a = SCALE_K * Scale2_f / SCALE12;
  Resp2_a  = Resp2_t? (RESP_K / Resp2_t) : SCALE16;
  Fall2_a  = Fall2_t? (FALL_K / Fall2_t) : SCALE16;
  Hold2_a  = HOLD_K * Hold2_t / SCALE16;
  MFall2_a = MFall2_t? (FALL_K / MFall2_t) : SCALE16;
  MHold2_a = HOLD_K * MHold2_t / SCALE16;
}

//------------------- Reading parameters from EEPROM: ------------------------

void TProcess::EERead(uint8_t p)
{
  Integ1_t = Eeprom->Read(EE_INTEG1_T, INTEG1_P[p]);
  Decay1_t = Eeprom->Read(EE_DECAY1_T, DECAY1_P[p]);
  Scale1_f = Eeprom->Read(EE_SCALE1_T, SCALE1_P[p]);
  Resp1_t  = Eeprom->Read(EE_RESP1_T,  RESP1_P[p]);
  Fall1_t  = Eeprom->Read(EE_FALL1_T,  FALL1_P[p]);
  Hold1_t  = Eeprom->Read(EE_HOLD1_T,  HOLD1_P[p]);
  MFall1_t = Eeprom->Read(EE_MFALL1_T, MFALL1_P[p]);
  MHold1_t = Eeprom->Read(EE_MHOLD1_T, MHOLD1_P[p]);

  Integ2_t = Eeprom->Read(EE_INTEG2_T, INTEG2_P[p]);
  Decay2_t = Eeprom->Read(EE_DECAY2_T, DECAY2_P[p]);
  Scale2_f = Eeprom->Read(EE_SCALE2_T, SCALE2_P[p]);
  Resp2_t  = Eeprom->Read(EE_RESP2_T,  RESP2_P[p]);
  Fall2_t  = Eeprom->Read(EE_FALL2_T,  FALL2_P[p]);
  Hold2_t  = Eeprom->Read(EE_HOLD2_T,  HOLD2_P[p]);
  MFall2_t = Eeprom->Read(EE_MFALL2_T, MFALL2_P[p]);
  MHold2_t = Eeprom->Read(EE_MHOLD2_T, MHOLD2_P[p]);

  ApplyParams();
}

//--------------------- Saving parameters in EEPROM: -------------------------

void TProcess::EESave(void)
{
  Eeprom->Write(EE_INTEG1_T, Integ1_t);
  Eeprom->Write(EE_DECAY1_T, Decay1_t);
  Eeprom->Write(EE_SCALE1_T, Scale1_f);
  Eeprom->Write(EE_RESP1_T,  Resp1_t);
  Eeprom->Write(EE_FALL1_T,  Fall1_t);
  Eeprom->Write(EE_HOLD1_T,  Hold1_t);
  Eeprom->Write(EE_MFALL1_T, MFall1_t);
  Eeprom->Write(EE_MHOLD1_T, MHold1_t);

  Eeprom->Write(EE_INTEG2_T, Integ2_t);
  Eeprom->Write(EE_DECAY2_T, Decay2_t);
  Eeprom->Write(EE_SCALE2_T, Scale2_f);
  Eeprom->Write(EE_RESP2_T,  Resp2_t);
  Eeprom->Write(EE_FALL2_T,  Fall2_t);
  Eeprom->Write(EE_HOLD2_T,  Hold2_t);
  Eeprom->Write(EE_MFALL2_T, MFall2_t);
  Eeprom->Write(EE_MHOLD2_T, MHold2_t);
}

//----------------------------------------------------------------------------
