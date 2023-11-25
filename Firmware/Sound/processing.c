#include "main.h"
#include "defines.h"
#include "generator.h"
#include "reverb.h"
#include "filters.h"
#include "compressor.h"

extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;
extern UART_HandleTypeDef huart1;


int16_t buffer_output[4];
int16_t buffer_input[4];
int16_t sample_output = 0;
int16_t debug_buffer[2][DEBUG_BUFFER_SIZE];
int16_t debug_sample_pointer = 0;
uint8_t debug_buffer_pointer = 0;

uint8_t test_enabled = 0;
int16_t peak_level = 0;


float reverbDry = 0;
float reverbWet = 0;


void Set_Dry_Wet(uint8_t dry10, uint8_t wet10) {
  __disable_irq();
  reverbDry = (float)(dry10*dry10*dry10) / 1000.0f;
  reverbWet = (float)(wet10*wet10*wet10) / 1000.0f;
  __enable_irq();
}


static void Peak_Level_Put(int16_t sample)
{
  if (sample > peak_level) {
    peak_level = sample;
  } else {
    if (sample < -peak_level) {
      peak_level = -sample;
    }
  }
}


int16_t Peak_Level_Get(void)
{
  int16_t ret;
  __disable_irq();
  ret = peak_level;
  peak_level = 0;
  __enable_irq();
  return ret;
}


void Processing_Start(void)
{
  Reverb_Init();
  Compressor_Init();
  Filters_Init();

  if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) {
    test_enabled = 1;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // LED ON
  } else {
    test_enabled = 0;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // LED OFF
  }
  HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)buffer_output, 4);
  HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)buffer_input, 4);
}


static void Debug_Put(int16_t sample)
{
  debug_buffer[debug_buffer_pointer][debug_sample_pointer] = sample;
  debug_sample_pointer++;
  if (debug_sample_pointer == DEBUG_BUFFER_SIZE) {
    HAL_UART_Abort(&huart1);
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&debug_buffer[debug_buffer_pointer][0], sizeof(debug_buffer[0]));
    debug_sample_pointer = 0;
    debug_buffer_pointer++;
    if (debug_buffer_pointer == 2) {
      debug_buffer_pointer = 0;
    }
  }
}


static void Buffer_Put(int16_t sample)
{
  static int32_t counter = 0;
  HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);
  Debug_Put(sample);
  Peak_Level_Put(sample);
  float fSample = Compressor_Do(sample);

  if (test_enabled) {
    sample_output = Reverb_Do(sample);
  } else {
    sample_output = fSample * reverbDry + Reverb_Do(fSample) * reverbWet;
    //sample_output = sample + Reverb_Do(Compressor_Do(Filters_Do(sample)));
  }

  Debug_Put(sample_output);
  HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);
}


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  buffer_output[0] = sample_output; // Right OUT
  buffer_output[1] = sample_output; // Left OUT
}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  buffer_output[2] = sample_output; // Right OUT
  buffer_output[3] = sample_output; // Left OUT
}


void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  Buffer_Put(buffer_input[0]); // Right IN
}


void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  Buffer_Put(buffer_input[2]); // Right IN
}
