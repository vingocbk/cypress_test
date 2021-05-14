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

/* Header file includes */
#include <math.h>
#include "status_led_task.h"
#include "debug.h"
#include "task.h"  
#include "timers.h"   
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/* LED refresh interval of 500ms is used when continuously toggling a 
   status LED */
#define STATUS_LED_TOGGLE_INTERVAL (pdMS_TO_TICKS(500u))
/* LED refresh interval of 4s is used for blinking a status LED once */
#define STATUS_LED_BLINK_INTERVAL  (pdMS_TO_TICKS(1000u))
/* Idle interval is used for static LED states or when no LEDs are ON */
#define STATUS_LED_IDLE_INTERVAL   (portMAX_DELAY)

TimerHandle_t xTimers;
QueueHandle_t statusLedCommandQ;
void static StatusLedGreenTimerCallback(TimerHandle_t xTimer);

/*******************************************************************************
* Function Name: void static StatusLedGreenTimerCallback(TimerHandle_t xTimer)                          
********************************************************************************
* Summary:
*  This function is called when the Orange LED Timer expires
*
* Parameters:
*  TimerHandle_t xTimer :  Current timer value (unused)
*
* Return:
*  void
*
*******************************************************************************/
void static StatusLedGreenTimerCallback(TimerHandle_t xTimer)
{
    /* Variable used to store the return values of RTOS APIs */
//    BaseType_t rtosApiResult;
    
    /* Remove warning for unused parameter */
    (void)xTimer;
  
    Cy_GPIO_Inv(PIN_TEST_G_0_PORT, PIN_TEST_G_0_NUM);

}

void Task_StatusLed(void *pvParameters)
{
    /* Remove warning for unused parameter */
    (void)pvParameters;
    DEBUG_PRINTF("Run Task_StatusLed!!!\r\n");
    
    status_led_commands_t statusLedData;
    
    xTimers = xTimerCreate ("Orange Status LED Timer",
                            STATUS_LED_BLINK_INTERVAL, pdTRUE, 
                            NULL, StatusLedGreenTimerCallback); 
    
    /* Variable used to store the return values of RTOS APIs */
    BaseType_t rtosApiResult;
    
    for(;;){
        //Cy_GPIO_Inv(PIN_TEST_G_0_PORT, PIN_TEST_G_0_NUM);
        rtosApiResult = xQueueReceive(statusLedCommandQ, &statusLedData,
                         portMAX_DELAY);
        /* Command has been received from statusLedCommandQ */
        if(rtosApiResult == pdTRUE)
        {
            /* Take an action based on the command received for Orange LED */
            switch(statusLedData)
            {
                /* OFF LED */
                case BLE_NO_ALERT:
                    xTimerStop(xTimers,0u);
                    Cy_GPIO_Clr(PIN_TEST_G_0_PORT, PIN_TEST_G_0_NUM);
                    break;
                /* BLYNK LED */
                case BLE_MILD_ALERT:
                    xTimerStart(xTimers,0u);
                    break;
                /* ON LED */    
                case BLE_HIGH_ALERT:
                    xTimerStop(xTimers,0u);
                    Cy_GPIO_Set(PIN_TEST_G_0_PORT, PIN_TEST_G_0_NUM);
                    break;
                default:
                    break;
            }
            
        }
    }
}




/* [] END OF FILE */
