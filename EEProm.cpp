/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* EEPROM ROUTINES
*
* Copyright 2021 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* JANUARY 4, 2021
*
******************************************************************************/

#include <EEProm.h>
#include <EEPROMex.h>
#include <GainDial.h>
#include <MeterLCD.h>

GainDial  Gain_Dial;
MeterLCD  Meter_LCD;


typedef struct                         // structure containing specific operating points
{
  uint32_t Freq;
  uint8_t Band;
  uint8_t Mode;
} _FreqBandMode;


typedef struct                        // structure containing variables to be stored in EEPROM
{
  uint16_t      Version;
  uint8_t       EE_AF_GAIN;
  uint8_t       EE_RF_ATTN;
  uint8_t       EE_IF_GAIN;
  uint8_t       EE_LN_GAIN;
  uint8_t       EE_AGC;               // 0=Fast, 1=Slow, 2=Manual IF Gain
  uint8_t       EE_RATE;              // 0=1,000Hz, 1=250Hz, 2=100Hz, 3=10Hz
  uint8_t       EE_PREAMP;            // 0=OFF, 1=ON
  uint16_t      EE_MIXER_1_BAL[11];
  uint16_t      EE_MIXER_1_CLK[11];
  uint16_t      EE_MIXER_2_BAL[11];
  uint16_t      EE_MIXER_2_CLK[11];
  
  _FreqBandMode EE_VFO_A;
  _FreqBandMode EE_VFO_B;
  _FreqBandMode EE_Band_Settings_VFO_A[11];

  uint8_t       EE_VFO_A_Current_Band;          // VFO A current band
  uint8_t       EE_VFO_B_Current_Band;          // VFO B current band

  

} eepromSettings;

eepromSettings storedSettings;


// S-Meter calibration data by band and s-level, currently hard coded not stored in EEPROM 
uint8_t EE_S_METER_TABLE[11][15] = {{199,196,193,190,184,168,153,138,125,105,87,72,57,46,44},   // 160M
                                    {199,196,193,190,178,167,152,137,124,104,87,72,57,46,44},   // 80M
                                    {199,196,193,190,180,164,150,135,122,102,87,72,57,46,44},   // 40M
                                    {199,196,193,190,181,166,150,136,122,103,85,70,57,46,44},   // 30M
                                    {199,196,193,190,182,166,150,136,122,102,85,69,56,46,44},   // 20M
                                    {199,196,193,190,184,168,152,137,124,104,86,71,58,47,44},   // 17M
                                    {199,196,193,190,182,166,150,135,122,102,85,70,57,47,44},   // 15M
                                    {199,196,193,190,185,168,153,138,124,104,86,71,58,47,44},   // 12M
                                    {199,196,193,190,188,188,155,140,126,106,88,72,59,48,44},   // 10aM
                                    {199,196,193,190,186,186,153,137,124,104,86,70,57,48,44},   // 10bM
                                    {199,196,193,190,184,184,152,137,123,103,86,70,57,47,44}};  // 10cM


uint16_t EEpromAddress = 0;           // EEPROM Function data start address


EEProm::EEProm(void)
{

}


void EEProm::begin(void)
{

}


void EEProm::saveSettings(void) // UPDATE EEPROM DATA ARRAY AT POWER DOWN
{
  EEPROM.updateBlock(EEpromAddress, storedSettings);
}


void EEProm::readSettings(uint16_t *currentVersion) // VARIABLES READ FROM EEPROM AT POWER UP
{
    Meter_LCD.eepromStart();
    Meter_LCD.eepromReadingData();
    EEPROM.readBlock(EEpromAddress, storedSettings);   

    if (storedSettings.Version != *currentVersion)  // write default values to EEPROM if new software or Arduino board
    {
      Meter_LCD.eepromNoDataFound();
      Meter_LCD.eepromInitializing();

      storedSettings.Version = *currentVersion;
      storedSettings.EE_AF_GAIN = 0;
      storedSettings.EE_RF_ATTN = 0;
      storedSettings.EE_IF_GAIN = 9;
      storedSettings.EE_LN_GAIN = 64;
      storedSettings.EE_AGC = 0;
      storedSettings.EE_RATE = 1;
      storedSettings.EE_PREAMP = 0;

      storedSettings.EE_VFO_A.Freq = 10137500;
      storedSettings.EE_VFO_A.Band = 3;
      storedSettings.EE_VFO_A.Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[0].Freq = 1841500;
      storedSettings.EE_Band_Settings_VFO_A[0].Band = 0;
      storedSettings.EE_Band_Settings_VFO_A[0].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[1].Freq = 3574500;
      storedSettings.EE_Band_Settings_VFO_A[1].Band = 1;
      storedSettings.EE_Band_Settings_VFO_A[1].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[2].Freq = 7075500;
      storedSettings.EE_Band_Settings_VFO_A[2].Band = 2;
      storedSettings.EE_Band_Settings_VFO_A[2].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[3].Freq = 10137500;
      storedSettings.EE_Band_Settings_VFO_A[3].Band = 3;
      storedSettings.EE_Band_Settings_VFO_A[3].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[4].Freq = 14075500;
      storedSettings.EE_Band_Settings_VFO_A[4].Band = 4;
      storedSettings.EE_Band_Settings_VFO_A[4].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[5].Freq = 18101500;
      storedSettings.EE_Band_Settings_VFO_A[5].Band = 5;
      storedSettings.EE_Band_Settings_VFO_A[5].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[6].Freq = 21075500;
      storedSettings.EE_Band_Settings_VFO_A[6].Band = 6;
      storedSettings.EE_Band_Settings_VFO_A[6].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[7].Freq = 24916500;
      storedSettings.EE_Band_Settings_VFO_A[7].Band = 7;
      storedSettings.EE_Band_Settings_VFO_A[7].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[8].Freq = 28075500;
      storedSettings.EE_Band_Settings_VFO_A[8].Band = 8;
      storedSettings.EE_Band_Settings_VFO_A[8].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[9].Freq = 28501500;
      storedSettings.EE_Band_Settings_VFO_A[9].Band = 9;
      storedSettings.EE_Band_Settings_VFO_A[9].Mode = 0;

      storedSettings.EE_Band_Settings_VFO_A[10].Freq = 2901500;
      storedSettings.EE_Band_Settings_VFO_A[10].Band = 10;
      storedSettings.EE_Band_Settings_VFO_A[10].Mode = 0;


      for(uint8_t i = 0; i < 11; i++)  
    {      
        storedSettings.EE_MIXER_1_BAL[i] = 0x0800;    // Set Mixer ADCs to mid-range setting 2,048
        storedSettings.EE_MIXER_1_CLK[i] = 0x0800;
        storedSettings.EE_MIXER_2_BAL[i] = 0x0800;
        storedSettings.EE_MIXER_2_CLK[i] = 0x0800;
    }
      saveSettings();
    }
}

