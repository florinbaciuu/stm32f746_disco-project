

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"

//---------------------------------------------------

TIM_HandleTypeDef htim6;

// init the timer
// functie apelata automat de  HAL_Init(); in main
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    RCC_ClkInitTypeDef clkconfig;
    uint32_t uwTimclock = 0U;
    uint32_t uwPrescalerValue = 0U;
    uint32_t pFLatency;

    __HAL_RCC_TIM6_CLK_ENABLE();

    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
    uwTimclock = HAL_RCC_GetPCLK1Freq();

    uwPrescalerValue = (uwTimclock / 1000000U) - 1U;

    htim6.Instance = TIM6;
    htim6.Init.Period = (1000000U / 1000U) - 1U;
    htim6.Init.Prescaler = uwPrescalerValue;
    htim6.Init.ClockDivision = 0;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim6) == HAL_OK)
    {
        if (HAL_TIM_Base_Start_IT(&htim6) == HAL_OK)
        {
            HAL_NVIC_SetPriority(TIM6_DAC_IRQn, TickPriority, 0U);
            HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
            return HAL_OK;
        }
    }
    return HAL_ERROR;
}

//---------------------------------------------------

void HAL_SuspendTick(void)
{
    __HAL_TIM_DISABLE_IT(&htim6, TIM_IT_UPDATE);
}

void HAL_ResumeTick(void)
{
    __HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
}

//---------------------------------------------------

//Creează handlerul de interrupt:
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

//---------------------------------------------------

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
}

//---------------------------------------------------

