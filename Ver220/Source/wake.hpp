//----------------------------------------------------------------------------

//Wake protocol class, header file

//----------------------------------------------------------------------------

#ifndef WAKE_HPP
#define WAKE_HPP

//----------------------------- Constants: -----------------------------------

//Standard command codes:
enum
{
  CMD_NOP,     //no operation
  CMD_ERR,     //packet receive error
  CMD_ECHO,    //echo
  CMD_INFO,    //read device information
  CMD_SETADDR, //address set
  CMD_GETADDR  //address get
};

//Error codes:
enum
{
  ERR_NO,      //no error
  ERR_TX,      //Rx/Tx error
  ERR_BU,      //device busy error
  ERR_RE,      //device not ready error
  ERR_PA,      //parameters value error
  ERR_NR,      //no replay
  ERR_NC,      //no carrier
  ERR_EE       //EEPROM error
};

//----------------------------------------------------------------------------
//------------------------ Wake protocol class: ------------------------------
//----------------------------------------------------------------------------

class TWake
{
private:
  enum WPnt_t
  {
    PTR_ADD,       //buffer offset for address
    PTR_CMD,       //buffer offset for command code
    PTR_LNG,       //buffer offset for packet length
    PTR_DAT        //buffer offset for data
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
    WST_IDLE,      //idle state
    WST_ADD,       //receiving address
    WST_CMD,       //receiving command
    WST_LNG,       //receiving packet length
    WST_DATA,      //receiving/transmitting data
    WST_CRC,       //receiving/transmitting CRC
    WST_DONE       //ready state
  };

  uint8_t Addr;    //device address
  uint8_t RxState; //receive process state
  bool RxStuff;    //stuffing at the reception flag
  uint8_t *RxPtr;  //receive buffer pointer
  uint8_t *RxEnd;  //receive buffer end pointer value
  uint8_t RxCount; //number of bytes received
  uint8_t *RxData; //receive buffer

  uint8_t TxState; //transmit process state
  bool TxStuff;    //stuffing at the transmitting flag
  uint8_t *TxPtr;  //transmit buffer pointer
  uint8_t *TxEnd;  //transmit buffer end pointer value
  uint8_t TxCount; //number of bytes to transmit
  uint8_t *TxData; //transmit buffer

  uint8_t Frame;
  void Do_Crc8(uint8_t b, uint8_t *crc); //CRC calculation
  static const uint8_t CRC_INIT = 0xDE;  //initial CRC value
  static const uint8_t CRC_FEND = 0x82;  //the same, but with FEND
protected:
  void Rx(uint8_t data);     //receive byte
  bool Tx(uint8_t &data);    //transmit byte
public:
  TWake(uint8_t frame);
  uint8_t GetCmd(void);      //returns the current command code
  uint8_t GetRxCount(void);  //returns the number of bytes received
  void SetRxPtr(uint8_t p);  //sets the receive buffer pointer
  uint8_t GetRxPtr(void);    //reads the receive buffer pointer
  uint8_t GetByte(void);     //reads a byte from the receive buffer
  int16_t GetWord(void);     //reads a word from the receive buffer
  int32_t GetDWord(void);    //reads a double word from the receive buffer
  void GetData(uint8_t *d, uint8_t count); //reads data from the receive buffer

  void SetTxPtr(uint8_t p);  //sets the transmit buffer pointer
  uint8_t GetTxPtr(void);    //reads the transmit buffer pointer
  void AddByte(uint8_t b);   //puts a byte in the transmit buffer
  void AddWord(int16_t w);   //puts a word in the send buffer
  void AddDWord(int32_t dw); //puts a double word in the transmit buffer
  void AddData(uint8_t *d, uint8_t count); //puts the data in the transmit buffer
  void TxStart(uint8_t cmd, uint8_t &data); //packet transmit start
  bool AskTxEnd(void);       //determining the end of a packet transmission
};

//----------------------------------------------------------------------------

#endif
