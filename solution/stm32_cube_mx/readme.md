# Documentation

## Video

[![Watch the video](https://img.youtube.com/vi/CgwbZnTcp8Q/hqdefault.jpg)](https://www.youtube.com/embed/CgwbZnTcp8Q)

## [CubeMX Project](./Solution/)
## [Code](./Solution/Core/Src/main.c)

#### Below you can find the relevant parts of the code.

```C
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
#include <string.h>
/* USER CODE END Includes */

/* USER CODE BEGIN PFP */
void convert_ms_to_hhmmssms(uint32_t* ms, uint32_t* hours, uint32_t* minutes, uint32_t* seconds);
void get_timestamp(char* timestamp);
void elapsed_tim2_callback(TIM_HandleTypeDef *htim);
/* USER CODE END PFP */

/* USER CODE BEGIN 2 */
HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, elapsed_tim2_callback);
HAL_TIM_Base_Start_IT(&htim2);
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void convert_ms_to_hhmmssms(uint32_t* ms, uint32_t* hours, uint32_t* minutes, uint32_t* seconds)
{
  *seconds = *ms / 1000;
  *ms = *ms % 1000;

  *minutes = *seconds / 60;
  *seconds = *seconds % 60;

  *hours = *minutes / 60;
  *minutes = *minutes % 60;
}

void get_timestamp(char* timestamp)
{
  uint32_t h, m, s, ms;
  ms = HAL_GetTick();
  convert_ms_to_hhmmssms(&ms, &h, &m, &s);
  sprintf(timestamp, "%lu:%lu:%lu:%lu", h, m, s, ms);
}

void elapsed_tim2_callback(TIM_HandleTypeDef *htim)
{
  //ADC reading
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 100); //100ms
  uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  //HAL_ADC_Stop(&hadc1);

  //voltage to Lux conversion
  float voltage = adc_value * (5.0 / 4095.0);
  int lux = 123.8957 + (65237400000 - 123.8957) / (1 + pow((voltage / 0.00003897886), 1.778511));
  
  //creating timestamp
  char timestamp[16];
  get_timestamp(timestamp);

  //sending result to UART
  char buffer[64];
  sprintf(buffer, "%s brightness: ~%dLux\n", timestamp, lux);
  HAL_UART_Transmit(&hlpuart1, (uint8_t*)buffer, strlen(buffer), 100); //100ms
}
/* USER CODE END 4 */
```

### Development environment

To program the board I used a combination of STM32CubeMX and Visual Studio Code.

Useful links:

- <https://www.st.com/en/development-tools/stm32cubemx.html>
- <https://github.com/bmd-studio/stm32-for-vscode>

### Technical Documentation

- [STM32G4 HAL documentation](https://www.st.com/en/embedded-software/stm32cubeg4.html#documentation)
