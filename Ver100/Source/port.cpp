//----------------------------------------------------------------------------

//������ ��������� �����

//----------------------------------------------------------------------------

#include "main.hpp"
#include "port.hpp"
#include "process.hpp"
#include "control.hpp"

//----------------------------------------------------------------------------
//------------------------------ ����� TPort ---------------------------------
//----------------------------------------------------------------------------

//----------------------------- �����������: ---------------------------------

TPort::TPort(void)
{
  WakePort = new TWakePort(BAUD_RATE, FRAME_SIZE);
  Command = CMD_NOP;
}

//----------------------------------------------------------------------------
//-------------------------- ���������� ������: ------------------------------
//----------------------------------------------------------------------------

void TPort::Execute(void)
{
  if(Command == CMD_NOP)          //���� ������� �� � ������ ����������,
    Command = WakePort->GetCmd(); //�� ������ ������ ���� �������
  if(Command != CMD_NOP)          //���� ���� �������, ����������
  {
    switch(Command)
    {

//------------------------ ����������� �������: ------------------------------

    case CMD_ERR: //��������� ������
      {
        WakePort->AddByte(ERR_TX);
        break;
      }
    case CMD_ECHO: //���
      {
        char cnt = WakePort->GetRxCount();
        for(char i = 0; i < cnt; i++)
          WakePort->AddByte(WakePort->GetByte());
        break;
      }
    case CMD_INFO: //������ ��������� �� ����������
      {
        char Info[] = {DEVICE_NAME};
        char *s = Info;
        do WakePort->AddByte(*s++);
          while(*s);
        break;
      }

//------------------------- ����������� �������: -----------------------------

    case CMD_SET_MODE: //��������� ������ (�������)
      {
        //TODO: Control->SetMode(WakePort->GetByte());
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_MODE: //������ ������ (�������)
      {
        WakePort->AddByte(ERR_NO);
        //TODO: WakePort->AddByte(Control->GetMode());
        break;
      }
    case CMD_GET_METER: //������ ��������� (BarL, BarR, PeakL, PeakR)
      {
        if(!Control->Upd) return; //������ �� ������
        WakePort->AddByte(ERR_NO);
        WakePort->AddWord(Control->GetBarL());
        WakePort->AddWord(Control->GetBarR());
        WakePort->AddWord(Control->GetPeakL());
        WakePort->AddWord(Control->GetPeakR());
        Control->Upd = 0;
        break;
      }
    case CMD_GET_STAT:   //������ ���������� (BarL, BarR, PeakL, PeakR)
      {
        WakePort->AddByte(ERR_NO);
        //TODO: WakePort->AddWord(Control->GetMaxBarL());
        //TODO: WakePort->AddWord(Control->GetMaxBarR());
        //TODO: WakePort->AddWord(Control->GetMaxPeakL());
        //TODO: WakePort->AddWord(Control->GetMaxPeakR());
        break;
      }
    case CMD_RES_STAT:   //����� ����������
      {
        //TODO: Control->StatReset();
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_SET_PARAMS: //��������� ����������
      {
        TProcess::SetINF1_a(WakePort->GetWord()); //Int1
        TProcess::SetINF2_a(WakePort->GetWord()); //Int2
        TProcess::SetDEF1_a(WakePort->GetWord()); //Dec1
        TProcess::SetDEF2_a(WakePort->GetWord()); //Dec2
        TProcess::SetRSF1_a(WakePort->GetWord()); //Res1
        TProcess::SetRSF2_a(WakePort->GetWord()); //Res2
        TProcess::SetFLF1_a(WakePort->GetWord()); //Fal1
        TProcess::SetFLF2_a(WakePort->GetWord()); //Fal2
        TProcess::SetSCL1_a(WakePort->GetWord()); //Scl1
        TProcess::SetSCL2_a(WakePort->GetWord()); //Scl2
        TProcess::SetHLD1_a(WakePort->GetWord()); //Hld1
        TProcess::SetHLD2_a(WakePort->GetWord()); //Hld2
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_PARAMS: //������ ����������
      {
        WakePort->AddByte(ERR_NO);
        WakePort->AddWord(TProcess::GetINF1_a()); //Int1
        WakePort->AddWord(TProcess::GetINF2_a()); //Int2
        WakePort->AddWord(TProcess::GetDEF1_a()); //Dec1
        WakePort->AddWord(TProcess::GetDEF2_a()); //Dec2
        WakePort->AddWord(TProcess::GetRSF1_a()); //Res1
        WakePort->AddWord(TProcess::GetRSF2_a()); //Res2
        WakePort->AddWord(TProcess::GetFLF1_a()); //Fal1
        WakePort->AddWord(TProcess::GetFLF2_a()); //Fal2
        WakePort->AddWord(TProcess::GetSCL1_a()); //Scl1
        WakePort->AddWord(TProcess::GetSCL2_a()); //Scl2
        WakePort->AddWord(TProcess::GetHLD1_a()); //Hld1
        WakePort->AddWord(TProcess::GetHLD2_a()); //Hld2
        break;
      }
    case CMD_SET_TABLE: //�������� �������
      {
        for(int8_t i = 0; i < SEGS; i++)
          Control->SetTab(i, WakePort->GetWord());
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_TABLE: //������ �������
      {
        WakePort->AddByte(ERR_NO);
        for(int8_t i = 0; i < SEGS; i++)
          WakePort->AddWord(Control->GetTab(i));
        break;
      }
    case CMD_EE_SAVE: //���������� ���������� � EEPROM
      {
        Control->EESave();
        WakePort->AddByte(ERR_NO);
        break;
      }
    default: //����������� �������
      {
        WakePort->AddByte(ERR_PA);
      }
    }
    WakePort->StartTx(Command);
    Command = CMD_NOP; //������� ���������
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
