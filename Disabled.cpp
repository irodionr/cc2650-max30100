// ==============MAX30100.cpp=====================

//void taskFxn(UArg arg0, UArg arg1)
//{

    // MODE = 010 (0x2) - HR only enabled
/*  writeTo(0x06, 0x2, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    readFrom(0x06, txBuffer, rxBuffer, &i2cTransaction, &i2c); */

    // (SPO2_HI_RES_EN = 0x3F - Maximum brightness of the led
 /* writeTo(0x07, 0x3F, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    readFrom(0x07, txBuffer, rxBuffer, &i2cTransaction, &i2c); */           //temporarily disabled

    // IR_PA (Invisible) = 0001, 1000, 1111 (0x1, 0x8, 0xF) - Typical LED current = 4.4 mA, 27.1 mA, 50.0 mA
/*  writeTo(0x09, 0xF1, txBuffer, rxBuffer, &i2cTransaction, &i2c);
    readFrom(0x09, txBuffer, rxBuffer, &i2cTransaction, &i2c); */           //temporarily disabled

//}

// ===============MAX30100.h========================

/*class MAX30100 {
public:
//  static uint16_t IR = 0;      // Last IR reflectance datapoint
//  uint16_t RED = 0;     // Last Red reflectance datapoint

  MAX30100();
  void  setLEDs(pulseWidth pw, ledCurrent red, ledCurrent ir);  // Sets the LED state
  void  setSPO2(sampleRate sr); // Setup the SPO2 sensor, disabled by default
  int   getNumSamp(void);       // Get number of samples
  void  readSensor(void);       // Updates the values
  void  shutdown(void);   // Instructs device to power-save
  void  reset(void);      // Resets the device
  void  startup(void);    // Leaves power-save
  int   getRevID(void);   // Gets revision ID
  int   getPartID(void);  // Gets part ID
  void  begin(pulseWidth pw = pw200, // Longest pulseWidth
              ledCurrent ir = i50,    // Highest current
              sampleRate sr = sr100); // 2nd lowest sampleRate
  void  printRegisters(void); // Dumps contents of registers for debug

private:
  void    I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data);
  uint8_t I2CreadByte(uint8_t address, uint8_t subAddress);
  void    I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);
};

*/