uint8_t EEProm::getAF_Gain(void)
{
  return storedSettings.EE_AF_GAIN;
}


void EEProm::storeAF_Gain(uint8_t gain)
{
  storedSettings.EE_AF_GAIN = gain;
}


uint8_t EEProm::getRF_Attn(void)
{
  return storedSettings.EE_RF_ATTN;
}


void EEProm::storeRF_Attn(uint8_t gain)
{
  storedSettings.EE_RF_ATTN = gain;
}


uint8_t EEProm::getIF_Gain(void)
{
  return storedSettings.EE_IF_GAIN;
}


void EEProm::storeIF_Gain(uint8_t gain)
{
  storedSettings.EE_IF_GAIN = gain;
}


uint8_t EEProm::getLN_Gain(void)
{
  return storedSettings.EE_LN_GAIN;
}


void EEProm::storeLN_Gain(uint8_t gain)
{
  storedSettings.EE_LN_GAIN = gain;
}


uint32_t EEProm::getVFOA_Freq(void)
{
  return storedSettings.EE_VFO_A.Freq;
}


void EEProm::storeVFOA_Freq(uint32_t frequency)
{
  storedSettings.EE_VFO_A.Freq = frequency;
}


uint8_t EEProm::getVFOA_Band(void)
{
  return storedSettings.EE_VFO_A.Band;
}


void EEProm::storeVFOA_Band(uint8_t band)
{
  storedSettings.EE_VFO_A.Band = band;
}


uint8_t EEProm::getVFOA_Mode(void)
{
  return storedSettings.EE_VFO_A.Mode;
}


void EEProm::storeVFOA_Mode(uint8_t mode)
{
  storedSettings.EE_VFO_A.Mode = mode;
}


uint32_t EEProm::getBand_Freq(uint8_t band)
{
  return (storedSettings.EE_Band_Settings_VFO_A[band].Freq);
}


void EEProm::storeBand_Freq(uint32_t frequency, uint8_t band)
{
  storedSettings.EE_Band_Settings_VFO_A[band].Freq = frequency;
}


uint8_t EEProm::getBand_Mode(uint8_t band)
{
  return (storedSettings.EE_Band_Settings_VFO_A[band].Mode);
}


void EEProm::storeBand_Mode(uint8_t mode, uint8_t band)
{
  storedSettings.EE_Band_Settings_VFO_A[band].Mode = mode;
}


uint8_t EEProm::getAGC(void)
{
  return storedSettings.EE_AGC;
}


void EEProm::storeAGC(uint8_t agc)
{
  storedSettings.EE_AGC = agc;
}


uint8_t EEProm::getRate(void)
{
  return storedSettings.EE_RATE;
}


void EEProm::storeRate(uint8_t rate)
{
  storedSettings.EE_RATE = rate;
}


uint8_t EEProm::getPreAmp(void)
{
  return storedSettings.EE_PREAMP;
}


void EEProm::storePreAmp(uint8_t preamp)
{
  storedSettings.EE_PREAMP = preamp;
}

uint16_t EEProm::getMixer1Bal(void)
{
  return storedSettings.EE_MIXER_1_BAL[storedSettings.EE_VFO_A.Band];
}


uint16_t EEProm::getMixer1Clk(void)
{
  return storedSettings.EE_MIXER_1_CLK[storedSettings.EE_VFO_A.Band];
}


uint16_t EEProm::getMixer2Bal(void)
{
  return storedSettings.EE_MIXER_2_BAL[storedSettings.EE_VFO_A.Band];
}


uint16_t EEProm::getMixer2Clk(void)
{
  return storedSettings.EE_MIXER_2_CLK[storedSettings.EE_VFO_A.Band];
}

void EEProm::storeMixer1Bal(uint16_t ADC_setting)
{
  storedSettings.EE_MIXER_1_BAL[storedSettings.EE_VFO_A.Band] = ADC_setting;
}


void EEProm::storeMixer1Clk(uint16_t ADC_setting)
{
  storedSettings.EE_MIXER_1_CLK[storedSettings.EE_VFO_A.Band] = ADC_setting;
}


void EEProm::storeMixer2Bal(uint16_t ADC_setting)
{
  storedSettings.EE_MIXER_2_BAL[storedSettings.EE_VFO_A.Band] = ADC_setting;
}


void EEProm::storeMixer2Clk(uint16_t ADC_setting)
{
  storedSettings.EE_MIXER_2_CLK[storedSettings.EE_VFO_A.Band] = ADC_setting;
}