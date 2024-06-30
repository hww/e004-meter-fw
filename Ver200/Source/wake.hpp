//----------------------------------------------------------------------------

//Класс релизации протокола Wake, заголовочный файл

//----------------------------------------------------------------------------

#ifndef WAKE_HPP
#define WAKE_HPP

//----------------------------- Константы: -----------------------------------

//Коды стандартных команд:
enum
{
  CMD_NOP,     //нет операции
  CMD_ERR,     //ошибка приема пакета
  CMD_ECHO,    //эхо
  CMD_INFO,    //чтение информации об устройстве
  CMD_SETADDR, //установка адреса
  CMD_GETADDR  //чтение адреса
};

//Коды ошибок:
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
//------------------------ Класс протокола WAKE: -----------------------------
//----------------------------------------------------------------------------

class TWake
{
private:
  enum WPnt_t
  {
    PTR_ADD,       //смещение в буфере для адреса
    PTR_CMD,       //смещение в буфере для кода команды
    PTR_LNG,       //смещение в буфере для длины пакета
    PTR_DAT        //смещение в буфере для данных
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
    WST_IDLE,      //состояние ожидания
    WST_ADD,       //прием адреса
    WST_CMD,       //прием команды
    WST_LNG,       //прием длины пакета
    WST_DATA,      //прием/передача данных
    WST_CRC,       //прием/передача CRC
    WST_DONE       //состояние готовности
  };

  uint8_t Addr;    //адрес устройства
  uint8_t RxState; //состояние процесса приема
  bool RxStuff;    //признак стаффинга при приеме
  uint8_t *RxPtr;  //указатель буфера приема
  uint8_t *RxEnd;  //значение указателя конца буфера приема
  uint8_t RxCount; //количество принятых байт
  uint8_t *RxData; //буфер приема

  uint8_t TxState; //состояние процесса передачи
  bool TxStuff;    //признак стаффинга при передаче
  uint8_t *TxPtr;  //указатель буфера передачи
  uint8_t *TxEnd;  //значение указателя конца буфера передачи
  uint8_t TxCount; //количество передаваемых байт
  uint8_t *TxData; //буфер передачи

  uint8_t Frame;
  void Do_Crc8(uint8_t b, uint8_t *crc); //вычисление контрольной суммы
  static const uint8_t CRC_INIT = 0xDE;  //начальное значение контрольной суммы
  static const uint8_t CRC_FEND = 0x82;  //то же, но с учетом FEND
protected:
  void Rx(uint8_t data);     //прием байта
  bool Tx(uint8_t &data);    //передача байта
public:
  TWake(uint8_t frame);
  uint8_t GetCmd(void);      //возвращает текущий код команды
  uint8_t GetRxCount(void);  //возвращает количество принятых байт
  void SetRxPtr(uint8_t p);  //устанавливает указатель буфера приема
  uint8_t GetRxPtr(void);    //читает указатель буфера приема
  uint8_t GetByte(void);     //читает байт из буфера приема
  int16_t GetWord(void);     //читает слово из буфера приема
  int32_t GetDWord(void);    //читает двойное слово из буфера приема
  void GetData(uint8_t *d, uint8_t count); //читает данные из буфера приема

  void SetTxPtr(uint8_t p);  //устанавливает указатель буфера передачи
  uint8_t GetTxPtr(void);    //читает указатель буфера передачи
  void AddByte(uint8_t b);   //помещает байт в буфер передачи
  void AddWord(int16_t w);   //помещает слово в буфер передачи
  void AddDWord(int32_t dw); //помещает двойное слово в буфер передачи
  void AddData(uint8_t *d, uint8_t count); //помещает данные в буфер передачи
  void TxStart(uint8_t cmd, uint8_t &data); //начало передачи пакета
  bool AskTxEnd(void);       //определение конца передачи пакета
};

//----------------------------------------------------------------------------

#endif
