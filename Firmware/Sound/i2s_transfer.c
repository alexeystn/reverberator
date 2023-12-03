#include "main.h"
#include "defines.h"
#include "processing.h"


extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;
extern UART_HandleTypeDef huart1;


int16_t buffer_output[4];
int16_t buffer_input[4];
int16_t sample_output = 0;
int16_t debug_buffer[2][DEBUG_BUFFER_SIZE];
int16_t debug_sample_pointer = 0;
uint8_t debug_buffer_pointer = 0;


void Debug_EnablePin(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DEBUG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_GPIO_Port, &GPIO_InitStruct);
}

void I2S_Transfer_Start(void)
{
#ifdef ENABLE_DEBUG_PIN
  Debug_EnablePin();
#endif

  HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)buffer_output, 4);
  HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)buffer_input, 4);
}


static void Debug_Put(int16_t sample)
{
  debug_buffer[debug_buffer_pointer][debug_sample_pointer] = sample;
  debug_sample_pointer++;
  if (debug_sample_pointer == DEBUG_BUFFER_SIZE) {
    HAL_UART_Abort(&huart1);
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&debug_buffer[debug_buffer_pointer][0],
        sizeof(debug_buffer[0]));
    debug_sample_pointer = 0;
    debug_buffer_pointer++;
    if (debug_buffer_pointer == 2) {
      debug_buffer_pointer = 0;
    }
  }
}


static void Buffer_Put(int16_t input)
{
  HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);
  Debug_Put(input | 0x0001);
  sample_output = Processing_Apply(input);
  Debug_Put(sample_output & ~0x0001);
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
#ifdef USE_INPUT_UNBALANCED
  Buffer_Put(buffer_input[0]); // Right IN
#endif
#ifdef USE_INPUT_BALANCED
  Buffer_Put(buffer_input[1]); // Left IN
#endif
#ifdef USE_INPUT_SUMM
  Buffer_Put(buffer_input[0] + buffer_input[1]); // Both
#endif
}


void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
#ifdef USE_INPUT_UNBALANCED
  Buffer_Put(buffer_input[2]); // Right IN
#endif
#ifdef USE_INPUT_BALANCED
  Buffer_Put(buffer_input[3]); // Left IN
#endif
#ifdef USE_INPUT_SUMM
  Buffer_Put(buffer_input[2] + buffer_input[3]); // Both
#endif
}


