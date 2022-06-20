#ifndef ProdDetAudio_Board_h
#define ProdDetAudio_Board_h


class ProdDetAudio_Board
{
  public:
  	ProdDetAudio_Board(void);
  	void begin(void);
    void enableBFO(void);
    void disableBFO(void);
    void enableCarrOsc(void);
    void disableCarrOsc(void);
    void setBFO_CarrierOscMode(uint8_t mode);
    void sendI2C_1_Byte(uint8_t addr, uint8_t data);
    
  
  private:
    void setI2CForPDABrd(void);
};


#endif