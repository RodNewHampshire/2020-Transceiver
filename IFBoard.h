#ifndef IFBoard_h
#define IFBoard_h

#include <arduino.h>
#include <Wire.h>

class IFBoard
{
  public:
  	IFBoard(void);
  	void begin(void);
    uint8_t getSMeterCalValue(void);
  	void updateSMeter(void);
  	void setAGCMode(uint8_t);
  	void setManIFGain(uint8_t level);
  
  private:
    void setI2CForIFBrd(void);
};


#endif