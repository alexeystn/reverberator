#include "storage.h"


#define STORAGE_P_COUNT  6

#define STORAGE_SECTOR     FLASH_SECTOR_3
#define STORAGE_ADDRESS    0x0800C000


extern uint8_t parameterValue[];


void Storage_Save(void)
{
  uint32_t value = 0;
  uint8_t checksum = 0xA;

  for (int i = 0; i < STORAGE_P_COUNT; i++) {
    value += parameterValue[i] & 0x0F;
    checksum ^= parameterValue[i];
    value <<= 4;
  }
  value += checksum & 0x0F;

  FLASH_EraseInitTypeDef eraseInit;
  eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
  eraseInit.NbSectors = 1;
  eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  eraseInit.Sector = STORAGE_SECTOR;

  uint32_t SectorError;

  HAL_FLASH_Unlock();
  HAL_FLASHEx_Erase(&eraseInit, &SectorError);

  HAL_FLASH_Unlock();
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, STORAGE_ADDRESS, value);
  HAL_FLASH_Lock();

}

uint8_t Storage_Load(void)
{
  uint8_t success = 0;
  uint32_t value = 0;
  uint8_t checksum_load;
  uint8_t checksum_calc;

  value = *((uint32_t*)STORAGE_ADDRESS);

  checksum_calc = 0xA;
  checksum_load = value & 0x0F;

  for (int i = 0; i < STORAGE_P_COUNT; i++) {
    value >>= 4;
    parameterValue[STORAGE_P_COUNT - i - 1] = value & 0x0F;
    checksum_calc ^= value & 0x0F;
  }

  success = (checksum_calc == checksum_load);

  if (!success) {
    for (int i = 0; i < STORAGE_P_COUNT; i++) {
      parameterValue[i] = 5;
    }
  }
  return success;
}





