#include "usbd_cdc_if.h"
#include "EmbeddedIOServiceCollection.h"
#include "Stm32HalDigitalService.h"
#include "Stm32HalTimerService.h"
#include "Stm32HalAnalogService.h"
#include "Stm32HalPwmService.h"
#include "EngineMain.h"
#include "Variable.h"
#include <functional>

using namespace OperationArchitecture;
using namespace EmbeddedIOServices;
using namespace Stm32;
using namespace Engine;

extern char _config;

extern "C"
{
  uint32_t Commands[32];
  uint8_t CommandReadPointer = 0;
  bool secondCommand = false;
  EmbeddedIOServiceCollection _embeddedIOServiceCollection;
  EngineMain *_engineMain;
  Variable *loopTime;
  uint32_t prev;
  
  bool val = true;
  void test() {
      _embeddedIOServiceCollection.DigitalService->WritePin(45, val);
      val = !val;
  }
  Task t = Task(&test);

  void Setup() 
  {
    //ignore the warnings. these are here so we can check the alignment so we can put it in the editor
    volatile size_t uint8_align = alignof(uint8_t);
    volatile size_t uint16_align = alignof(uint16_t);
    volatile size_t uint32_align = alignof(uint32_t);
    volatile size_t uint64_align = alignof(uint64_t);
    volatile size_t bool_align = alignof(bool);
    volatile size_t int8_align = alignof(int8_t);
    volatile size_t int16_align = alignof(int16_t);
    volatile size_t int32_align = alignof(int32_t);
    volatile size_t int64_align = alignof(int64_t);
    volatile size_t float_align = alignof(float);
    volatile size_t double_align = alignof(double);

    const char responseText1[34] = "Initializing EmbeddedIOServices\n\r";
    CDC_Transmit_FS((uint8_t*)responseText1, strlen(responseText1));
    _embeddedIOServiceCollection.DigitalService = new Stm32HalDigitalService();
    _embeddedIOServiceCollection.AnalogService = new Stm32HalAnalogService();
    _embeddedIOServiceCollection.TimerService = new Stm32HalTimerService(TimerIndex::Index1);
    _embeddedIOServiceCollection.PwmService = new Stm32HalPwmService();
    const char responseText2[33] = "EmbeddedIOServices Initialized\n\r";
    CDC_Transmit_FS((uint8_t*)responseText2, strlen(responseText2));

    _embeddedIOServiceCollection.DigitalService->InitPin(45, Out);
    _embeddedIOServiceCollection.TimerService->ScheduleTask(&t, _embeddedIOServiceCollection.TimerService->GetTick() + _embeddedIOServiceCollection.TimerService->GetTicksPerSecond());

    const char responseText3[26] = "Initializing EngineMain\n\r";
    CDC_Transmit_FS((uint8_t*)responseText3, strlen(responseText3));
		unsigned int _configSize = 0;
    _engineMain = new EngineMain(reinterpret_cast<void*>(&_config), _configSize, &_embeddedIOServiceCollection);
    const char responseText4[25] = "EngineMain Initialized\n\r";
    CDC_Transmit_FS((uint8_t*)responseText4, strlen(responseText4));

    const char responseText5[24] = "Setting Up EngineMain\n\r";
    CDC_Transmit_FS((uint8_t*)responseText5, strlen(responseText5));
    _engineMain->Setup();
    const char responseText6[19] = "EngineMain Setup\n\r";
    CDC_Transmit_FS((uint8_t*)responseText6, strlen(responseText6));
    loopTime = _engineMain->SystemBus->GetOrCreateVariable(250);
  }
  void Loop() 
  {
    if(!t.Scheduled)
        _embeddedIOServiceCollection.TimerService->ScheduleTask(&t, t.Tick + _embeddedIOServiceCollection.TimerService->GetTicksPerSecond());

    if(Commands[CommandReadPointer] != 0)
    {
      std::map<uint32_t, Variable*>::iterator it = _engineMain->SystemBus->Variables.find(Commands[CommandReadPointer]);
      if (it != _engineMain->SystemBus->Variables.end())
      {
        if(it->second->Type == POINTER || it->second->Type == BIGOTHER)
        {
          if(Commands[CommandReadPointer + 1] != 0)
          {
            CDC_Transmit_FS(((uint8_t *)((uint64_t*)it->second->Value + (Commands[CommandReadPointer + 1] - 1))), sizeof(uint64_t));
            Commands[CommandReadPointer] = 0;
            CommandReadPointer++;
            CommandReadPointer++;
            CommandReadPointer %= 32;
            secondCommand = false;
          }
          else if(!secondCommand)
          {
            CDC_Transmit_FS((uint8_t*)&it->second->Type, sizeof(VariableType));
            secondCommand = true;
          }
        }
        else
        {
          CDC_Transmit_FS((uint8_t*)it->second, sizeof(Variable));
          Commands[CommandReadPointer] = 0;
          CommandReadPointer++;
          CommandReadPointer %= 32;
        }
      }
    }
    const uint32_t now = _embeddedIOServiceCollection.TimerService->GetTick();
    loopTime->Set((float)(now-prev) / _embeddedIOServiceCollection.TimerService->GetTicksPerSecond());
    prev = now;
    _engineMain->Loop();
  }
}