//----------------------------------------------------------------------------

//����� ��������� ����������������� ��������, ������������ ����

//----------------------------------------------------------------------------

#ifndef SREG_HPP
#define SREG_HPP

//----------------------------- ���������: -----------------------------------

//������ ���������:
enum { REG1,  REG2,  REG3,  REG4,  REG5,  REG6,  REG7,
       REG8,  REG9,  REG10, REG11, REG12, REG13, REG14, REGS };

//----------------------------------------------------------------------------
//----------------------------- ����� TSreg: ---------------------------------
//----------------------------------------------------------------------------

class TSreg
{
private:
  Pin_SCLK_t Pin_SCLK;
  Pin_SDATA_t Pin_SDATA;
  Pin_LOAD_t Pin_LOAD;
  Pin_OE_t Pin_OE;
  void SpiWr(uint8_t d);
public:
  TSreg(void) {};
  void Init(void);           //������������� SPI
  void Load(uint8_t *Value); //�������� ��������
  void Enable(bool en);      //����������/���������� ������� ��������
};

//----------------------------------------------------------------------------

#endif
