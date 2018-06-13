/******************************************************************************

 @file  main.c

 @brief main entry of the BLE stack sample application.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************

 Copyright (c) 2013-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_01_18
 Release Date: 2016-05-06 22:17:28
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */


#include "MAX30100.h"
#include "MAXLIB/filter.h"
#include "MAXLIB/R_W.h"

#include <xdc/runtime/Error.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/sysbios/BIOS.h>

#include "icall.h"
#include "hal_assert.h"
#include "bcomdef.h"
#include "peripheral.h"

#include <ti/drivers/UART.h>
#include <uart_logs.h>

/* Header files required to enable instruction fetch cache */
#include <inc/hw_memmap.h>
#include <driverlib/vims.h>

#ifndef USE_DEFAULT_USER_CFG

#include "ble_user_config.h"

// BLE user defined configuration
bleUserCfg_t user0Cfg = BLE_USER_CFG;

#endif // USE_DEFAULT_USER_CFG

#include <ti/mw/display/Display.h>

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * EXTERNS
 */

extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

//extern Display_Handle dispHandle;

void taskFxn(UArg arg0, UArg arg1)
{
	I2C_Handle      i2c;
	I2C_Params      i2cParams;
	I2C_Transaction i2cTransaction;
	uint8_t         txBuffer[2];
	uint8_t         rxBuffer[1];
	uint8_t         buffer[4];
	uint8_t         temp;
	float           dataIR[SIZE];
	float           dataR[SIZE];
	float           hr;
	float           o2;
	int i;
	i2cTransaction.slaveAddress = MAX30100_ADDRESS;
	i2cParams.bitRate = I2C_400kHz;

	/* === I2C initialization === */
	I2C_Params_init(&i2cParams);
	i2c = I2C_open(CC2650_LAUNCHXL_I2C0, &i2cParams);

	writeTo(MAX30100_MODE_CONFIG, 0x40, txBuffer, rxBuffer, &i2cTransaction, &i2c); //reset

	/* === Measuring heart rate === */
	// increase RED LED current until IR and RED DC levels match
	writeTo(MAX30100_LED_CONFIG, 0x87, txBuffer, rxBuffer, &i2cTransaction, &i2c); //LED current = 27.1 mA for RED, 24.0 mA for IR
	writeTo(MAX30100_SPO2_CONFIG, 0x7, txBuffer, rxBuffer, &i2cTransaction, &i2c); //sample rate = 100 Hz, LED pulse width = 1600 us, ADC resolution = 16 bits
	writeTo(MAX30100_MODE_CONFIG, 0x3, txBuffer, rxBuffer, &i2cTransaction, &i2c); //mode = SPO2
	writeTo(MAX30100_INT_ENABLE, 0x10, txBuffer, rxBuffer, &i2cTransaction, &i2c); //enable SPO2_RDY interrupt

	System_printf("Ready\n");

	for (i = 0; i < 20; i++) { // first 10 to 15 readings are incorrect
	    // wait for data
        temp = 0x0;
        while (temp == 0x0) {
            temp = returnFrom(MAX30100_INT_STATUS, txBuffer, rxBuffer, &i2cTransaction, &i2c);
            temp &= 0x10;
        }

        // read 4 bytes
        buffer[0] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
        buffer[1] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
        buffer[2] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
        buffer[3] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	}
	
	for (i = 0; i < SIZE; i++) {
        // wait for data
		temp = 0x0;
        while (temp == 0x0) {
            temp = returnFrom(MAX30100_INT_STATUS, txBuffer, rxBuffer, &i2cTransaction, &i2c);
            temp &= 0x10;
        }

		// read 4 bytes
        buffer[0] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
        buffer[1] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
        buffer[2] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
        buffer[3] = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);

		// form bytes into data
        dataIR[i] = (float)((buffer[0] << 8) | buffer[1]);
        dataR[i] = (float)((buffer[2] << 8) | buffer[3]);
	}
	
	threshold(dataIR, 1000.0);
	threshold(dataR, 1000.0);
	
	dc(dataIR, 0.95);
	dc(dataR, 0.95);
	
	o2 = spo2(dataIR, dataR);
	if (o2 < 70.0) {
		o2 = 0.0;
	} else if (o2 > 100.0) {
		o2 = 1.0;
	}
	
	meanMedian(dataIR, 15);
	butterworth(dataIR);
	
	hr = heartrate(dataIR);
	if (hr < 30.0) {
		hr = 0.0;
	} else if (hr > 210.0) {
		hr = 1.0;
	}
	
	/* === I2C closing === */
	I2C_close(i2c);
	System_printf("\nI2C Closed\n");
	System_flush();
}

