//----------------------------------------------------------------------------

//Serial port support class

//----------------------------------------------------------------------------

#include "main.hpp"
#include "port.hpp"
#include "process.hpp"
#include "control.hpp"
#include "bright.hpp"

//----------------------------- Constants: -----------------------------------

static uint8_t const DevName[] = { "Meter V"VERSION }; //device name

//----------------------------------------------------------------------------
//------------------------------ TPort class ---------------------------------
//----------------------------------------------------------------------------

//----------------------------- Constructor: ---------------------------------

TPort::TPort(void)
{
  WakePort = new TWakePort(BAUD_RATE, FRAME_SIZE);
  Command = CMD_NOP;
  Addr = new uint8_t; //TODO: heap profiling
}

//----------------------------------------------------------------------------
//--------------------------- Execute commands: ------------------------------
//----------------------------------------------------------------------------

void TPort::Execute(void)
{
  if(Command == CMD_NOP)          //if the command is not in progress,
    Command = WakePort->GetCmd(); //then reading the new command code
  if(Command != CMD_NOP)          //if the command is received, then the execution
  {
    switch(Command)
    {

//------------------------- Standard commands: -------------------------------

    case CMD_ERR: //error handling
      {
        WakePort->AddByte(ERR_TX);
        break;
      }
    case CMD_ECHO: //echo
      {
        uint8_t cnt = WakePort->GetRxCount();
        for(uint8_t i = 0; i < cnt; i++)
          WakePort->AddByte(WakePort->GetByte());
        break;
      }
    case CMD_INFO: //read device information
      {
        uint8_t i = 0;
        while(DevName[i])
          WakePort->AddByte(DevName[i++]);
        break;
      }

//--------------------------- Special commands: ------------------------------

    case CMD_SET_PRESET: //preset number set
      {
        Control->ChangePreset(WakePort->GetByte());
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_PRESET: //preset number get
      {
        WakePort->AddByte(ERR_NO);
        WakePort->AddByte(Control->GetPreset());
        break;
      }
    case CMD_SET_MODE: //display mode set
      {
        Control->SetDispMode(WakePort->GetByte());
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_MODE: //display mode get
      {
        WakePort->AddByte(ERR_NO);
        WakePort->AddByte(Control->GetDispMode());
        break;
      }
    case CMD_GET_METER: //meter readings get
      {
        if(!Control->Upd) return; //data not ready
        uint8_t m = Control->GetDispMode();
        WakePort->AddByte(ERR_NO);
        //WakePort->AddByte(m);
        if(m & DP_BAR)
        {
          WakePort->AddWord(Control->GetBarL());
          WakePort->AddWord(Control->GetBarR());
        }
        if(m & DP_PEAK)
        {
          WakePort->AddWord(Control->GetPeakL());
          WakePort->AddWord(Control->GetPeakR());
        }
        if(m & DP_BARMAX)
        {
          WakePort->AddWord(Control->GetBarMaxL());
          WakePort->AddWord(Control->GetBarMaxR());
        }
        if(m & DP_PEAKMAX)
        {
          WakePort->AddWord(Control->GetPeakMaxL());
          WakePort->AddWord(Control->GetPeakMaxR());
        }
        Control->Upd = 0;
        break;
      }
    case CMD_RES_STAT:   //statistics reset
      {
        Control->StatReset();
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_SET_PARAMS: //parameters set
      {
        TProcess::Integ1_t = WakePort->GetWord();
        TProcess::Decay1_t = WakePort->GetWord();
        TProcess::Scale1_f = WakePort->GetWord();
        TProcess::Resp1_t  = WakePort->GetWord();
        TProcess::Fall1_t  = WakePort->GetWord();
        TProcess::Hold1_t  = WakePort->GetWord();
        TProcess::MFall1_t = WakePort->GetWord();
        TProcess::MHold1_t = WakePort->GetWord();

        TProcess::Integ2_t = WakePort->GetWord();
        TProcess::Decay2_t = WakePort->GetWord();
        TProcess::Scale2_f = WakePort->GetWord();
        TProcess::Resp2_t  = WakePort->GetWord();
        TProcess::Fall2_t  = WakePort->GetWord();
        TProcess::Hold2_t  = WakePort->GetWord();
        TProcess::MFall2_t = WakePort->GetWord();
        TProcess::MHold2_t = WakePort->GetWord();

        TProcess::ApplyParams();
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_PARAMS: //parameters get
      {
        WakePort->AddByte(ERR_NO);
        WakePort->AddWord(TProcess::Integ1_t);
        WakePort->AddWord(TProcess::Decay1_t);
        WakePort->AddWord(TProcess::Scale1_f);
        WakePort->AddWord(TProcess::Resp1_t);
        WakePort->AddWord(TProcess::Fall1_t);
        WakePort->AddWord(TProcess::Hold1_t);
        WakePort->AddWord(TProcess::MFall1_t);
        WakePort->AddWord(TProcess::MHold1_t);

        WakePort->AddWord(TProcess::Integ2_t);
        WakePort->AddWord(TProcess::Decay2_t);
        WakePort->AddWord(TProcess::Scale2_f);
        WakePort->AddWord(TProcess::Resp2_t);
        WakePort->AddWord(TProcess::Fall2_t);
        WakePort->AddWord(TProcess::Hold2_t);
        WakePort->AddWord(TProcess::MFall2_t);
        WakePort->AddWord(TProcess::MHold2_t);
        break;
      }
    case CMD_SET_TABLE: //table load
      {
        for(int8_t i = 0; i < SEGS; i++)
          Control->SetTab(i, WakePort->GetWord());
        Control->ApplyTab();
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_TABLE: //table read
      {
        WakePort->AddByte(ERR_NO);
        for(int8_t i = 0; i < SEGS; i++)
          WakePort->AddWord(Control->GetTab(i));
        break;
      }
    case CMD_SET_LEDS: //LEDs control
      {
        Bright->SetMode(WakePort->GetByte());
        Bright->SetMeter(WakePort->GetByte());
        Bright->SetScale(WakePort->GetByte());
        Control->SetLeds(WakePort->GetWord());
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_GET_LEDS: //LEDs read
      {
        WakePort->AddByte(ERR_NO);
        WakePort->AddByte(Bright->GetMode());
        WakePort->AddByte(Bright->GetMeter());
        WakePort->AddByte(Bright->GetScale());
        WakePort->AddWord(Control->GetLeds());
        break;
      }
    case CMD_EE_SAVE: //saving parameters in EEPROM
      {
        if(Control->EESave())
          WakePort->AddByte(ERR_EE);
            else WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_DEFAULT: //load defaults
      {
        Control->LoadDefaults();
        WakePort->AddByte(ERR_NO);
        break;
      }
    case CMD_TEST: //test command
      {
        WakePort->AddByte(ERR_NO);
        WakePort->AddDWord((uint32_t)Addr);
        break;
      }
    default: //unknown command
      {
        WakePort->AddByte(ERR_PA);
      }
    }
    WakePort->StartTx(Command);
    Command = CMD_NOP; //command completed
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
