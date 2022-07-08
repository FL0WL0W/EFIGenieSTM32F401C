#include "usbd_cdc_if.h"
#include "EmbeddedIOServiceCollection.h"
#include "Stm32HalDigitalService.h"
#include "Stm32HalTimerService.h"
#include "Stm32HalAnalogService.h"
#include "Stm32HalPwmService.h"
#include "EFIGenieMain.h"
#include "Variable.h"
#include "CallBack.h"
#include "Config.h"
#include "STM32HalCommunicationService_CDC.h"
#include "CommunicationHandlers/CommunicationHandler_GetVariable.h"

using namespace OperationArchitecture;
using namespace EmbeddedIOServices;
using namespace EmbeddedIOOperations;
using namespace Stm32;
using namespace EFIGenie;

extern char _config;


extern "C"
{
  uint8_t VariableBuff[9];
  uint32_t Commands[32];
  uint8_t CommandReadPointer = 0;
  bool secondCommand = false;
  EmbeddedIOServiceCollection _embeddedIOServiceCollection;
  STM32HalCommunicationService_CDC *_cdcService;
  CommunicationHandler_GetVariable *_getVariableHandler;
  EFIGenieMain *_engineMain;
  Variable *loopTime;
  uint32_t prev;
  GeneratorMap<Variable> _variableMap;

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

    _cdcService = new STM32HalCommunicationService_CDC();

    const char responseText1[34] = "Initializing EmbeddedIOServices\n\r";
    _cdcService->Send((uint8_t*)responseText1, strlen(responseText1));
    _cdcService->Flush();
    _embeddedIOServiceCollection.DigitalService = new Stm32HalDigitalService();
    _embeddedIOServiceCollection.AnalogService = new Stm32HalAnalogService();
    _embeddedIOServiceCollection.TimerService = new Stm32HalTimerService(TimerIndex::Index2);
    _embeddedIOServiceCollection.PwmService = new Stm32HalPwmService();
    const char responseText2[33] = "EmbeddedIOServices Initialized\n\r";
    _cdcService->Send((uint8_t*)responseText2, strlen(responseText2));
    _cdcService->Flush();

    const char responseText3[26] = "Initializing EngineMain\n\r";
    _cdcService->Send((uint8_t*)responseText3, strlen(responseText3));
    _cdcService->Flush();
		size_t _configSize = 0;
    _engineMain = new EFIGenieMain(reinterpret_cast<void*>(&_config), _configSize, &_embeddedIOServiceCollection, &_variableMap);
    const char responseText4[25] = "EngineMain Initialized\n\r";
    _cdcService->Send((uint8_t*)responseText4, strlen(responseText4));
    _cdcService->Flush();

    const void *metadata = Config::OffsetConfig(&_config, _configSize);
    _getVariableHandler = new CommunicationHandler_GetVariable(&_variableMap);
    _cdcService->RegisterHandler(_getVariableHandler);

    const char responseText5[24] = "Setting Up EngineMain\n\r";
    _cdcService->Send((uint8_t*)responseText5, strlen(responseText5));
    _cdcService->Flush();
    _engineMain->Setup();
    const char responseText6[19] = "EngineMain Setup\n\r";
    _cdcService->Send((uint8_t*)responseText6, strlen(responseText6));
    _cdcService->Flush();
    loopTime = _variableMap.GenerateValue(250);
  }
  void Loop() 
  {
    const tick_t now = _embeddedIOServiceCollection.TimerService->GetTick();
    loopTime->Set((float)(now-prev) / _embeddedIOServiceCollection.TimerService->GetTicksPerSecond());
    prev = now;
    _cdcService->Flush();
    _engineMain->Loop();
  }
}