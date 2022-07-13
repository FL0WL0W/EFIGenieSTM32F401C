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
#include "CommunicationHandler_Prefix.h"
#include "STM32HalCommunicationService_CDC.h"
#include "CommunicationHandlers/CommunicationHandler_GetVariable.h"
#include "stm32f4xx_hal_flash_ex.h"

using namespace OperationArchitecture;
using namespace EmbeddedIOServices;
using namespace EmbeddedIOOperations;
using namespace Stm32;
using namespace EFIGenie;

extern char _config;


extern "C"
{
  const char ack[1] = {6};
  const char nack[1] = {21};
  GeneratorMap<Variable> *_variableMap;
  uint8_t VariableBuff[9];
  uint32_t Commands[32];
  uint8_t CommandReadPointer = 0;
  const char doneResponseText[10] = " (Done)\n\r";
  const void *_metadata;
  bool secondCommand = false;
  EmbeddedIOServiceCollection _embeddedIOServiceCollection;
  STM32HalCommunicationService_CDC *_cdcService;
  CommunicationHandler_GetVariable *_getVariableHandler;
  CommunicationHandler_Prefix *_prefixHandler;
  EFIGenieMain *_engineMain;
  Variable *loopTime;
  uint32_t prev;

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

    _variableMap = new GeneratorMap<Variable>();
    _cdcService = new STM32HalCommunicationService_CDC();
    _cdcService->RegisterHandler(_prefixHandler = new CommunicationHandler_Prefix());

    _embeddedIOServiceCollection.DigitalService = new Stm32HalDigitalService();
    _embeddedIOServiceCollection.AnalogService = new Stm32HalAnalogService();
    _embeddedIOServiceCollection.TimerService = new Stm32HalTimerService(TimerIndex::Index2);
    _embeddedIOServiceCollection.PwmService = new Stm32HalPwmService();

		size_t _configSize = 0;
    _engineMain = new EFIGenieMain(reinterpret_cast<void*>(&_config), _configSize, &_embeddedIOServiceCollection, _variableMap);

    _metadata = Config::OffsetConfig(&_config, *reinterpret_cast<const uint32_t *>(&_config) + 8);
    _getVariableHandler = new CommunicationHandler_GetVariable(_variableMap);
    _prefixHandler->RegisterHandler(_getVariableHandler, "g", 1);
    _prefixHandler->RegisterReceiveCallBack(new communication_receive_callback_t([](communication_send_callback_t send, void *data, size_t length)
    { 
      const size_t minDataSize = sizeof(void *) + sizeof(size_t);
      if(length < minDataSize)
        return static_cast<size_t>(0);

      void *readData = *reinterpret_cast<void **>(data);
      size_t readDataLength = *reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(data) + sizeof(void *));
      send(readData, readDataLength);

      return minDataSize;
    }), "r", 1);
    _prefixHandler->RegisterReceiveCallBack(new communication_receive_callback_t([](communication_send_callback_t send, void *data, size_t length)
    { 
      const size_t minDataSize = sizeof(void *) + sizeof(size_t);
      if(length < minDataSize)
        return static_cast<size_t>(0);

      void *writeDataDest = *reinterpret_cast<void **>(data);
      size_t writeDataLength = *reinterpret_cast<size_t *>(reinterpret_cast<uint8_t *>(data) + sizeof(void *));
      void *writeData = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(data) + sizeof(void *) + sizeof(size_t));

      if(length < minDataSize + writeDataLength)
        return static_cast<size_t>(0);

      if(reinterpret_cast<size_t>(writeDataDest) >= 0x20000000 && reinterpret_cast<size_t>(writeDataDest) <= 0x2000FA00)
      {
        std::memcpy(writeDataDest, writeData, writeDataLength);
      }
      else if(reinterpret_cast<size_t>(writeDataDest) >= 0x8004000 && reinterpret_cast<size_t>(writeDataDest) <= 0x8008000)
      {
        HAL_FLASH_Unlock();
        FLASH_OBProgramInitTypeDef pOBInit;
        pOBInit.OptionType = OPTIONBYTE_WRP;
        pOBInit.WRPState = OB_WRPSTATE_DISABLE;
        pOBInit.WRPSector = FLASH_SECTOR_1;
        pOBInit.Banks = FLASH_BANK_1;
        HAL_FLASHEx_OBProgram(&pOBInit);
        if(reinterpret_cast<size_t>(writeDataDest) == 0x8004000)
        {
          FLASH_EraseInitTypeDef pEraseInit;
          pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
          pEraseInit.NbSectors = 1;
          pEraseInit.Sector = 1;
          pEraseInit.VoltageRange = VOLTAGE_RANGE_3;

          uint32_t SectorError;
          HAL_FLASHEx_Erase(&pEraseInit, &SectorError);
        }

        for(size_t i = 0; i < writeDataLength; i++)
        {
          uint32_t programType = FLASH_TYPEPROGRAM_BYTE;
          uint8_t data = 0;
          std::memcpy(&data,  reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(writeData) + i), sizeof(uint8_t));
          HAL_FLASH_Program(programType, reinterpret_cast<size_t>(writeDataDest) + i, data);
        }
        pOBInit.WRPState = OB_WRPSTATE_ENABLE;
        HAL_FLASHEx_OBProgram(&pOBInit);
      }

      send(ack, sizeof(ack));

      return minDataSize + writeDataLength;
    }), "w", 1);
    _prefixHandler->RegisterReceiveCallBack(new communication_receive_callback_t([](communication_send_callback_t send, void *data, size_t length)
    { 
      if(_engineMain != 0)
      {
        delete _engineMain;
        _engineMain = 0;
      }
      send(ack, sizeof(ack));
      return static_cast<size_t>(0);
    }), "q", 1, false);
    _prefixHandler->RegisterReceiveCallBack(new communication_receive_callback_t([](communication_send_callback_t send, void *data, size_t length)
    { 
      if(_engineMain == 0)
      {
        size_t configSize = 0;
        _engineMain = new EFIGenieMain(&_config, configSize, &_embeddedIOServiceCollection, _variableMap);
        _metadata = Config::OffsetConfig(&_config, *reinterpret_cast<const uint32_t *>(&_config) + 8);

        _engineMain->Setup();
      }
      send(ack, sizeof(ack));
      return static_cast<size_t>(0);
    }), "s", 1, false);
    _prefixHandler->RegisterReceiveCallBack(new communication_receive_callback_t([](communication_send_callback_t send, void *data, size_t length)
    { 
      if(length < sizeof(uint32_t))//make sure there are enough bytes to process a request
        return static_cast<size_t>(0);
      uint8_t offset = *reinterpret_cast<uint32_t *>(data); //grab offset from data

      send(reinterpret_cast<const uint8_t *>(_metadata) + offset * 64, 64);

      return static_cast<size_t>(sizeof(uint32_t));//return number of bytes handled
    }), "m", 1);
    _prefixHandler->RegisterReceiveCallBack(new communication_receive_callback_t([](communication_send_callback_t send, void *data, size_t length)
    { 
      size_t configLocation[1] = { reinterpret_cast<size_t>(&_config) };
      send(configLocation, sizeof(configLocation));
      
      return static_cast<size_t>(0);
    }), "c", 1, false);

    _engineMain->Setup();
    loopTime = _variableMap->GenerateValue(250);
  }
  void Loop() 
  {
    const tick_t now = _embeddedIOServiceCollection.TimerService->GetTick();
    loopTime->Set((float)(now-prev) / _embeddedIOServiceCollection.TimerService->GetTicksPerSecond());
    prev = now;
    _cdcService->Flush();

    if(_engineMain != 0)
      _engineMain->Loop();
  }
}