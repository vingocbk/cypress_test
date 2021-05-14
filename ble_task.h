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
#ifndef BLE_TASK_H
#define BLE_TASK_H

/* Header file includes */ 
#include "project.h"
#include "FreeRTOS.h"
#include "queue.h"
    

/* List of BLE commands */
typedef enum
{
    BLE_NO_ALERT_A,
    BLE_MILD_ALERT_A,
    BLE_HIGH_ALERT_A
}ble_commands_t;
    
extern QueueHandle_t bleCommandQ;


/* Task_Ble takes care of the BLE module in this code example */   
void Task_Ble(void *pvParameters);
void AppCallBack(uint32_t event, void* eventParam);
    
#endif /*BLE_TASK_H*/

/* [] END OF FILE */
