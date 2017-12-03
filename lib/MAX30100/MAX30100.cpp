#include "MAX30100.h"
#include "lib/MAXLIB/R_W.h"

void taskFxn(UArg arg0, UArg arg1)
{
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;
    uint8_t         txBuffer[2];
    uint8_t         rxBuffer[1];
    i2cTransaction.slaveAddress = MAX30100_ADDRESS;
    i2cParams.bitRate = I2C_400kHz;

    /* === I2C initialization === */
    I2C_Params_init(&i2cParams);
    i2c = I2C_open(CC2650STK_I2C0, &i2cParams);
    if (i2c == NULL){ System_printf("I2C initialization error\n"); }
    else { System_printf("\nI2C initialized\n\n"); }

    /* === READ & WRITE === */
    readFrom(MAX30100_PART_ID, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    System_printf("Part ID: 0x%x\n", rxBuffer[0]);
    readFrom(MAX30100_REV_ID, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    System_printf("Rev ID: 0x%x\n", rxBuffer[0]);
    writeTo(MAX30100_MODE_CONFIG, MAX30100_SPO2_ON, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    readFrom(MAX30100_MODE_CONFIG, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    if (rxBuffer[0]==0x2) { System_printf("HR only enabled\n"); }
    if (rxBuffer[0]==0x3) { System_printf("SPO2 enabled\n"); }
    writeTo(MAX30100_SPO2_CONFIG, pw200, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    readFrom(MAX30100_SPO2_CONFIG, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    if (rxBuffer[0]==0x00) { System_printf("Brightness level 1\n"); }
    if (rxBuffer[0]==0x01) { System_printf("Brightness level 2\n"); }
    if (rxBuffer[0]==0x10) { System_printf("Brightness level 3\n"); }
    if (rxBuffer[0]==0x11) { System_printf("Brightness level 4\n"); }
    writeTo(MAX30100_LED_CONFIG, 0xFF, txBuffer, rxBuffer, &i2cTransaction, &i2c);         //selected 50 mA & 50 mA
    readFrom(MAX30100_LED_CONFIG, txBuffer, rxBuffer, &i2cTransaction, &i2c);              //ISSUES #1

    /* === I2C Closing === */
    I2C_close(i2c);
    System_printf("\nI2C Closed\n");
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
        System_abort("\nLED pins initialization error\n");
    }

    PIN_setOutputValue(ledPinHandle, Board_LED1, 1);

    System_flush();

    BIOS_start();

    return (0);
}
