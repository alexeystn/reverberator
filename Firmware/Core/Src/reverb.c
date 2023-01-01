#include "main.h"
#include "defines.h"
#include "generator.h"
#include "filters.h"

extern I2S_HandleTypeDef hi2s1;
extern I2S_HandleTypeDef hi2s3;


int16_t data_output[4];
int16_t data_input[4];
int16_t delayed_output = 0;
uint32_t pointer = 0;
uint8_t test_enabled = 0;


filter_t Filters_Comb[F_COMB_COUNT];
filter_t Filters_AllPass[F_ALLPASS_COUNT];


float Reverb_Process(float sample)
{
  float newSample = 0;
  uint8_t i;
  for (i = 0; i < F_COMB_COUNT; i++) {
    newSample += Filter_Process(&Filters_Comb[i], sample);
  }
  newSample /= 4.0f;

  for (i = 0; i < F_ALLPASS_COUNT; i++) {
    newSample = Filter_Process(&Filters_AllPass[i], newSample);
  }
  return newSample;
}


void Reverb_Start(void)
{
  Filter_Init(&Filters_Comb[0], F_COMB, 0.805, 36.04);
  Filter_Init(&Filters_Comb[1], F_COMB, 0.827, 31.12);
  Filter_Init(&Filters_Comb[2], F_COMB, 0.783, 40.44);
  Filter_Init(&Filters_Comb[3], F_COMB, 0.764, 44.92);
  Filter_Init(&Filters_AllPass[0], F_ALLPASS, 0.7, 5.00);
  Filter_Init(&Filters_AllPass[1], F_ALLPASS, 0.7, 1.68);
  Filter_Init(&Filters_AllPass[2], F_ALLPASS, 0.7, 0.48);

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
  delayed_output = Reverb_Process(sample);
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
