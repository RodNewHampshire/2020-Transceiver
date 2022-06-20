/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* Frequency Control Routines
*
* Copyright 2021 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* FEBRUARY 28, 2021
*
******************************************************************************/

#include <AmpLink.h>
#include <arduino.h>
#include <EEProm.h>
#include <FreqControl.h>
#include <FuncLCD.h>
#include <MeterLCD.h>
#include <Mixer_Board_v2.h>
#include <ProdDetAudio_Board.h>

#define UPPER         1
#define LOWER         0

AmpLink             __ampLink;
EEProm 			        __eeProm;
FuncLCD 		        __FuncLCD;
MeterLCD		        __MeterLCD;
Mixer_Board_v2      __MixerBoard;
ProdDetAudio_Board  __ProdDetAudio;

//Index to band arrays:
//0=160M, 1=80M, 2=40M, 3=30M, 4=20M, 5=17M, 6=15M, 7=12M, 8=10Ma, 9=10Mb, 10=10Mc

uint32_t _VFOA_Band_Limits[2], _VFOB_Band_Limits[2];

uint8_t S_METER_CAL[15];
uint8_t S_METER_TABLE[11][15];
extern uint8_t EE_S_METER_TABLE[11][15];

uint16_t _dummyFreqUpdateAccum = 0;

typedef struct                         // structure containing specific operating parameters
{
  uint32_t Freq;
  uint8_t Band;
  uint8_t Mode;
} __FreqBandMode;

__FreqBandMode _VFO_A;

uint32_t correctedFreq;

int16_t FREQ_CORRECTION[11] = {-350, 0, 50, 200, 500, 500, 750, 750, 750, 750, 200};


uint8_t   rate;
uint16_t  tuningRate;
uint16_t  tuningRateTable[4] = {1000, 250, 100, 10};


FreqControl::FreqControl(void)
{

}


void FreqControl::begin(void)
{
  for(uint8_t band=0; band<11; band++)                // Copy S-meter calibration table from EEPROM
  {
    for(uint8_t sLevel=0; sLevel<15; sLevel++)
    {
      S_METER_TABLE[band][sLevel] = EE_S_METER_TABLE[band][sLevel];
    }
  }
  rate = __eeProm.getRate();
  tuningRate = tuningRateTable[rate];
}


void FreqControl::initVFOA(void)
{
  _VFO_A.Freq = __eeProm.getVFOA_Freq();
  _VFO_A.Band = __eeProm.getVFOA_Band();
  _VFO_A.Mode = __eeProm.getVFOA_Mode();
  Set_Band_Limits((long*)&_VFOA_Band_Limits, _VFO_A.Band);
  for(uint8_t n=0; n<15; n++) S_METER_CAL[n] = S_METER_TABLE[_VFO_A.Band][n];
  updateVFOA(&_dummyFreqUpdateAccum);
}

/*
int32_t FreqControl::getVFOA_Dial_Freq(void)
{
    int32_t dialFreq;

    if(VFO_A.Mode == 0) dialFreq = VFO_A.Freq - 1500;
    else if (VFO_A.Mode == 1) dialFreq = VFO_A.Freq + 1500;
    else dialFreq = VFO_A.Freq;

    return(dialFreq);
}*/

/*
int32_t FreqControl::getVFOB_Dial_Freq(void)
{
    int32_t dialFreq;

    if(VFO_B.Mode == 0) dialFreq = VFO_B.Freq - 1500;
    else if (VFO_B.Mode == 1) dialFreq = VFO_B.Freq + 1500;
    else dialFreq = VFO_B.Freq;

    return(dialFreq);
}*/


void FreqControl::bandChangeVFOA(uint8_t newBand)
{
    __eeProm.storeBand_Freq(_VFO_A.Freq, _VFO_A.Band);		      // save current band frequency & mode
    __eeProm.storeBand_Mode(_VFO_A.Mode, _VFO_A.Band);

    _VFO_A.Band = newBand;									                    // get new band frequency & mode
    _VFO_A.Freq = __eeProm.getBand_Freq(newBand);			
    _VFO_A.Mode = __eeProm.getBand_Mode(newBand);

    __eeProm.storeVFOA_Freq(_VFO_A.Freq);					              // store new VFOA freq, band, & mode
    __eeProm.storeVFOA_Band(_VFO_A.Band);
    __eeProm.storeVFOA_Mode(_VFO_A.Mode);

    Set_Band_Limits((long*)&_VFOA_Band_Limits, _VFO_A.Band);	  // set new band limits
    
    __FuncLCD.update(BAND_Cmnd + _VFO_A.Band);				          // update band display
    __FuncLCD.update(MODE_Cmnd + _VFO_A.Mode);				          // update mode display
    
    updateVFOA(&_dummyFreqUpdateAccum);						              // update VFOA
    __ampLink.updateXmtBand();                                  // update Amplifier band

    for(uint8_t n=0; n<15; n++) S_METER_CAL[n] = S_METER_TABLE[newBand][n];

}


