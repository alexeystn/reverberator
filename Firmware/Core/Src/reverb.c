#include "main.h"

extern I2S_HandleTypeDef hi2s1;
extern I2S_HandleTypeDef hi2s3;


#define DELAY_LENGTH 10000

static int16_t buffer[DELAY_LENGTH];
static int16_t data_output[4];
static int16_t data_input[4];
static int16_t delayed_output = 0;
static uint32_t pointer = 0;


void Reverb_Start(void)
{
  HAL_I2S_Transmit_DMA(&hi2s1, (uint16_t*)data_output, 4);
  HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)data_input, 4);
}


static void BufferPut(int16_t sample)
{
  if (pointer >= DELAY_LENGTH) {
    pointer = 0;
  }
  delayed_output = buffer[pointer];
  buffer[pointer] = buffer[pointer] * 3 / 4 + sample;
  pointer++;
}


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  data_output[0] = delayed_output;
  data_output[1] = delayed_output;
}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  data_output[2] = delayed_output;
  data_output[3] = delayed_output;
}


void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  BufferPut(data_input[0]);
}


void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  BufferPut(data_input[2]);
}
