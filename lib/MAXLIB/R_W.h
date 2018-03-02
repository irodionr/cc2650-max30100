#ifndef R_W_H_
#define R_W_H_

void readFrom(uint8_t regAddr, uint8_t *txBuffer, uint8_t *rxBuffer, I2C_Transaction *i2cTransaction, I2C_Handle *i2c)
{

    txBuffer[0] = regAddr;
    i2cTransaction->writeBuf = txBuffer;
    i2cTransaction->writeCount = 1;
    i2cTransaction->readBuf = rxBuffer;
    i2cTransaction->readCount = 1;

    if (I2C_transfer(*i2c, &(*i2cTransaction)))
    {
        System_printf("0x%x: 0x%x\n", regAddr, rxBuffer[0]);
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
//        System_printf("Writing to 0x%x: 0x%x\n", regAddr, data);
    }
    else
    {
        System_printf("I2C Bus Fault\n");
    }

    System_flush();
}



#endif /* R_W_H_ */
