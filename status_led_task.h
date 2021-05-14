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

/* Include guard */
#ifndef STATUS_LED_TASK_H
#define STATUS_LED_TASK_H

/* Header file includes */ 
#include "project.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"  
    

/* List of BLE commands */
typedef enum
{
    BLE_NO_ALERT,
    BLE_MILD_ALERT,
    BLE_HIGH_ALERT
}status_led_commands_t;
    
extern QueueHandle_t statusLedCommandQ;
    
/* Task_Ble takes care of the BLE module in this code example */   
void Task_StatusLed(void *pvParameters);
//void static StatusLedGreenTimerCallback(TimerHandle_t xTimer);
#endif /* STATUS_LED_TASK_H */

/* [] END OF FILE */