void FreqControl::updateVFOA(int16_t *_freqUpdate)
{
  _VFO_A.Freq = _VFO_A.Freq + *_freqUpdate;
  if (_VFO_A.Freq < _VFOA_Band_Limits[LOWER]) _VFO_A.Freq = _VFOA_Band_Limits[LOWER];
  if (_VFO_A.Freq > _VFOA_Band_Limits[UPPER]) _VFO_A.Freq = _VFOA_Band_Limits[UPPER];
  correctedFreq = _VFO_A.Freq + FREQ_CORRECTION[_VFO_A.Band];
  __MixerBoard.vfoUpdate(&correctedFreq);
  __MeterLCD.updateVFOADisplay(&_VFO_A.Freq, &_VFO_A.Mode);
  __ProdDetAudio.setBFO_CarrierOscMode(_VFO_A.Mode);
  __eeProm.storeVFOA_Freq(_VFO_A.Freq);
}


void FreqControl::setTuningRate(uint8_t rateUpdate)
{
  tuningRate = tuningRateTable[rateUpdate];
}


/*
void FreqControl::decrementBandVFOA(void)
{
  if (VFO_A.Band == 0)VFO_A.Band = 10;
  else VFO_A.Band--;
  bandChangeVFOA();
  updateVFOA(&dummyFreqUpdateAccum);
}*/

/*
void FreqControl::incrementBandVFOA(void)
{
  if (VFO_A.Band == 10)VFO_A.Band = 0;
  else VFO_A.Band++;
  bandChangeVFOA();
  updateVFOA(&dummyFreqUpdateAccum);
}*/

/*
uint8_t FreqControl::changeVFOAmode(void)
{
  uint8_t temp = VFO_A.Mode;
  if (temp == 2) temp = 0;
  else temp++;
  VFO_A.Mode = temp;
  _main_LCD.updateVFOADisplay(&VFO_A.Freq, &VFO_A.Mode);
  return temp;
}*/

/*
void FreqControl::initVFOB(void)
{
  Set_Band_Limits((long*)&VFOB_Band_Limits, VFO_B.Band);
  _right_LCD.update(Band_Cmnd + VFO_B.Band);
  _right_LCD.update(Mode_Cmnd + VFO_B.Mode);
  updateVFOB(&dummyFreqUpdateAccum);

  _Xmt_Link.updateXmtBand();
}*/

/*
void FreqControl::bandChangeVFOB()
{
    Band_Settings_VFO_B[VFO_B_Current_Band].Freq = VFO_B.Freq;        // save current band frequency & mode
    Band_Settings_VFO_B[VFO_B_Current_Band].Mode = VFO_B.Mode;

    VFO_B.Freq = Band_Settings_VFO_B[VFO_B.Band].Freq;                // load new band frequency & mode
    VFO_B.Mode = Band_Settings_VFO_B[VFO_B.Band].Mode;

    VFO_B_Current_Band = VFO_B.Band;                                  // update current band

    Set_Band_Limits((long*)&VFOB_Band_Limits, VFO_B.Band);
    _right_LCD.update(Band_Cmnd + VFO_B.Band);
    _right_LCD.update(Mode_Cmnd + VFO_B.Mode);
}*/

/*
void FreqControl::updateVFOB(int16_t *_freqUpdate)
{
  VFO_B.Freq = VFO_B.Freq + *_freqUpdate;
  if (VFO_B.Freq < VFOB_Band_Limits[LOWER]) VFO_B.Freq = VFOB_Band_Limits[LOWER];
  if (VFO_B.Freq > VFOB_Band_Limits[UPPER]) VFO_B.Freq = VFOB_Band_Limits[UPPER];
  _si570.VFO_Update(&VFO_B.Freq, &VFO_B.Band);
  _main_LCD.updateVFOBDisplay(&VFO_B.Freq, &VFO_B.Mode);
}*/

