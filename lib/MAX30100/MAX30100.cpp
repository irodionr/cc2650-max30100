#include "MAX30100.h"
#include "lib/MAXLIB/R_W.h"

uint8_t returnFrom(uint8_t regAddr, uint8_t *txBuffer, uint8_t *rxBuffer, I2C_Transaction *i2cTransaction, I2C_Handle *i2c)
{

    txBuffer[0] = regAddr;
    i2cTransaction->writeBuf = txBuffer;
    i2cTransaction->writeCount = 1;
    i2cTransaction->readBuf = rxBuffer;
    i2cTransaction->readCount = 1;

    if (I2C_transfer(*i2c, &(*i2cTransaction)))
    {
        return rxBuffer[0];
    }

	return 0x0;
}

void taskFxn(UArg arg0, UArg arg1)
{
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;
    uint8_t         txBuffer[2];
    uint8_t         rxBuffer[1];
	uint8_t			tmp;
	uint16_t		dataIR;
	uint16_t		dataR;
    i2cTransaction.slaveAddress = MAX30100_ADDRESS;
    i2cParams.bitRate = I2C_400kHz;
	int i;
	
    /* === I2C initialization === */
    I2C_Params_init(&i2cParams);
    i2c = I2C_open(CC2650STK_I2C0, &i2cParams);
    if (i2c == NULL){ System_printf("I2C initialization error\n"); }
    else { System_printf("\nI2C initialized\n\n"); }

    writeTo(MAX30100_MODE_CONFIG, 0x40, txBuffer, rxBuffer, &i2cTransaction, &i2c); //reset

	/* === Measuring heart rate === */
    writeTo(MAX30100_LED_CONFIG, 0xFF, txBuffer, rxBuffer, &i2cTransaction, &i2c); //LED current = 50 mA
    writeTo(MAX30100_SPO2_CONFIG, 0x7, txBuffer, rxBuffer, &i2cTransaction, &i2c); //sample rate = 100 sps, LED pulse width = 1600 us, ADC resolution = 16 bits
	writeTo(MAX30100_MODE_CONFIG, 0x2, txBuffer, rxBuffer, &i2cTransaction, &i2c); //mode = HR only
	writeTo(MAX30100_INT_ENABLE, 0xA0, txBuffer, rxBuffer, &i2cTransaction, &i2c); //enable ALMOST_FULL and HR_READY interrupts
	
	writeTo(MAX30100_FIFO_WR_PTR, 0x0, txBuffer, rxBuffer, &i2cTransaction, &i2c); //clear write pointer
	writeTo(MAX30100_FIFO_RD_PTR, 0x0, txBuffer, rxBuffer, &i2cTransaction, &i2c); //clear read pointer

	readFrom(MAX30100_OVRFLOW_CTR, txBuffer, rxBuffer, &i2cTransaction, &i2c);

	for (i = 0; i < 128; i++) { //number of samples to read
	    tmp = returnFrom(MAX30100_INT_STATUS, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	    tmp &= 0x20; //0b0010000 - HR_RDY flag

	    while (tmp != 0x20) { //waiting for HR_RDY flag in INTERRUPT_STATUS
			tmp = returnFrom(MAX30100_INT_STATUS, txBuffer, rxBuffer, &i2cTransaction, &i2c);
			tmp &= 0x20; //0b0010000 - HR_RDY flag
			
			System_printf("Waiting for HR_RDY... \n");
			System_flush();
		}
		
		readFrom(MAX30100_OVRFLOW_CTR, txBuffer, rxBuffer, &i2cTransaction, &i2c);
		readFrom(MAX30100_INT_STATUS, txBuffer, rxBuffer, &i2cTransaction, &i2c);

		dataIR = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c); //1st byte - IR data for HR
		dataIR = (dataIR << 8) + returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c); //2nd byte - IR data for HR
		dataR = returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c); //3rd byte
		dataR = (dataR << 8) + returnFrom(MAX30100_FIFO_DATA, txBuffer, rxBuffer, &i2cTransaction, &i2c); //4th byte
		
		readFrom(MAX30100_INT_STATUS, txBuffer, rxBuffer, &i2cTransaction, &i2c);

		System_printf("%d\n", dataIR, dataIR);
		//System_printf("R data: 0x%x\n", dataR);
		System_flush();

		readFrom(MAX30100_FIFO_WR_PTR, txBuffer, rxBuffer, &i2cTransaction, &i2c);
		readFrom(MAX30100_OVRFLOW_CTR, txBuffer, rxBuffer, &i2cTransaction, &i2c);
		readFrom(MAX30100_FIFO_RD_PTR, txBuffer, rxBuffer, &i2cTransaction, &i2c);
	}
	
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
