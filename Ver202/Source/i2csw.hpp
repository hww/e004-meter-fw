//----------------------------------------------------------------------------

// ласс программной реализации шины I2C, заголовочный файл

//----------------------------------------------------------------------------

#ifndef I2CSW_H
#define I2CSW_H

//-----------------------------  онстанты: -----------------------------------

#define I2C_RD       1 //признак чтени€ данных по шине I2C
#define I2C_ACK      1 //признак передачи ACK
#define I2C_NACK     0 //признак передачи NACK

//----------------------------------------------------------------------------
//-----------------------------  ласс TI2Csw: --------------------------------
//----------------------------------------------------------------------------

class TI2Csw
{
private:
  static Pin_SCL_t Pin_SCL;
  static Pin_SDA_t Pin_SDA;
  static void BitDelay(void);
  static uint16_t const I2C_CLK = 100; //частота шины I2C, к√ц
  static uint16_t const I2C_DELAY = APB2_CLOCK / 1000 / I2C_CLK / 2 - 18;
public:
  static void Init(void);
  static void Free(void);
  static void Start(void);
  static bool Write(uint8_t data);
  static uint8_t Read(bool ack);
  static void Stop(void);
};

//----------------------------------------------------------------------------

#endif