/*
void FreqControl::decrementBandVFOB(void)
{
  if (VFO_B.Band == 0)VFO_B.Band = 10;
  else VFO_B.Band--;
  bandChangeVFOB();
  updateVFOB(&dummyFreqUpdateAccum);
}*/

/*
void FreqControl::incrementBandVFOB(void)
{
  if (VFO_B.Band == 10)VFO_B.Band = 0;
  else VFO_B.Band++;
  bandChangeVFOB();
  updateVFOB(&dummyFreqUpdateAccum);
}*/

/*
uint8_t FreqControl::changeVFOBmode(void)
{
  uint8_t temp = VFO_B.Mode;
  if (temp == 2) temp = 0;
  else temp++;
  VFO_B.Mode = temp;
  _main_LCD.updateVFOBDisplay(&VFO_B.Freq, &VFO_B.Mode);
  return temp;
}*/

/*
void FreqControl::VFOAtoVFOB(uint8_t *vfoFlag)
{
  Band_Settings_VFO_B[VFO_A.Band].Freq = VFO_A.Freq;                // save VFOA setting to corresponding VFOB band
  Band_Settings_VFO_B[VFO_A.Band].Mode = VFO_A.Mode;

  if (vfoFlag)                                                      // if VFOB is active...
  {
    if (VFO_A.Band != VFO_B.Band)                                   // if VFOA and VFO on different bands, change VFO B band
    {
    VFO_B.Band = VFO_A.Band;
    bandChangeVFOB();      
    }           
    
    else                                                            // if VFOA and VFOB already on same band...
    {
      VFO_B.Freq = VFO_A.Freq;                                      // copy frequency and mode data from VFOA to VFOB
      VFO_B.Mode = VFO_A.Mode;
    }

    initVFOB();                                                     // update VFO B
  }

  else                                                              // if VFOA is active...
  {
    Band_Settings_VFO_B[VFO_B.Band].Freq = VFO_B.Freq;              // save current VFOB settings
    Band_Settings_VFO_B[VFO_B.Band].Mode = VFO_B.Mode;
    VFO_B.Freq = VFO_A.Freq;                                        // load new VFOB settings from VFOA
    VFO_B.Mode = VFO_A.Mode;
    VFO_B.Band = VFO_A.Band;
    _main_LCD.updateVFOBDisplay(&VFO_B.Freq, &VFO_B.Mode);           // update the display
  }
  eepromUpdate();
}*/

/*
void FreqControl::memoryToVFOA(uint8_t *memNumber, uint8_t *vfoFlag)
{
  if (VFO_A.Band != Memory_Array[*memNumber].Band)
  {
    Band_Settings_VFO_A[Memory_Array[*memNumber].Band].Freq = Memory_Array[*memNumber].Freq;        // save new band frequency & mode
    Band_Settings_VFO_A[Memory_Array[*memNumber].Band].Mode = Memory_Array[*memNumber].Mode;
    VFO_A.Band = Memory_Array[*memNumber].Band;
  
    Band_Settings_VFO_A[VFO_A_Current_Band].Freq = VFO_A.Freq;        // save current band frequency & mode
    Band_Settings_VFO_A[VFO_A_Current_Band].Mode = VFO_A.Mode;

    VFO_A.Freq = Band_Settings_VFO_A[VFO_A.Band].Freq;                // load new band frequency & mode                
    VFO_A.Mode = Band_Settings_VFO_A[VFO_A.Band].Mode;

    VFO_A_Current_Band = VFO_A.Band;                                  // update current band

    Set_Band_Limits((long*)&VFOA_Band_Limits, VFO_A.Band);
  }
  else
  {
    VFO_A.Freq = Memory_Array[*memNumber].Freq;
    VFO_A.Mode = Memory_Array[*memNumber].Mode;
  }

  if (!*vfoFlag) 
    {
      _right_LCD.update(Band_Cmnd + VFO_A.Band);
      _right_LCD.update(Mode_Cmnd + VFO_A.Mode);
      updateVFOA(&dummyFreqUpdateAccum);
    }
  else _main_LCD.updateVFOADisplay(&VFO_A.Freq, &VFO_A.Mode);
  eepromUpdate();
}*/

/*
void FreqControl::VFOAtoMemory(uint8_t *memNumber)
{
  Memory_Array[*memNumber].Freq = VFO_A.Freq;
  Memory_Array[*memNumber].Band = VFO_A.Band;
  Memory_Array[*memNumber].Mode = VFO_A.Mode;
  _main_LCD.showMemoryDisplay(&Memory_Array[*memNumber].Freq, &Memory_Array[*memNumber].Mode, memNumber);
  eepromUpdate();
}*/

