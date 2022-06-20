#ifndef Mixer_Board_v2_h
#define Mixer_Board_v2_h


class Mixer_Board_v2
{
  public:
  	Mixer_Board_v2(void);
  	void begin(void);
    void vfoUpdate(uint32_t *freq);
    void txMode(void);
    void rxMode(void);
    void Mixer_Board_v2::readGPSFreqCorrection(void);
  
  private:
    void setFirstLO(uint64_t Freq);
    void setSecondLO(uint64_t Freq);
    void setI2CForMxrBrd(void);
    void sendI2C_1_Byte(uint8_t addr, uint8_t data);
};


#endif