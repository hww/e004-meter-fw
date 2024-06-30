//----------------------------------------------------------------------------

//����� ����������� ���������� ���� I2C, ������������ ����

//----------------------------------------------------------------------------

#ifndef I2CSW_H
#define I2CSW_H

//----------------------------- ���������: -----------------------------------

#define I2C_RD       1 //������� ������ ������ �� ���� I2C
#define I2C_ACK      1 //������� �������� ACK
#define I2C_NACK     0 //������� �������� NACK

//----------------------------------------------------------------------------
//----------------------------- ����� TI2Csw: --------------------------------
//----------------------------------------------------------------------------

class TI2Csw
{
private:
  static Pin_SCL_t Pin_SCL;
  static Pin_SDA_t Pin_SDA;
  static void BitDelay(void);
  static uint16_t const I2C_CLK = 100; //������� ���� I2C, ���
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
