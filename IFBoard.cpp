/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA IF BOARD SOFTWAREbROUTINES
*
* Copyright 2021 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* SEPTEMBER 18, 2021
*
******************************************************************************/

#include <GainDial.h>
#include <IFBoard.h>
#include <MeterLCD.h>

#define I2C_IF_Board 0x10 >> 1

#define SLOW  0                                 // AGC settings
#define FAST  1
#define MAN   2

/******************************************************************************/

GainDial _Gain_Dial;
MeterLCD S_Meter;

uint8_t agcMode;

extern uint8_t S_METER_CAL[15];


IFBoard::IFBoard(void)
{

}


void IFBoard::begin(void)
{
    
}


void IFBoard::setAGCMode(uint8_t _agcMode)
{
    uint8_t temp;
    temp = _Gain_Dial.getManIFGain();
    setI2CForIFBrd();
    Wire.beginTransmission(I2C_IF_Board);
    Wire.write(_agcMode + (temp << 3));
    Wire.endTransmission();
    if(_agcMode == MAN)
    {
                                                // Code here to update MeterLCD for manual IF gain display
    }
}


void IFBoard::setManIFGain(uint8_t level)
{
    if(agcMode == MAN)
    {
        setI2CForIFBrd();
        Wire.beginTransmission(I2C_IF_Board);
        Wire.write(MAN + (level << 3));
        Wire.endTransmission();
    }
}


uint8_t IFBoard::getSMeterCalValue(void)
{
    uint8_t Cal_value = 0xFF;

    setI2CForIFBrd();
    Wire.requestFrom(I2C_IF_Board, (uint8_t) 1);
    if(Wire.available()) Cal_value = Wire.read();
    Wire.endTransmission();
    return(Cal_value);
}


void IFBoard::updateSMeter(void)
{
    uint8_t Sig_Strength = 0xFF;

    //if(_AGC_Mode == MAN) return;

    setI2CForIFBrd();
    Wire.requestFrom(I2C_IF_Board, (uint8_t) 1);
    if(Wire.available()) Sig_Strength = Wire.read();
    Wire.endTransmission();

    if(Sig_Strength > S_METER_CAL[0]) S_Meter.updateSMeterDsply(0);                                             //S0
    else if(Sig_Strength <= S_METER_CAL[0] && Sig_Strength > S_METER_CAL[1]) S_Meter.updateSMeterDsply(1);      //S1
    else if(Sig_Strength <= S_METER_CAL[1] && Sig_Strength > S_METER_CAL[2]) S_Meter.updateSMeterDsply(2);      //S2
    else if(Sig_Strength <= S_METER_CAL[2] && Sig_Strength > S_METER_CAL[3]) S_Meter.updateSMeterDsply(3);      //S3
    else if(Sig_Strength <= S_METER_CAL[3] && Sig_Strength > S_METER_CAL[4]) S_Meter.updateSMeterDsply(4);      //S4
    else if(Sig_Strength <= S_METER_CAL[4] && Sig_Strength > S_METER_CAL[5]) S_Meter.updateSMeterDsply(5);      //S5
    else if(Sig_Strength <= S_METER_CAL[5] && Sig_Strength > S_METER_CAL[6]) S_Meter.updateSMeterDsply(6);      //S6
    else if(Sig_Strength <= S_METER_CAL[6] && Sig_Strength > S_METER_CAL[7]) S_Meter.updateSMeterDsply(7);      //S7
    else if(Sig_Strength <= S_METER_CAL[7] && Sig_Strength > S_METER_CAL[8]) S_Meter.updateSMeterDsply(8);      //S8
    else if(Sig_Strength <= S_METER_CAL[8] && Sig_Strength > S_METER_CAL[9]) S_Meter.updateSMeterDsply(9);      //S9
    else if(Sig_Strength <= S_METER_CAL[9] && Sig_Strength > S_METER_CAL[10]) S_Meter.updateSMeterDsply(10);    //+10
    else if(Sig_Strength <= S_METER_CAL[10] && Sig_Strength > S_METER_CAL[11]) S_Meter.updateSMeterDsply(11);   //+20
    else if(Sig_Strength <= S_METER_CAL[11] && Sig_Strength > S_METER_CAL[12]) S_Meter.updateSMeterDsply(12);   //+30
    else if(Sig_Strength <= S_METER_CAL[12] && Sig_Strength > S_METER_CAL[13]) S_Meter.updateSMeterDsply(13);   //+40
    else if(Sig_Strength <= S_METER_CAL[13] && Sig_Strength > S_METER_CAL[14]) S_Meter.updateSMeterDsply(14);   //+50
    else if(Sig_Strength <= S_METER_CAL[14]) S_Meter.updateSMeterDsply(15);                                     //+60
}


void IFBoard::setI2CForIFBrd(void)
{
  Wire.beginTransmission(0x70);
  Wire.write(0x02);
  Wire.endTransmission();
}
