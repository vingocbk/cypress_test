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
#include "ble_task.h"
#include "debug.h"
#include "task.h"  
#include "timers.h"  
#include "ias.h"
#include "status_led_task.h"

QueueHandle_t bleCommandQ;


void Task_Ble(void *pvParameters)
{
    /* Remove warning for unused parameter */
    (void)pvParameters;     
    DEBUG_PRINTF("Run Task_Ble!!!\r\n");
    
    /* Start BLE component and register generic event handler */
    status_led_commands_t datasend_t = BLE_MILD_ALERT;
    xQueueSend(statusLedCommandQ, &datasend_t, 0u);
    Cy_BLE_Start(AppCallBack);
    /* Initialize BLE Services */
    IasInit();
    
    for(;;){
        /* Cy_BLE_ProcessEvents() allows BLE stack to process pending events */
        Cy_BLE_ProcessEvents();
        //vTaskDelay(pdMS_TO_TICKS(100u));
    }
}



/*******************************************************************************
* Function Name: AppCallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*   event      - The event code
*   eventParam - The event parameters
*
*******************************************************************************/
void AppCallBack(uint32 event, void *eventParam)
{
    static cy_stc_ble_gap_sec_key_info_t keyInfo =
    {
        .localKeysFlag    = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST,
        
        .exchangeKeysFlag = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_ENC_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_IRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_CSRK_KEY_DIST,
    };
    
    switch(event)
    {
        /* Mandatory events to be handled by Find Me Target design */
        case CY_BLE_EVT_STACK_ON:
            DEBUG_PRINTF("CY_BLE_EVT_STACK_ON\r\n");
            Cy_BLE_GAP_GenerateKeys(&keyInfo);
            
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:  
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            /* Start BLE advertisement for 180 seconds and update link status on LEDs */
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            IasSetAlertLevel(NO_ALERT);
            break;

        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            /* BLE link is established */
            keyInfo.SecKeyParam.bdHandle = (*(cy_stc_ble_gap_connected_param_t *)eventParam).bdHandle;
            Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
            status_led_commands_t datasend_tt = BLE_NO_ALERT;
            xQueueSend(statusLedCommandQ, &datasend_tt, 0u);
            break;

        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            if(Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_STOPPED)
            {   
                /* Fast and slow advertising period complete, go to low power  
                 * mode (Hibernate) and wait for an external
                 * user event to wake up the device again */
                //UpdateLedState();   
                Cy_BLE_Stop();             
            }
            break;

        /* Other events that are generated by the BLE Component that
         * are not required for functioning of this design */
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CY_BLE_EVT_TIMEOUT: 
            //Cy_SCB_UART_PutString(UART_HW, "CY_BLE_EVT_TIMEOUT\r\n");
//            if((((cy_stc_ble_timeout_param_t *)eventParam)->reasonCode == CY_BLE_GENERIC_APP_TO) && 
//               (((cy_stc_ble_timeout_param_t *)eventParam)->timerHandle == timerParam.timerHandle))
//            {
                /* Update Led State */
                //UpdateLedState();
                
                /* Indicate that timer is raised to the main loop */
                //mainTimer++;
            //}
            break;
            
        case CY_BLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred */
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
            /* Hibernate */
            //Cy_SCB_UART_PutString(UART_HW, "CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE\r\n");
            //UpdateLedState();
            Cy_SysPm_Hibernate();
            break;
            
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CY_BLE_EVT_GAP_AUTH_REQ:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            if(cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].security == 
                (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_1))
            {
                cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].authErr =
                    CY_BLE_GAP_AUTH_ERROR_PAIRING_NOT_SUPPORTED;
            }    
            
            cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].bdHandle = 
                ((cy_stc_ble_gap_auth_info_t *)eventParam)->bdHandle;

            if(Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]) != 
                    CY_BLE_SUCCESS)
            {
                Cy_BLE_GAP_RemoveOldestDeviceFromBondedList();
                Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]);
            }  
            break;
            
        case CY_BLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        case CY_BLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        case CY_BLE_EVT_GAP_AUTH_COMPLETE:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;
        case CY_BLE_EVT_GAP_AUTH_FAILED:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        case CY_BLE_EVT_GAP_ENCRYPT_CHANGE:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;
            
        case CY_BLE_EVT_GAP_KEYS_GEN_COMPLETE:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            keyInfo.SecKeyParam = (*(cy_stc_ble_gap_sec_key_param_t *)eventParam);
            Cy_BLE_GAP_SetIdAddress(&cy_ble_deviceAddress);
            break;

        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CY_BLE_EVT_GATT_CONNECT_IND:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        case CY_BLE_EVT_GATT_DISCONNECT_IND:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            IasSetAlertLevel(NO_ALERT);
            break;

        case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        /**********************************************************
        *                       Other Events
        ***********************************************************/
        case CY_BLE_EVT_PENDING_FLASH_WRITE:
            DEBUG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;

        default:
            break;
    }
}




/* [] END OF FILE */
