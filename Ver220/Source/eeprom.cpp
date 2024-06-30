//----------------------------------------------------------------------------

//External EEPROM (24Ñ16) support class

//-------------------------- Resources Used: ---------------------------------

//An external EEPROM chip 24C16 is used, which is connected to the SCL (PB8),
//SDA (PB9) pins. The I2C interface is emulated in software.
//Unfortunately, hardware I2C1 (remap) cannot be used, as it conflicts with SPI1
//(remap): the PB5 pin is always 1 (see errata).
//The TIM16 timer is used to generate bus timings.
//When writing the address, the ASK response is checked.
//If there is no response, repeated attempts are made to write the address.
//If no response is received within the EEPROM_WRTM timeout (this is the
//maximum EEPROM write time), then an error flag is returned.

//----------------------------------------------------------------------------

#include "main.hpp"
#include "i2csw.hpp"
#include "eeprom.hpp"
#include "eemap.hpp"

//----------------------------------------------------------------------------
//----------------------------- TEeprom class: -------------------------------
//----------------------------------------------------------------------------

//------------------------------ Constructor: --------------------------------

TEeprom::TEeprom(void)
{
  TI2Csw::Init();
  Error = 0;
  SelectPreset(0);
}

//---------------- Write address and wait for EEPROM ready: ------------------

//addr - word address.
//Returns true if an EEPROM response is received.

bool TEeprom::SetAddress(uint8_t pre, uint16_t addr)
{
  bool ask;
  uint16_t a = (uint16_t)pre * EE_PRE_SIZE + addr;
  ByteAddress = (a << 1) & 0xFE;
  PageAddress = (a >> 6) & 0x0E;
  TSysTimer::TimeoutStart_ms(EEPROM_WRTM);
  do
  {
    TI2Csw::Start();
    ask = TI2Csw::Write(I2C_ADDR | PageAddress);
  }
  while(!ask && !TSysTimer::TimeoutOver_ms());
  if(ask)
  {
    TI2Csw::Write(ByteAddress);
  }
  else
  {
    TI2Csw::Stop();
  }
  return(ask);
}

//----------------------------------------------------------------------------
//-------------------------- Interface Methods: ------------------------------
//----------------------------------------------------------------------------

//------------------------- Preset number select: ----------------------------

//All read-write operations will be performed only inside the EEPROM section
//belonging to this preset. Validity is set or reset individually for each section.

void TEeprom::SelectPreset(uint8_t p)
{
  if(p < EE_PREN)
  {
    Preset = p;
    fValid = (Rd16(EE_SIGN) == EE_SIGNATURE);
  }
}

//---------------------- Reading data from EEPROM: ---------------------------

//addr - word address inside the EEPROM section of current preset.
//Returns data word from EEPROM for current preset.

uint16_t TEeprom::Rd16(uint16_t addr)
{
  if(SetAddress(Preset, addr))
  {
    TI2Csw::Stop();
    TI2Csw::Start();
    TI2Csw::Write(I2C_ADDR | PageAddress | I2C_RD);
    uint8_t data_l = TI2Csw::Read(I2C_ACK);
    uint8_t data_h = TI2Csw::Read(I2C_NACK);
    return(WORD(data_h, data_l));
  }
  return(0);
}

//------------------------ Write data to EEPROM: -----------------------------

//addr - word address inside the EEPROM section of current preset.
//data - data word for write to the EEPROM section.

void TEeprom::Wr16(uint16_t addr, uint16_t data)
{
  if(SetAddress(Preset, addr))
  {
    TI2Csw::Write(LO(data));
    TI2Csw::Write(HI(data));
    TI2Csw::Stop();
  }
}

//------------------- Reading data with validity check: ----------------------

//addr - word address inside the EEPROM section of current preset.
//def - default value.
//Returns data word from EEPROM for current preset.
//If the EEPROM section is not valid or read error occurs,
//returns def value.

uint16_t TEeprom::Read(uint16_t addr, uint16_t def)
{
  if(fValid) return(Rd16(addr)); return(def);
}

//---------------------- Write data with update check: -----------------------

//addr - word address inside the EEPROM section of current preset.
//data - data word for write to the EEPROM section.
//A write is only made if the new data is different.

void TEeprom::Write(uint16_t addr, uint16_t data)
{
  if(Rd16(addr) != data) Wr16(addr, data);
  if(Rd16(addr) != data) Error = 1;
}

//--------------------------- Write signature: -------------------------------

//Writing a signature for the EEPROM section of the current preset.

void TEeprom::Validate(void)
{
  Write(EE_SIGN, EE_SIGNATURE);
  fValid = (Rd16(EE_SIGN) == EE_SIGNATURE);
}

//------------------------- Validity flag reset: -----------------------------

//Reset the validity flag for the current preset.

void TEeprom::Invalidate(void)
{
  fValid = 0;
}

//----------------------- Reading preset number: -----------------------------

uint8_t TEeprom::ReadP(void)
{
  if(SetAddress(EE_PREN, 0))
  {
    TI2Csw::Stop();
    TI2Csw::Start();
    TI2Csw::Write(I2C_ADDR | PageAddress | I2C_RD);
    uint8_t p = TI2Csw::Read(I2C_NACK);
    if(p > EE_PRE4) p = 0;
    return(p);
  }
  return(0);
}

//------------------------ Write preset number: ------------------------------

void TEeprom::WriteP(uint8_t p)
{
  if(SetAddress(EE_PREN, 0))
  {
    TI2Csw::Write(p);
    TI2Csw::Stop();
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
