//----------------------------------------------------------------------------

//Serial register support class, header file

//----------------------------------------------------------------------------

#ifndef SREG_HPP
#define SREG_HPP

//----------------------------------------------------------------------------
//----------------------------- TSreg class: ---------------------------------
//----------------------------------------------------------------------------

class TSreg
{
private:
  Pin_SCLK_t Pin_SCLK;
  Pin_SDATA_t Pin_SDATA;
  Pin_LOAD_t Pin_LOAD;
  Pin_OE_t Pin_OE;
  uint8_t Regs;
  void SpiWr(uint8_t d);
public:
  TSreg() {};
  void Init(uint8_t n);      //SPI init
  void Load(uint8_t *Value); //registers load
  void Enable(bool en);      //enable/disable registers outputs
};

//----------------------------------------------------------------------------

#endif
