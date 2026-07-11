#include "usbd_cdc_if.h"
#include "ServiceRegistry.h"
#include "Stm32HalDigitalService.h"
#include "Stm32HalTimerService.h"
#include "Stm32HalAnalogService.h"
#include "Stm32HalPwmService.h"
#include "EngineMain.h"
#include "Variable.h"
#include "CallBack.h"
#include "Config.h"
#include "CommunicationHandler_Prefix.h"
#include "STM32HalCommunicationService_CDC.h"
#include "CommunicationHandlers/CommunicationHandler_EFIGenie.h"
#include "stm32f4xx_hal_flash_ex.h"

using namespace OperationArchitecture;
using namespace EmbeddedIOServices;
using namespace EmbeddedIOOperations;
using namespace Stm32;
using namespace EFIGenie;

extern char _config;


extern "C"
{
  GeneratorMap<Variable> *_variableMap;
  uint8_t VariableBuff[9];
  uint32_t Commands[32];
  uint8_t CommandReadPointer = 0;
  const char doneResponseText[10] = " (Done)\n\r";
  const void *_metadata;
  bool secondCommand = false;
  ServiceRegistry _serviceRegistry;
  IDigitalService *_digitalService;
  IAnalogService *_analogService;
  ITimerService *_timerService;
  IPwmService *_pwmService;
  STM32HalCommunicationService_CDC *_cdcService;
  CommunicationHandler_EFIGenie *_efigenieHandler;
  EngineMain *_engineMain;
  Variable *loopTime;
  uint32_t prev;

  bool write(void *destination, const void *data, size_t length) {
    if(reinterpret_cast<size_t>(destination) >= 0x20000000 && reinterpret_cast<size_t>(destination) <= 0x2000FA00)
    {
      std::memcpy(destination, data, length);
    }
    else if(reinterpret_cast<size_t>(destination) >= 0x8004000 && reinterpret_cast<size_t>(destination) <= 0x8008000)
    {
      HAL_FLASH_Unlock();
      FLASH_OBProgramInitTypeDef pOBInit;
      pOBInit.OptionType = OPTIONBYTE_WRP;
      pOBInit.WRPState = OB_WRPSTATE_DISABLE;
      pOBInit.WRPSector = FLASH_SECTOR_1;
      pOBInit.Banks = FLASH_BANK_1;
      HAL_FLASHEx_OBProgram(&pOBInit);
      if(reinterpret_cast<size_t>(destination) == 0x8004000)
      {
        FLASH_EraseInitTypeDef pEraseInit;
        pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
        pEraseInit.NbSectors = 1;
        pEraseInit.Sector = 1;
        pEraseInit.VoltageRange = VOLTAGE_RANGE_3;

        uint32_t SectorError;
        HAL_FLASHEx_Erase(&pEraseInit, &SectorError);
      }

      for(size_t i = 0; i < length; i++)
      {
        uint32_t programType = FLASH_TYPEPROGRAM_BYTE;
        uint8_t data = 0;
        std::memcpy(&data,  reinterpret_cast<const void *>(reinterpret_cast<const uint8_t *>(data) + i), sizeof(uint8_t));
        HAL_FLASH_Program(programType, reinterpret_cast<size_t>(destination) + i, data);
      }
      pOBInit.WRPState = OB_WRPSTATE_ENABLE;
      HAL_FLASHEx_OBProgram(&pOBInit);
      HAL_FLASH_Lock();
    }
    return true;
  }

  bool quit() {
    if(_engineMain != 0)
    {
      delete _engineMain;
      _engineMain = 0;
    }
    return true;
  }

  bool start() {
    if(_engineMain == 0)
    {
      size_t configSize = 0;
      _engineMain = new EngineMain(&_config, configSize, &_serviceRegistry, _variableMap);
      _metadata = Config::OffsetConfig(&_config, *reinterpret_cast<const uint32_t *>(&_config) + 8);

      _engineMain->Setup();
    }
    return true;
  }

  void Setup() 
  {
    _variableMap = new GeneratorMap<Variable>();
    _cdcService = new STM32HalCommunicationService_CDC();

    _digitalService = new Stm32HalDigitalService();
    _analogService = new Stm32HalAnalogService();
    _timerService = new Stm32HalTimerService(TimerIndex::Index2);
    _pwmService = new Stm32HalPwmService();

    _serviceRegistry.Register<IDigitalService>(_digitalService);
    _serviceRegistry.Register<IAnalogService>(_analogService);
    _serviceRegistry.Register<ITimerService>(_timerService);
    _serviceRegistry.Register<IPwmService>(_pwmService);

		size_t _configSize = 0;
    _engineMain = new EngineMain(reinterpret_cast<void*>(&_config), _configSize, &_serviceRegistry, _variableMap);

    _metadata = Config::OffsetConfig(&_config, *reinterpret_cast<const uint32_t *>(&_config) + 8);

    _efigenieHandler = new CommunicationHandler_EFIGenie(_variableMap, write, quit, start, reinterpret_cast<const void*>(&_config));
    _cdcService->RegisterReceiveCallBack([](communication_send_callback_t send, const void *data, size_t length){ return _efigenieHandler->Receive(send, data, length);});

    _engineMain->Setup();
    loopTime = _variableMap->GenerateValue(250);
  }
  void Loop() 
  {
    const tick_t now = _timerService->GetTick();
    *loopTime = (float)(now-prev) / _timerService->GetTicksPerSecond();
    prev = now;
    _cdcService->Flush();

    if(_engineMain != 0)
      _engineMain->Loop();
  }
}