/*
void FreqControl::displayMemory(uint8_t *memNumber)
{
  _main_LCD.showMemoryDisplay(&Memory_Array[*memNumber].Freq, &Memory_Array[*memNumber].Mode, memNumber);
}*/


/********************BANDS EDGE LIMITS****************************************************************************************/

#define Bottom_160M   1800000
#define Top_160M      2000000

#define Bottom_80M    3500000
#define Top_80M       4000000

#define Bottom_40M    7000000
#define Top_40M       7300000

#define Bottom_30M    10100000
#define Top_30M       10150000

#define Bottom_20M    14000000
#define Top_20M       14350000

#define Bottom_17M    18068000
#define Top_17M       18168000

#define Bottom_15M    21000000
#define Top_15M       21450000

#define Bottom_12M    24890000
#define Top_12M       24990000

#define Bottom_10aM   28000000
#define Top_10aM      28500000

#define Bottom_10bM   28500000
#define Top_10bM      29000000

#define Bottom_10cM   29000000
#define Top_10cM      29700000


void FreqControl::Set_Band_Limits(long *Band_Limit_Array, char Band_Number)
{
  switch (Band_Number)
  {
    case 0:
      *(Band_Limit_Array) = Bottom_160M;
      *(Band_Limit_Array + 1) = Top_160M;
      break;

    case 1:
      *(Band_Limit_Array) = Bottom_80M;
      *(Band_Limit_Array + 1) = Top_80M;
      break;

    case 2:
      *(Band_Limit_Array) = Bottom_40M;
      *(Band_Limit_Array + 1) = Top_40M;
      break;

    case 3:
      *(Band_Limit_Array) = Bottom_30M;
      *(Band_Limit_Array + 1) = Top_30M;
      break;

    case 4:
      *(Band_Limit_Array) = Bottom_20M;
      *(Band_Limit_Array + 1) = Top_20M;
      break;

    case 5:
      *(Band_Limit_Array) = Bottom_17M;
      *(Band_Limit_Array + 1) = Top_17M;
      break;

    case 6:
      *(Band_Limit_Array) = Bottom_15M;
      *(Band_Limit_Array + 1) = Top_15M;
      break;

    case 7:
      *(Band_Limit_Array) = Bottom_12M;
      *(Band_Limit_Array + 1) = Top_12M;
      break;

    case 8:
      *(Band_Limit_Array) = Bottom_10aM;
      *(Band_Limit_Array + 1) = Top_10aM;
      break;

    case 9:
      *(Band_Limit_Array) = Bottom_10bM;
      *(Band_Limit_Array + 1) = Top_10bM;
      break;

    case 10:
      *(Band_Limit_Array) = Bottom_10cM;
      *(Band_Limit_Array + 1) = Top_10cM;
      break;
  }
}


uint8_t FreqControl::freqToBandNumber(int32_t freq)
{
  if(freq >= Bottom_160M && freq <= Top_160M) return (0);
  else if(freq >= Bottom_80M && freq <= Top_80M) return (1);
  else if(freq >= Bottom_40M && freq <= Top_40M) return (2);
  else if(freq >= Bottom_30M && freq <= Top_30M) return (3);
  else if(freq >= Bottom_20M && freq <= Top_20M) return (4);
  else if(freq >= Bottom_17M && freq <= Top_17M) return (5);
  else if(freq >= Bottom_15M && freq <= Top_15M) return (6);
  else if(freq >= Bottom_12M && freq <= Top_12M) return (7);
  else if(freq >= Bottom_10aM && freq <= Top_10aM) return (8);
  else if(freq >= Bottom_10bM && freq <= Top_10bM) return (9);
  else return (10);
}


void FreqControl::catVFO_A_FreqUpdate(int32_t freq)
{
  bandChangeVFOA(freqToBandNumber(freq));
  
  if(_VFO_A.Mode == 0) _VFO_A.Freq = freq + 1500;
  else if (_VFO_A.Mode == 1) _VFO_A.Freq = freq - 1500;
  else _VFO_A.Freq = freq;
  
  if (_VFO_A.Freq < _VFOA_Band_Limits[LOWER]) _VFO_A.Freq = _VFOA_Band_Limits[LOWER];
  if (_VFO_A.Freq > _VFOA_Band_Limits[UPPER]) _VFO_A.Freq = _VFOA_Band_Limits[UPPER];

  updateVFOA(&_dummyFreqUpdateAccum);
}

