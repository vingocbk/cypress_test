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
#include "project.h"
#include "debug.h"

int main(void)
{
    /* Enable global interrupts */
    __enable_irq();
    
    /* Unfreeze IO if device is waking up from hibernate */
//    if(Cy_SysPm_GetIoFreezeStatus())
//    {
//        Cy_SysPm_IoUnfreeze();
//    }
    /* Start the UART debug port */
    UART_DEBUG_START();
    DEBUG_PRINTF("HELLO FROM CORE 0!");


    /* Start the controller portion of BLE. Host runs on the CM4 */
    if(Cy_BLE_Start(NULL) == CY_BLE_SUCCESS)
    {
        DEBUG_PRINTF("Success  : Cortex-M0+ - BLE controller initialization\r\n");
        
        /* Enable CM4 only if BLE Controller started successfully. 
           CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout 
           is changed. */
        Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);
    }
    else
    {
        DEBUG_PRINTF("Failure! : Cortex-M0+ - BLE controller initialization\r\n");
        
        /* Halt the CPU */
        CY_ASSERT(0u);
    }
    
    for (;;)
    {
        /* Process the controller portion of the BLE events and wake up the host
           (CM4) and send data to the host via IPC if necessary */
        Cy_BLE_ProcessEvents();
        
        /* Put CM0+ to Deep Sleep mode. The BLE hardware automatically wakes
            up the CPU if processing is required */
        Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}

/* [] END OF FILE */
