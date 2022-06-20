#ifndef GainDial_h
#define GainDial_h

#include <arduino.h>


class GainDial
{
  public:
    GainDial(void);
    void begin(void); 
    void select(void);
    void CW(void);
    void CCW(void);
    uint8_t getManIFGain(void);

  
  private:
  	void incrementAFGain(void);
  	void decrementAFGain(void);
  	void incrementRFAttn(void);
  	void decrementRFAttn(void);
  	void incrementIFGain(void);
  	void decrementIFGain(void);
  	void incrementLineGain(void);
  	void decrementLineGain(void);
  	void setAFGain(uint8_t level);
  	void setLineGain(uint8_t level);
  	void disableZeroCrossingDetection(void);
    void setRFAttn(uint8_t level);
    void setIFGain(uint8_t level);
    void setI2CForDS1807(void);
};

#endif