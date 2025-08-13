

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);

//volatile uint32_t g_systick_hits = 0;   // diagnostic

void SysTick_Handler(void)
{
    HAL_IncTick();
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
    //xPortSysTickHandler();// voi folosii varianta de mai sus ca se vede clar ca de aici e problema
    //g_systick_hits++;       // diagnostic
}