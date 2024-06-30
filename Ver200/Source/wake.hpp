//----------------------------------------------------------------------------

//����� ��������� ��������� Wake, ������������ ����

//----------------------------------------------------------------------------

#ifndef WAKE_HPP
#define WAKE_HPP

//----------------------------- ���������: -----------------------------------

//���� ����������� ������:
enum
{
  CMD_NOP,     //��� ��������
  CMD_ERR,     //������ ������ ������
  CMD_ECHO,    //���
  CMD_INFO,    //������ ���������� �� ����������
  CMD_SETADDR, //��������� ������
  CMD_GETADDR  //������ ������
};

//���� ������:
enum
{
  ERR_NO,      //no error
  ERR_TX,      //Rx/Tx error
  ERR_BU,      //device busy error
  ERR_RE,      //device not ready error
  ERR_PA,      //parameters value error
  ERR_NR,      //no replay
  ERR_NC       //no carrier
};

//----------------------------------------------------------------------------
//------------------------ ����� ��������� WAKE: -----------------------------
//----------------------------------------------------------------------------

class TWake
{
private:
  enum WPnt_t
  {
    PTR_ADD,       //�������� � ������ ��� ������
    PTR_CMD,       //�������� � ������ ��� ���� �������
    PTR_LNG,       //�������� � ������ ��� ����� ������
    PTR_DAT        //�������� � ������ ��� ������
  };
  enum WStuff_t
  {
    FEND  = 0xC0,  //Frame END
    FESC  = 0xDB,  //Frame ESCape
    TFEND = 0xDC,  //Transposed Frame END
    TFESC = 0xDD   //Transposed Frame ESCape
  };
  enum WState_t
  {
    WST_IDLE,      //��������� ��������
    WST_ADD,       //����� ������
    WST_CMD,       //����� �������
    WST_LNG,       //����� ����� ������
    WST_DATA,      //�����/�������� ������
    WST_CRC,       //�����/�������� CRC
    WST_DONE       //��������� ����������
  };

  uint8_t Addr;    //����� ����������
  uint8_t RxState; //��������� �������� ������
  bool RxStuff;    //������� ��������� ��� ������
  uint8_t *RxPtr;  //��������� ������ ������
  uint8_t *RxEnd;  //�������� ��������� ����� ������ ������
  uint8_t RxCount; //���������� �������� ����
  uint8_t *RxData; //����� ������

  uint8_t TxState; //��������� �������� ��������
  bool TxStuff;    //������� ��������� ��� ��������
  uint8_t *TxPtr;  //��������� ������ ��������
  uint8_t *TxEnd;  //�������� ��������� ����� ������ ��������
  uint8_t TxCount; //���������� ������������ ����
  uint8_t *TxData; //����� ��������

  uint8_t Frame;
  void Do_Crc8(uint8_t b, uint8_t *crc); //���������� ����������� �����
  static const uint8_t CRC_INIT = 0xDE;  //��������� �������� ����������� �����
  static const uint8_t CRC_FEND = 0x82;  //�� ��, �� � ������ FEND
protected:
  void Rx(uint8_t data);     //����� �����
  bool Tx(uint8_t &data);    //�������� �����
public:
  TWake(uint8_t frame);
  uint8_t GetCmd(void);      //���������� ������� ��� �������
  uint8_t GetRxCount(void);  //���������� ���������� �������� ����
  void SetRxPtr(uint8_t p);  //������������� ��������� ������ ������
  uint8_t GetRxPtr(void);    //������ ��������� ������ ������
  uint8_t GetByte(void);     //������ ���� �� ������ ������
  int16_t GetWord(void);     //������ ����� �� ������ ������
  int32_t GetDWord(void);    //������ ������� ����� �� ������ ������
  void GetData(uint8_t *d, uint8_t count); //������ ������ �� ������ ������

  void SetTxPtr(uint8_t p);  //������������� ��������� ������ ��������
  uint8_t GetTxPtr(void);    //������ ��������� ������ ��������
  void AddByte(uint8_t b);   //�������� ���� � ����� ��������
  void AddWord(int16_t w);   //�������� ����� � ����� ��������
  void AddDWord(int32_t dw); //�������� ������� ����� � ����� ��������
  void AddData(uint8_t *d, uint8_t count); //�������� ������ � ����� ��������
  void TxStart(uint8_t cmd, uint8_t &data); //������ �������� ������
  bool AskTxEnd(void);       //����������� ����� �������� ������
};

//----------------------------------------------------------------------------

#endif
