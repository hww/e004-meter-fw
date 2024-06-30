//----------------------------------------------------------------------------

//Пресеты

//----------------------------------------------------------------------------

#ifndef PRESETS_HPP
#define PRESETS_HPP

//----------------------------------------------------------------------------

#include "control.hpp"
#include "process.hpp"
#include "display.hpp"
#include "bright.hpp"

//----------------------------------------------------------------------------

//Время интеграции INTEG1_Px задается в мс с дискретностью 0.1 мс.
//Оно больше постоянной времени интегрирующей цепи в INTEG_TTAU = 4.0 раз
//(из-за требования достижения уровня -2 дБ при Sin сигнале).
//Это время можно задавать в диапазоне 0 - 2700.0 мс.

//Время разряда интегрирующего фильтра DECAY1_Px задается в мс.
//Оно больше постоянной времени интегрирующей цепи в DECAY_TTAU = 2.3 раз
//(из-за требования достижения уровня -20 дБ при DC сигнале).
//Это время можно задавать в диапазоне 0 - 65000 мс.

//Время срабатывания RESP1_Px задается в мс.
//Оно больше постоянной времени интегрирующей цепи в RESP_TTAU = 2.3 раз
//(из-за требования достижения уровня -1 дБ при DC сигнале).
//Это время можно задавать в диапазоне 0 - 65000 мс.

//Время обратного хода FALL1_Px и MFALL1_Px (для макс. значения) задается в мс.
//Оно больше постоянной времени интегрирующей цепи в FALL_TTAU = 2.3 раз
//(из-за требования достижения уровня -20 дБ при DC сигнале).
//Это время можно задавать в диапазоне 0 - 65000 мс.

//Время удержания HOLD1_Px и MHOLD1_Px (для макс. значения) задается в мс.
//Это время можно задавать в диапазоне 0 - 65000 мс.

//Масштабирующий коэффициент SCALE1_Px имеет номинальное значение 1.000.
//Его можно задавать в диапазоне 0 - 32.000 с дискретностью 0.001.
//Максимальное усиление составляет примерно 30 дБ. Использовать усиление можно
//для сдвига шкалы при необходимости более точного измерения малых сигналов.
//В нормальном режиме работы следует использовать всю шкалу АЦП: значение
//SCALE1_Px должно быть как можно меньше, но при этом должны достигаться
//максимальные показания измерителя. Значение SCALE1_Px зависит от отношения
//времени разряда интегрирующего фильтра к времени интеграции.

//----------- Макросы для задания tau вместо постоянной времени: -------------

#define INTEG_TAU(x) (x * INTEG_TTAU)
#define DECAY_TAU(x) ((uint16_t)(x * DECAY_TTAU))
#define RESP_TAU(x)  ((uint16_t)(x * RESP_TTAU))
#define FALL_TAU(x)  ((uint16_t)(x * FALL_TTAU))

//--------- Макросы преобразования величин с разрешением меньше 1: -----------

#define IRES(x) ((uint16_t)(x * INT_RES))
#define LRES(x) ((uint16_t)(x * LEV_RES))

//----------------------------------------------------------------------------
//-------------------------------- Пресет 1: ---------------------------------
//----------------------------------------------------------------------------

//Столбики отображают квазипиковый уровень со стандартными временами
//интеграции и обратного хода (5 мс и 1700 мс).
//Точки отображают пиковый (true peak) уровень.

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
static const uint16_t LEDS_P1 = TR1 + TR4 + TR7 + SCALE; //leds set

//----------------------------------------------------------------------------
//-------------------------------- Пресет 2: ---------------------------------
//----------------------------------------------------------------------------

//Столбики отображают квазипиковый уровень со стандартными временами
//интеграции и обратного хода (5 мс и 1700 мс).
//Точки отображают максимальный уровень столбиков.

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
static const uint16_t LEDS_P2 = TR1 + TR4 + TR7 + SCALE; //leds set

//----------------------------------------------------------------------------
//-------------------------------- Пресет 3: ---------------------------------
//----------------------------------------------------------------------------

//Столбики отображают квазипиковый уровень со стандартными для штатного
//измерителя "Электроника-004" временами интеграции и обратного хода.
//Здесь задаются значения тау, поэтому они домножаются на коэффициенты.

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
static const uint16_t LEDS_P3 = TR1 + TR4 + TR7 + SCALE; //leds set

//----------------------------------------------------------------------------
//-------------------------------- Пресет 4: ---------------------------------
//----------------------------------------------------------------------------

//Параметры соответствуют измерителю Dorrough 40-A2.
//То же, что и предыдущий пресет, только с запоминанием пиков.
//Здесь задаются значения тау, поэтому они домножаются на коэффициенты.

static const double   INTEG1_P4 = INTEG_TAU(300.0); //integration time 1, ms
static const uint16_t DECAY1_P4 = DECAY_TAU(300); //decay time 1, ms
static const double   SCALE1_P4 = 2.790; //scale 1
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
static const uint16_t LEDS_P4 = TR1 + TR4 + TR7 + SCALE; //leds set

//----------------------------------------------------------------------------
//--------------------------------- Шкала: -----------------------------------
//----------------------------------------------------------------------------

//Шкала сохраняется в каждом пресете отдельно, поэтому для них можно сделать
//идивидуальные шкалы. По умолчанию используется одна шкала для всех пресетов.
//Шкала задается в виде таблицы, где для каждого сегмента задан уровень
//срабатывания в dB с дискретностью 0.01 dB. В таблицу заносятся значения
//уровней в dB, умноженные на 100. Когда измеренный сигнал равен или выше
//этого уровня, сегмент включается. Значения в таблице должны быть монотонными.
//Последний элемент таблицы - это максимальный уровень. Разница максимального
//и минимального уровней в таблице может быть до 80 dB. Все уровни ниже
//-80 dB относительно максимального уровня считаются минус бесконечностью.
//Такой уровень может быт задан для первого сегмента линейки, чтобы он
//постоянно горел.

#ifdef METER50
static const int16_t DBTABLE[SEGS] =
{
   -9900,  -3400,  -3200, -3000,  -2800,  -2600,  -2400,  -2200, -2000,  -1800,
   -1600,  -1400,  -1200, -1000,   -900,   -800,   -700,   -600,  -500,   -450,
    -400,   -350,   -300,  -250,   -200,   -150,   -100,    -75,   -50,    -25,
       0,     25,     50,    75,    100,    125,    150,    175,   200,    250,
     300,    350,    400,   500,    600,    700,    800,    900,  1000,   1100
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
//------------------ Заполнение массивов коэффициентов: ----------------------
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