/*******************************************************************************
 * @fn          Main
 *
 * @brief       Application Main
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
int main()
{
  /* Register Application callback to trap asserts raised in the Stack */
  RegisterAssertCback(AssertHandler);

  PIN_init(BoardGpioInitTable);

#ifndef POWER_SAVING
  /* Set constraints for Standby, powerdown and idle mode */
  Power_setConstraint(PowerCC26XX_SB_DISALLOW);
  Power_setConstraint(PowerCC26XX_IDLE_PD_DISALLOW);
#endif // POWER_SAVING

  /* Initialize the RTOS Log formatting and output to UART in Idle thread.
   * Note: Define xdc_runtime_Log_DISABLE_ALL to remove all impact of Log.
   * Note: NULL as Params gives 115200,8,N,1 and Blocking mode */
  UART_init();
  UartLog_init(UART_open(Board_UART, NULL));

  /* Initialize ICall module */
  ICall_init();

  /* Start tasks of external images - Priority 5 */
  ICall_createRemoteTasks();

  /* Kick off profile - Priority 3 */
  GAPRole_createTask();

  //ProjectZero_createTask(); 					//DELETED

  PIN_Handle ledPinHandle;
  Task_Params taskParams;

  /* Call board init functions */
  Board_initGeneral();
  Board_initI2C();

  /* Construct tmp007 Task thread */
  Task_Params_init(&taskParams);
  taskParams.stackSize = TASKSTACKSIZE; // can be changed in Service.h
  taskParams.stack = &task0Stack;
  Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

  /* Open LED pins */
  ledPinHandle = PIN_open(&ledPinState, ledPinTable);
  if (!ledPinHandle) {
  	System_abort("\nLED pins initialization error\n");
  }

  PIN_setOutputValue(ledPinHandle, Board_LED1, 1);

  System_flush();
  
  /* enable interrupts and start SYS/BIOS */
  BIOS_start();

  return 0;
}


/*******************************************************************************
 * @fn          AssertHandler
 *
 * @brief       This is the Application's callback handler for asserts raised
 *              in the stack.
 *
 * input parameters
 *
 * @param       assertCause    - Assert cause as defined in hal_assert.h.
 * @param       assertSubcause - Optional assert subcause (see hal_assert.h).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void AssertHandler(uint8 assertCause, uint8 assertSubcause)
{
  // Open the display if the app has not already done so
/*  if ( !dispHandle )
  {
    dispHandle = Display_open(Display_Type_LCD, NULL);
  }

  Display_print0(dispHandle, 0, 0, ">>>STACK ASSERT");*/

  // check the assert cause
  switch (assertCause)
  {
    case HAL_ASSERT_CAUSE_OUT_OF_MEMORY:
      Display_print0(dispHandle, 0, 0, "***ERROR***");
      Display_print0(dispHandle, 2, 0, ">> OUT OF MEMORY!");
      break;

    case HAL_ASSERT_CAUSE_INTERNAL_ERROR:
      // check the subcause
      if (assertSubcause == HAL_ASSERT_SUBCAUSE_FW_INERNAL_ERROR)
      {
        Display_print0(dispHandle, 0, 0, "***ERROR***");
        Display_print0(dispHandle, 2, 0, ">> INTERNAL FW ERROR!");
      }
      else
      {
        Display_print0(dispHandle, 0, 0, "***ERROR***");
        Display_print0(dispHandle, 2, 0, ">> INTERNAL ERROR!");
      }
      break;

    case HAL_ASSERT_CAUSE_ICALL_ABORT:
      Display_print0(dispHandle, 0, 0, "***ERROR***");
      Display_print0(dispHandle, 2, 0, ">> ICALL ABORT!");
      HAL_ASSERT_SPINLOCK;
      break;

    default:
      Display_print0(dispHandle, 0, 0, "***ERROR***");
      Display_print0(dispHandle, 2, 0, ">> DEFAULT SPINLOCK!");
      HAL_ASSERT_SPINLOCK;
  }

  return;
}


/*******************************************************************************
 * @fn          smallErrorHook
 *
 * @brief       Error handler to be hooked into TI-RTOS.
 *
 * input parameters
 *
 * @param       eb - Pointer to Error Block.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void smallErrorHook(Error_Block *eb)
{
  for (;;);
}


/*******************************************************************************
 */
