#pragma once

/* ========= FreeRTOSConfig.h pentru STM32F746 (Cortex-M7 r0p1) + HAL =========
   - Port: FreeRTOS-Kernel/portable/GCC/ARM_CM7/r0p1
   - Tick: 1 ms
   - Heap: 128 KB (heap_4.c)
   - NU mapăm SysTick în config; facem handler în stm32f7xx_it.c
============================================================================= */

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

/* Scheduler & timp */
#define configUSE_PREEMPTION 1
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0 /* pune 1 doar dacă vrei vApplicationTickHook() */
#define configCPU_CLOCK_HZ (SystemCoreClock)
#define configTICK_RATE_HZ ((TickType_t) 1000)
#define configMAX_PRIORITIES 7
#define configMINIMAL_STACK_SIZE (1024)
#define configTOTAL_HEAP_SIZE ((size_t) (128 * 1024)) /* ajustează după nevoie */
#define configMAX_TASK_NAME_LEN 16
#define configUSE_16_BIT_TICKS 0
#define configIDLE_SHOULD_YIELD 1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_MALLOC_FAILED_HOOK 1

/* Sincronizare &  */
#define configUSE_MUTEXES 1
#define configUSE_RECURSIVE_MUTEXES 1
#define configUSE_COUNTING_SEMAPHORES 1
#define configQUEUE_REGISTRY_SIZE 8

/*  & timers */
#define configUSE_TIMERS 1
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 3)
#define configTIMER_QUEUE_LENGTH 10
#define configTIMER_TASK_STACK_DEPTH (4096)

/* Hook-uri & debug */
#define configCHECK_FOR_STACK_OVERFLOW 0 /* poți seta 2 pentru debug agresiv */
#define configUSE_MALLOC_FAILED_HOOK 1
#define configUSE_APPLICATION_TASK_TAG 1
#define configGENERATE_RUN_TIME_STATS 0
#define configUSE_TRACE_FACILITY 0
#define configUSE_NEWLIB_REENTRANT 1 /* printf thread-safe cu newlib */

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/* API-uri incluse */
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_xTaskGetTickCount 1
#define INCLUDE_vTaskCleanUpResources 0


/*------------- CMSIS-RTOS V2 specific defines -----------*/
/* When using CMSIS-RTOSv2 set configSUPPORT_STATIC_ALLOCATION to 1
 * is mandatory to avoid compile errors.
 * CMSIS-RTOS V2 implmentation requires the following defines
 *
#define configSUPPORT_STATIC_ALLOCATION          1   <-- cmsis_os threads are created using xTaskCreateStatic() API
#define configMAX_PRIORITIES                    (56) <-- Priority range in CMSIS-RTOS V2 is [0 .. 56]
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0    <-- when set to 1, configMAX_PRIORITIES can't be more than 32 which is not suitable for the new CMSIS-RTOS v2 priority range
*/

/* the CMSIS-RTOS V2 FreeRTOS wrapper is dependent on the heap implementation used
 * by the application thus the correct define need to be enabled from the list
 * below
 *
//define USE_FreeRTOS_HEAP_1
//define USE_FreeRTOS_HEAP_2
//define USE_FreeRTOS_HEAP_3
#define USE_FreeRTOS_HEAP_4
//define USE_FreeRTOS_HEAP_5
*/
#define USE_FreeRTOS_HEAP_4

/* Priorități NVIC (STM32F7 are 4 biți) */
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Assert */
#define configASSERT(x)           \
    if ((x) == 0) {               \
        taskDISABLE_INTERRUPTS(); \
        for (;;)                  \
            ;                     \
    }

/* Mapări ISR către CMSIS – SVC & PendSV le dă port.c (NU redefini în proiect) */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT:
   NU defini aici SysTick. Fă handler-ul în stm32f7xx_it.c și apelează HAL + RTOS.
   // NU: #define xPortSysTickHandler SysTick_Handler
*/
