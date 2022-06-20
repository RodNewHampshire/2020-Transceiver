#ifndef EEProm_h
#define EEPRom_h

#include <arduino.h>


class EEProm
{
  public:
    EEProm(void);
    void EEProm::begin(void);
    void EEProm::saveSettings(void);
    void EEProm::readSettings(uint16_t *currentVersion);
    uint8_t EEProm::getAF_Gain(void);
    uint8_t EEProm::getRF_Attn(void);
    uint8_t EEProm::getIF_Gain(void);
    uint8_t EEProm::getLN_Gain(void);
    void EEProm::storeAF_Gain(uint8_t gain);
    void EEProm::storeRF_Attn(uint8_t gain);
    void EEProm::storeIF_Gain(uint8_t gain);
    void EEProm::storeLN_Gain(uint8_t gain);
    uint32_t EEProm::getVFOA_Freq(void);
    uint8_t EEProm::getVFOA_Band(void);
    uint8_t EEProm::getVFOA_Mode(void);
    uint8_t EEProm::getAGC(void);
    uint8_t EEProm::getRate(void);
    uint8_t EEProm::getPreAmp(void);
    void EEProm::storeVFOA_Freq(uint32_t frequency);
    void EEProm::storeVFOA_Band(uint8_t band);
    void EEProm::storeVFOA_Mode(uint8_t mode);
    uint32_t EEProm::getBand_Freq(uint8_t band);
    void EEProm::storeBand_Freq(uint32_t frequency, uint8_t band);
    uint8_t EEProm::getBand_Mode(uint8_t band);
    void EEProm::storeBand_Mode(uint8_t mode, uint8_t band);
    void EEProm::storeAGC(uint8_t agc);
    void EEProm::storeRate(uint8_t rate);
    void EEProm::storePreAmp(uint8_t preamp);
    uint16_t EEProm::getMixer1Bal(void);
    uint16_t EEProm::getMixer1Clk(void);
    uint16_t EEProm::getMixer2Bal(void);
    uint16_t EEProm::getMixer2Clk(void);
    void EEProm::storeMixer1Bal(uint16_t ADC_setting);
    void EEProm::storeMixer1Clk(uint16_t ADC_setting);
    void EEProm::storeMixer2Bal(uint16_t ADC_setting);
    void EEProm::storeMixer2Clk(uint16_t ADC_setting);
    
  
  private:
};

#endif