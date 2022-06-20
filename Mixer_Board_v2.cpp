/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* 1st and 2nd Mixer Board Version 2 Routines
*
* Copyright 2021 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* JUNE 27, 2021
*
******************************************************************************/

#include <arduino.h>
#include <Mixer_Board_v2.h>
#include <si5351.h>
//#include <Wire.h>

#define METRO_MINI    0x62

Si5351      SI5351;

// Roofing Filter: 69.450 MHz

// 1st Mixer LO = CLK1 on Si5351
// 2nd Mixer LO = CLK0 on Si5351

// Rx Mode:     1st Mixer LO = VFO
//              2nd Mixer LO = LO

// Tx Mode:     1st Mixer LO = LO
//              2nd Mixer LO = VFO

uint8_t CurrentBand = 0xFF;

unsigned long long LO  = 7845000000ULL;                         // Local Oscillator frequency
unsigned long long VFO;                                         // Variable Frequency Oscillator Frequency
unsigned long long REF  = 250000000ULL;                         // GPS Stabilization reference frequency
unsigned long long IF  = 6945000000ULL;

typedef union
{
  int integer;                                                  // this union allows integers to be sent as binary representations over the I2C
  byte binary[sizeof(integer)];
} binaryInt;

binaryInt freqError;

const unsigned long _NomXtalFreq = 25000000;
unsigned long _XtalFreq;


Mixer_Board_v2::Mixer_Board_v2(void)
{

}


void Mixer_Board_v2::begin(void)
{
    setI2CForMxrBrd();

    SI5351.init(SI5351_CRYSTAL_LOAD_10PF, 0, 0);

    SI5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);       // Set to 2mA to ensure AD831 LO level < 1Vp-p
    SI5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);       // Set to 2mA to ensure AD831 LO level < 1Vp-p
    SI5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_4MA);       // Set to 4mA for driving 3.3V logic on Mini Metro

    SI5351.output_enable(SI5351_CLK0, 0);
    SI5351.output_enable(SI5351_CLK1, 0);
    SI5351.output_enable(SI5351_CLK2, 0);

    setSecondLO(LO);                               
    SI5351.set_freq(REF, SI5351_CLK2);                          // Set CLK2 to reference frequency for GPS stabilization

    SI5351.output_enable(SI5351_CLK0, 1);
    SI5351.output_enable(SI5351_CLK1, 1);
    SI5351.output_enable(SI5351_CLK2, 1);
}


void Mixer_Board_v2::vfoUpdate(uint32_t *freq)
{
    VFO = IF + (unsigned long long)(*freq) * 100;
    setFirstLO(VFO);
}


void Mixer_Board_v2::setFirstLO(uint64_t Freq)
{                                                      
    setI2CForMxrBrd();
    SI5351.set_freq(Freq, SI5351_CLK1);
}


void Mixer_Board_v2::setSecondLO(uint64_t Freq)        
{                                                     
    setI2CForMxrBrd();
    SI5351.set_freq(Freq, SI5351_CLK0);
}


void Mixer_Board_v2::txMode(void)                               // Switch VFO and LO for Xmt
{                                                               // Frequency updates are disabled during Xmt elsewhare in code
    setFirstLO(LO);
    setSecondLO(VFO);
}


void Mixer_Board_v2::rxMode(void)                               // Switch VFO and LO back to normal for Rcv
{
    setFirstLO(VFO);
    setSecondLO(LO);
}


void Mixer_Board_v2::setI2CForMxrBrd(void)
{
  Wire.beginTransmission(0x70);
  Wire.write(0x02);
  Wire.endTransmission();
}


void Mixer_Board_v2::readGPSFreqCorrection(void)
{
    uint8_t n = 0;

    setI2CForMxrBrd();

    Wire.requestFrom(METRO_MINI, sizeof(freqError.binary));

    while(Wire.available() !=0)
    {
        freqError.binary[n] = Wire.read();
        n++;
    }

    _XtalFreq = _NomXtalFreq + freqError.integer;
}


void Mixer_Board_v2::sendI2C_1_Byte(uint8_t addr, uint8_t data)
{
    Wire.beginTransmission(addr);
    Wire.write(byte (data));
    Wire.endTransmission();
}

