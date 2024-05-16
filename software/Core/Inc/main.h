/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim8;
extern DMA_HandleTypeDef hdma_tim1_ch1;
extern DMA_HandleTypeDef hdma_tim1_ch2;
extern DMA_HandleTypeDef hdma_tim8_ch3;
extern DMA_HandleTypeDef hdma_tim8_ch4_trig_com;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOC
#define ADC_BATT_Pin GPIO_PIN_0
#define ADC_BATT_GPIO_Port GPIOC
#define ADC_CURR_Pin GPIO_PIN_2
#define ADC_CURR_GPIO_Port GPIOC
#define BEEPER_1_Pin GPIO_PIN_4
#define BEEPER_1_GPIO_Port GPIOA
#define GYRO_EXTI_Pin GPIO_PIN_4
#define GYRO_EXTI_GPIO_Port GPIOC
#define GYRO_EXTI_EXTI_IRQn EXTI4_IRQn
#define S4_Pin GPIO_PIN_0
#define S4_GPIO_Port GPIOB
#define S5_Pin GPIO_PIN_1
#define S5_GPIO_Port GPIOB
#define GYRO_CS_Pin GPIO_PIN_2
#define GYRO_CS_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOB
#define S1_Pin GPIO_PIN_14
#define S1_GPIO_Port GPIOB
#define MOTOR_4_Pin GPIO_PIN_8
#define MOTOR_4_GPIO_Port GPIOC
#define MOTOR_3_Pin GPIO_PIN_9
#define MOTOR_3_GPIO_Port GPIOC
#define MOTOR_2_Pin GPIO_PIN_8
#define MOTOR_2_GPIO_Port GPIOA
#define MOTOR_1_Pin GPIO_PIN_9
#define MOTOR_1_GPIO_Port GPIOA
#define LED_STRIP_Pin GPIO_PIN_15
#define LED_STRIP_GPIO_Port GPIOA
#define S2_Pin GPIO_PIN_4
#define S2_GPIO_Port GPIOB
#define S3_Pin GPIO_PIN_5
#define S3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOC
#define GYRO_CS_Pin GPIO_PIN_2
#define GYRO_CS_GPIO_Port GPIOB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
