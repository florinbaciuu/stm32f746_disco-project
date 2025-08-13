#include "stm32f7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

static TIM_HandleTypeDef htim7;

extern void xPortSysTickHandler(void);

void vPortSetupTimerInterrupt(void)
{
    __HAL_RCC_TIM7_CLK_ENABLE();

    htim7.Instance = TIM7;
    htim7.Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / 1000000U) - 1U;
    htim7.Init.Period = (1000000U / configTICK_RATE_HZ) - 1U;
    htim7.Init.ClockDivision = 0;
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_Base_Init(&htim7);
    HAL_TIM_Base_Start_IT(&htim7);

    HAL_NVIC_SetPriority(TIM7_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void TIM7_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
            xPortSysTickHandler(); // Asta face tick-ul FreeRTOS!
        }
    }
}
