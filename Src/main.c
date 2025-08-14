/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Baciu Aurel Florin
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 Florin Aurel Baciu.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"

#include "FreeRTOS.h"
#include "task.h"

//================================================================//

UART_HandleTypeDef       huart1;  // ai grijă să fie inițializat
extern volatile uint32_t g_systick_hits;

//================================================================//

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void Error_Handler(void) {
    __disable_irq();
    for (;;) {
    }
}

//================================================================//

static void UART1_Init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* PA9 -> TX, PA10 -> RX */
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart1);
}

//================================================================//

static void SystemClock_Config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    __HAL_RCC_PWR_CLK_ENABLE(); /* Enable Power Control clock */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 25;
    RCC_OscInitStruct.PLL.PLLN       = 432;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        Error_Handler();
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
        Error_Handler();
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
        Error_Handler();
}

//================================================================//

int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
    return ch;
}

//================================================================//

void print_heap(void) {
    printf("Free heap: %lu bytes\n", (unsigned long) xPortGetFreeHeapSize());
    printf("Minimum ever free: %lu bytes\n", (unsigned long) xPortGetMinimumEverFreeHeapSize());
}

//================================================================//

/* LED-ul verde de pe STM32F746 Disco este pe PI1 */
#define LED_GPIO_PORT GPIOI
#define LED_PIN GPIO_PIN_1

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOI_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin              = LED_PIN;
    GPIO_InitStruct.Mode             = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull             = GPIO_NOPULL;
    GPIO_InitStruct.Speed            = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

//================================================================//

/*
███████ ██████  ███████ ███████ ██████ ████████  ██████  ███████ 
██      ██   ██ ██      ██      ██   ██   ██    ██    ██ ██      
█████   ██████  █████   █████   ██████    ██    ██    ██ ███████ 
██      ██   ██ ██      ██      ██   ██   ██    ██    ██      ██ 
██      ██   ██ ███████ ███████ ██   ██   ██     ██████  ███████ 
*/

void vApplicationMallocFailedHook(void) {
    __disable_irq();
    for (;;) {
    }
}

// -------------------------------------------

void vApplicationStackOverflowHook(TaskHandle_t t, char* name) {
    (void) t;
    (void) name;
    __disable_irq();
    for (;;) {
    }
}

//================================================================//

/********************************************** */
/*                   TASK                       */
/********************************************** */
static void uart_TaskFunction_t(void* arg) {
    (void) arg;
    const char* msg = "RTOS OK\r\n";
    for (;;) {
        HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), 100);
        printf("The board is on! XXX\r\n");
        print_heap();
        portYIELD();
        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}

// -------------------------------------------

/********************************************** */
/*                   TASK                       */
/********************************************** */
static void led_TaskFunction_t(void* arg) {
    (void) arg;
    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// -------------------------------------------

TaskHandle_t uart_xHandle;
TaskHandle_t led_xHandle;

bool RTOS_Kernel() {
    printf("FreeRTOS Kernel Init!\r\n");

    BaseType_t uart_task = xTaskCreate(uart_TaskFunction_t,
        "uart_task",
        2048,
        NULL,
        /* uxPriority:*/ (7),
        &uart_xHandle);
    configASSERT(uart_task == pdPASS);

    BaseType_t led_task = xTaskCreate(led_TaskFunction_t,
        "led_task",
        1024,
        NULL,
        /* uxPriority:*/ (5),
        &led_xHandle);
    configASSERT(led_task == pdPASS);

    vTaskStartScheduler();

    printf("FreeRTOS Kernel Ok!\r\n");


    return 1;
}

//================================================================//

/****************************/
/*
███    ███  █████  ██ ███    ██ 
████  ████ ██   ██ ██ ████   ██ 
██ ████ ██ ███████ ██ ██ ██  ██ 
██  ██  ██ ██   ██ ██ ██  ██ ██ 
██      ██ ██   ██ ██ ██   ████ 
  * This is the main entry point of the application.
*/
/****************************/
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();

    UART1_Init();
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("Hello World prin serial!\r\n");  // asta merge.
    printf("The board is on!\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*) "Ce mai faci ma eu sunt bine din test!!!!\r\n", 41, 100);

    RTOS_Kernel();

    while (1) {
        HAL_Delay(1500);
        printf("Nu trebuia sa se ajunga aici !\r\n");
    }
}

/****************************/

//================================================================//
