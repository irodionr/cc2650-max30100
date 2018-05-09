#include "MAX30100.h"
#include "lib/MAXLIB/R_W.h"
#include "lib/MAXLIB/filter.h"

void taskFxn(UArg arg0, UArg arg1)
{
	I2C_Handle      i2c;
	I2C_Params      i2cParams;
	I2C_Transaction i2cTransaction;
	uint8_t         txBuffer[2];
	uint8_t         rxBuffer[1];
	uint8_t         buffer[4];
	uint8_t         temp;
	uint16_t        dataIR[SIZE];
	uint16_t        dataR[SIZE];
	int i;
	i2cTransaction.slaveAddress = MAX30100_ADDRESS;
	i2cParams.bitRate = I2C_400kHz;

	/* === I2C initialization === */
	I2C_Params_init(&i2cParams);
	i2c = I2C_open(CC2650STK_I2C0, &i2cParams);

	writeTo(MAX30100_MODE_CONFIG, 0x40, txBuffer, rxBuffer, &i2cTransaction, &i2c); //reset

	/* === Measuring heart rate === */
	// increase RED LED current until IR and RED DC levels match
	writeTo(MAX30100_LED_CONFIG, 0x87, txBuffer, rxBuffer, &i2cTransaction, &i2c); //LED current = 24.0 mA
	writeTo(MAX30100_SPO2_CONFIG, 0x7, txBuffer, rxBuffer, &i2cTransaction, &i2c); //sample rate = 100 Hz, LED pulse width = 1600 us, ADC resolution = 16 bits
	writeTo(MAX30100_MODE_CONFIG, 0x3, txBuffer, rxBuffer, &i2cTransaction, &i2c); //mode = SPO2
	writeTo(MAX30100_INT_ENABLE, 0x10, txBuffer, rxBuffer, &i2cTransaction, &i2c); //enable SPO2_RDY interrupt

	for (i = 0; i < 20; i++) { // first 10 to 15 readings are incorrect
		returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	}
	
	System_printf("Ready\n");
	
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
        dataIR[i] = (buffer[0] << 8) | buffer[1];
        dataR[i] = (buffer[2] << 8) | buffer[3];
	}
	
	threshold(dataIR, 500.0);
	threshold(dataR, 500.0);
	
	dc(dataIR, 0.95);
	dc(dataR, 0.95);
	
	meanMedian(dataIR, 15);
	meanMedian(dataR, 15);
	
	butterworth(dataIR);
	butterworth(dataR);

	/* === I2C closing === */
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

	BIOS_start();

	return (0);
}
