/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include "Board.h"

#define TASKSTACKSIZE 1024

/* Global memory storage for a PIN_Config table */
static PIN_State ledPinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_LED2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

void readFrom(uint8_t regAddr, uint8_t *txBuffer, uint8_t *rxBuffer, I2C_Transaction *i2cTransaction, I2C_Handle *i2c)
{
	txBuffer[0] = regAddr;
	i2cTransaction->writeBuf = txBuffer;
	i2cTransaction->writeCount = 1;
	i2cTransaction->readBuf = rxBuffer;
	i2cTransaction->readCount = 1;
	
	if (I2C_transfer(*i2c, &(*i2cTransaction)))
	{
		System_printf("Reading from 0x%x: 0x%x\n", regAddr, rxBuffer[0]);
	}
	else
	{
		System_printf("I2C Bus Fault \n");
	}
	
	System_flush();
}

void writeTo(uint8_t regAddr, uint8_t data, uint8_t *txBuffer, uint8_t *rxBuffer, I2C_Transaction *i2cTransaction, I2C_Handle *i2c)
{
	txBuffer[0] = regAddr;
	txBuffer[1] = data;
	i2cTransaction->writeBuf = txBuffer;
	i2cTransaction->writeCount = 2;
	i2cTransaction->readBuf = rxBuffer;
	i2cTransaction->readCount = 0;
	
	if (I2C_transfer(*i2c, &(*i2cTransaction)))
	{
		System_printf("Writing to 0x%x: 0x%x\n", regAddr, data);
	}
	else
	{
		System_printf("I2C Bus Fault\n");
	}
	
	System_flush();
}

/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void taskFxn(UArg arg0, UArg arg1)
{
	I2C_Handle      i2c;
	I2C_Params      i2cParams;
	I2C_Transaction i2cTransaction;
	uint8_t         txBuffer[2];
	uint8_t         rxBuffer[1];

	/* === I2C initialization === */
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_400kHz;
	i2c = I2C_open(CC2650STK_I2C0, &i2cParams);
	if (i2c == NULL)
	{
		System_printf("I2C initialization error\n");
	}
	else
	{
		System_printf("I2C initialized\n");
	}
	
	System_flush();
	
	/* === MAX30100 slaveAddress === */
	i2cTransaction.slaveAddress = 0x57;
	
	// Part ID
	readFrom(0xFF, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	// Rev ID
	readFrom(0xFE, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	
	// MODE = 010 (0x2) - HR only enabled
	writeTo(0x06, 0x2, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	readFrom(0x06, txBuffer, rxBuffer, &i2cTransaction, &i2c); // check
	
	// IR_PA = 0001, 1000, 1111 (0x1, 0x8, 0xF) - Typical LED current = 4.4 mA, 27.1 mA, 50.0 mA
	writeTo(0x09, 0x1, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	readFrom(0x09, txBuffer, rxBuffer, &i2cTransaction, &i2c); // check

	/* === I2C Closing === */
	I2C_close(i2c);
	System_printf("I2C Closed\n");
	System_flush();
}

/*
 *  ======== main ========
 */
int main(void)
{
    PIN_Handle ledPinHandle;
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initI2C();

    /* Construct tmp007 Task thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle) {
        System_abort("LED pins initialization error\n");
    }

    PIN_setOutputValue(ledPinHandle, Board_LED1, 1);

    System_printf("\nProgram started successfully\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
