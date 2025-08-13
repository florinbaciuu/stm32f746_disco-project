

#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

//extern void xPortSysTickHandler(void);

__attribute__((weak)) void SysTick_Handler(void)
{
   //  HAL_IncTick(); // l=am pus in stm32f7xx_hal_timebase_tim
    // if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    //     xPortSysTickHandler();
    // }
}
