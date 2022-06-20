#ifndef FuncLCD_h
#define FuncLCD_h

#include <arduino.h>

#define ATTN_Cmnd       0x30		// function related commands
#define AMP_Cmnd        0x40
#define BAND_Cmnd       0x50
#define MODE_Cmnd       0x60
#define AGC_Cmnd        0x70
#define RATE_Cmnd       0xA0
#define TR_Cmnd			    0xB0
#define FUNC_Cmnd		    0xC0
#define GAIN_Cmnd		    0xD0
#define SCAL_Cmnd       0xE0
#define GPS_Cmnd        0xF0

class FuncLCD
{
  public:
    FuncLCD(void);
    void FuncLCD::begin(void); 
    void FuncLCD::clear(void);
    void FuncLCD::splash(void);
    void FuncLCD::init(uint8_t _AGC, uint8_t _RATE, uint8_t _AMP, uint8_t _MODE, uint8_t _BAND, uint8_t _TR);
    void FuncLCD::update(uint8_t command);
    void FuncLCD::dsplyGain(int8_t gain);
    void FuncLCD::blankGain(void);
    void FuncLCD::setI2CForLCD(void);
    
  
  private:

};

#endif