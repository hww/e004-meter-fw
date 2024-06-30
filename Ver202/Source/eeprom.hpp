//----------------------------------------------------------------------------

//Класс поддержки внешней EEPROM, заголовочный файл

//----------------------------------------------------------------------------

#ifndef EEPROM_H
#define EEPROM_H

//----------------------------------------------------------------------------
//---------------------------- Класс TEeprom: --------------------------------
//----------------------------------------------------------------------------

class TEeprom
{
private:
  static const uint8_t I2C_ADDR = 0xA0;   //адрес микросхемы EEPROM
  static const uint16_t EEPROM_WRTM = 25; //длительность цикла записи, мс
  bool SetAddress(uint8_t pre, uint16_t addr);
  uint8_t ByteAddress;
  uint8_t PageAddress;
  bool fValid;
  uint8_t Preset;
protected:
public:
  TEeprom(void);
  bool Error;
  void SelectPreset(uint8_t p);
  uint16_t Rd16(uint16_t addr);
  void Wr16(uint16_t addr, uint16_t data);
  uint16_t Read(uint16_t addr, uint16_t def = 0);
  void Write(uint16_t addr, uint16_t data);
  void Validate(void);
  void Invalidate(void);
};

//----------------------------------------------------------------------------

extern TEeprom *Eeprom;

//----------------------------------------------------------------------------

#endif
