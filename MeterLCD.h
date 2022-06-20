#ifndef MeterLCD_h
#define MeterLCD_h

#include <arduino.h>

class MeterLCD
{
  public:
    MeterLCD(void);
    void begin(void); 
    void clear(void);
    void splash(void);
    void functionDisplay(uint8_t functionNum);
    void displayValue(uint8_t functionNum);
    void updateVFOADisplay(uint32_t *frequency, uint8_t *mode);
    void updateGPSErrorDisplay(int error);
    void indicateVFOA(boolean status);
    void eepromStart(void);
    void eepromNoDataFound(void);
    void eepromInitializing(void);
    void eepromReadingData(void);
    void startSMeterCalibration(void);
    void stopSMeterCalibration(void);
    void calibrateSMeter(void);
    void updateSMeterDsply(uint8_t S_Level);
    void initializeRxDisplay(void);
    void initializeTxDisplay(void);
    void updatePADisplay(void);
    void updateAmpBars(uint8_t level, uint8_t display);
  
  private:
  	String intToASCII(uint16_t value);
  	uint8_t ToASCII(uint8_t value);
};

#endif