/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "ble_task.h"
#include "status_led_task.h"  
#include "debug.h"  
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/* Priorities of user tasks in this project */
#define TASK_BLE_PRIORITY           (2u)        //max is 7, config max in FreeRTOSConfig.h
#define TASK_STATUS_LED_PRIORITY    (1u)

/* Stack sizes of user tasks in this project */
#define TASK_BLE_STACK_SIZE         (1024u)
#define TASK_STATUS_LED_STACK_SIZE  (configMINIMAL_STACK_SIZE)

 
/* Queue lengths of message queues used in this project */
#define BLE_COMMAND_QUEUE_LEN       (10u)
#define STATUS_LED_QUEUE_LEN        (1u)

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    /* Start the UART debug port */
    UART_DEBUG_START();
    DEBUG_PRINTF("CORE CM4 START!!!\r\n");
    //DEBUG_PRINTF
    
    bleCommandQ = xQueueCreate(BLE_COMMAND_QUEUE_LEN, sizeof(ble_commands_t));
    statusLedCommandQ = xQueueCreate(STATUS_LED_QUEUE_LEN, sizeof(status_led_commands_t));
    /* Initialize wakeup pin for Hibernate */
    //Cy_SysPm_SetHibernateWakeupSource(CY_SYSPM_HIBERNATE_PIN1_LOW);
    
    
    xTaskCreate(Task_StatusLed, "Task Statusled", TASK_STATUS_LED_STACK_SIZE, NULL, TASK_STATUS_LED_PRIORITY, NULL);
    xTaskCreate(Task_Ble,"Task Ble", TASK_BLE_STACK_SIZE, NULL, TASK_BLE_PRIORITY, NULL);
    

    
    
    
    /* Start the RTOS scheduler. This function should never return */
    vTaskStartScheduler();
    
    /* Should never get here! */ 
    DEBUG_PRINTF("Error!   : RTOS - scheduler crashed \r\n");
    
    /* Halt the CPU if scheduler exits */
    CY_ASSERT(0);

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
