#include "main.h"
#include "defines.h"
#include "generator.h"
#include "reverb.h"
#include "compressor.h"


extern I2S_HandleTypeDef hi2s1;
extern I2S_HandleTypeDef hi2s3;


int16_t data_output[4];
int16_t data_input[4];
int16_t delayed_output = 0;
uint32_t pointer = 0;
uint8_t test_enabled = 0;


void Processing_Start(void)
{
  Reverb_Init();
  Compressor_Init();

  if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 0) {
    test_enabled = 1;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // LED ON
  } else {
    test_enabled = 0;
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // LED OFF
  }
  HAL_I2S_Transmit_DMA(&hi2s1, (uint16_t*)data_output, 4);
  HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)data_input, 4);
}


static void Buffer_Put(int16_t sample)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  //delayed_output = Reverb_Do(sample);
  delayed_output = Reverb_Do(Compressor_Do(sample));
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  data_output[0] = 0; // Right OUT
  data_output[1] = delayed_output; // Left OUT
}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  data_output[2] = 0; // Right OUT
  data_output[3] = delayed_output; // Left OUT
}


void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  Buffer_Put(data_input[1]); // Right IN
}


void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  Buffer_Put(data_input[3]); // Left IN
}
