/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* Product Detector & Audio Board Routines
*
* Copyright 2021 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* AUGUST 21, 2021
*
******************************************************************************/

#include <arduino.h>
#include <EEProm.h>
#include <si5351.h>
#include <ProdDetAudio_Board.h>
#include <Wire.h>

#define USB 0
#define LSB 1
#define CW  2


Si5351      Si_5351;

// CLK0 = BFO
// CLK1 = Carrier Osc

unsigned long long BFO_USB = 3599400000ULL;                         // Receive BFO USB
unsigned long long BFO_LSB = 3600600000ULL;                         // Receive BFO LSB
unsigned long long BFO_CW  = 3600000000ULL;                         // Receive BFO CW
unsigned long long CarrierOsc_USB = 899850000ULL;                   // Transmit Carrier Osc. USB
unsigned long long CarrierOsc_LSB = 900150000ULL;                   // Transmit Carrier Osc. LSB
unsigned long long CarrierOsc_CW  = 900000000ULL;                   // Transmit Carrier Osc. CW


ProdDetAudio_Board::ProdDetAudio_Board(void)
{

}


void ProdDetAudio_Board::begin(void)
{
    setI2CForPDABrd();

    Si_5351.init(SI5351_CRYSTAL_LOAD_10PF, 0, 0);

    Si_5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA);
    Si_5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_4MA);

    Si_5351.output_enable(SI5351_CLK0, 1);                          // Activate receive BFO
    Si_5351.output_enable(SI5351_CLK1, 0);
    Si_5351.output_enable(SI5351_CLK2, 0);
}


void ProdDetAudio_Board::enableBFO(void)
{
    setI2CForPDABrd();
    Si_5351.output_enable(SI5351_CLK0, 1);
}


void ProdDetAudio_Board::disableBFO(void)
{
    setI2CForPDABrd();
    Si_5351.output_enable(SI5351_CLK0, 0);
}


void ProdDetAudio_Board::enableCarrOsc(void)
{
    setI2CForPDABrd();
    Si_5351.output_enable(SI5351_CLK1, 1);
}


void ProdDetAudio_Board::disableCarrOsc(void)
{
    setI2CForPDABrd();
    Si_5351.output_enable(SI5351_CLK1, 0);
}


void ProdDetAudio_Board::setBFO_CarrierOscMode(uint8_t mode)
{
    setI2CForPDABrd();
    if(mode == USB) 
    {
        Si_5351.set_freq(BFO_USB, SI5351_CLK0);
        Si_5351.set_freq(CarrierOsc_USB, SI5351_CLK1);
    }
    else if (mode == LSB) 
    {
        Si_5351.set_freq(BFO_LSB, SI5351_CLK0);
        Si_5351.set_freq(CarrierOsc_LSB, SI5351_CLK1);
    }

    else if (mode == CW)
    {
        Si_5351.set_freq(BFO_CW, SI5351_CLK0);
        Si_5351.set_freq(CarrierOsc_CW, SI5351_CLK1);
    }
}


void ProdDetAudio_Board::sendI2C_1_Byte(uint8_t addr, uint8_t data)
{
    setI2CForPDABrd();
    Wire.beginTransmission(addr);
    Wire.write(byte (data));
    Wire.endTransmission();
}


void ProdDetAudio_Board::setI2CForPDABrd(void)
{
  Wire.beginTransmission(0x70);
  Wire.write(0x01);
  Wire.endTransmission();
}

