//----------------------------------------------------------------------------

//������ ��������� ������� EEPROM, ������������ ����

//----------------------------------------------------------------------------

#ifndef EEPROM_H
#define EEPROM_H

//----------------------------------------------------------------------------
//---------------------------- ����� TEeprom: --------------------------------
//----------------------------------------------------------------------------

class TEeprom
{
private:
  static uint8_t const I2C_ADDR = 0xA0;   //����� ���������� EEPROM
  static uint16_t const EEPROM_WRTM = 25; //������������ ����� ������, ��
  static uint16_t const EE_SIGNATURE = 0xBEDA; //��������� EEPROM
  static bool SetAddress(uint16_t addr);
  static uint8_t ByteAddress;
  static uint8_t PageAddress;
  static bool fValid;
protected:
public:
  TEeprom(void);
  static uint16_t Read(uint16_t addr, uint16_t def = 0);
  static void Write(uint16_t addr, uint16_t data);
  static void Update(uint16_t addr, uint16_t data);
  static void Validate(void);
  static void Invalidate(void);
};

//----------------------------------------------------------------------------

extern TEeprom *Eeprom;

//----------------------------------------------------------------------------

#endif
