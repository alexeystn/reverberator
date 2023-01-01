#include "main.h"
#include "defines.h"
#include "generator.h"

extern I2S_HandleTypeDef hi2s1;
extern I2S_HandleTypeDef hi2s3;


#define BUFFER_SIZE ((SAMPLING_FREQUENCY * (DELAY_TIME_MS -1)) / 1000)  // Why -1 ?

int16_t buffer[BUFFER_SIZE];
int16_t data_output[4];
int16_t data_input[4];
int16_t delayed_output = 0;
uint32_t pointer = 0;
uint8_t test_enabled = 0;


void Reverb_Start(void)
{
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
  if (pointer >= BUFFER_SIZE) {
    pointer = 0;
  }
  delayed_output = buffer[pointer];
  buffer[pointer] = sample;// buffer[pointer] * 3 / 4 + sample;
  pointer++;
}


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  Generator_Step();
  if (test_enabled) {
    data_output[0] = Generator_GetValue(); // Right OUT
  } else {
    data_output[0] = 0; // Right OUT
  }
  data_output[1] = delayed_output; // Left OUT
}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  Generator_Step();
  if (test_enabled) {
    data_output[2] = Generator_GetValue(); // Right OUT
  } else {
    data_output[2] = 0; // Right OUT
  }
  data_output[3] = delayed_output; // Left OUT
}


void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if (test_enabled) {
    Buffer_Put(data_input[0]); // Left IN
  } else {
    Buffer_Put(data_input[1]); // Right IN
  }
}


void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if (test_enabled) {
    Buffer_Put(data_input[2]); // Left IN
  } else {
    Buffer_Put(data_input[3]); // Left IN
  }
}
