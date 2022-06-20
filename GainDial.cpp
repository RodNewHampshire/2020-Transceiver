/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* GAIN DIAL ROUTINES
*
* Copyright 2020 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* NOVEMBER 27, 2020
*
******************************************************************************/

#include <EEProm.h>
#include <GainDial.h>
#include <FuncLCD.h>
#include <IFBoard.h>
#include <RelayDriver.h>
#include <Wire.h>

FuncLCD     func_LCD1;
EEProm      EE_Prom;
IFBoard     _if_Board;
RelayDriver _relay_Driver;

#define I2C_DS1807      				0x50 >> 1       // I2C Addresses
#define DISABLE_ZEROCROSSING_DETECTION 	0xBE
#define AF_SPKR_GAIN         			0xAA            // DS1807 Potentiometer addresses
#define AF_LINE_GAIN     				0xA9

uint8_t     gainFunc;

enum 									// function menu items are displayed in this order
{
	START_GAINS,					    // START_GAINS & END_GAINS are used to auromatically define the start and end of the function list
	AF,
	RF,
	IF,
	LINE_OUT,
	END_GAINS
};


GainDial::GainDial(void)
{

}


void GainDial::begin(void)
{
  	gainFunc = 1;
  	disableZeroCrossingDetection();
  	setAFGain(EE_Prom.getAF_Gain());
  	setLineGain(EE_Prom.getLN_Gain());
    setRFAttn(EE_Prom.getRF_Attn());
    setIFGain(EE_Prom.getIF_Gain());
  	func_LCD1.update(GAIN_Cmnd + AF-1);
    func_LCD1.dsplyGain(EE_Prom.getAF_Gain());
    func_LCD1.update(ATTN_Cmnd + EE_Prom.getRF_Attn());
    func_LCD1.update(AMP_Cmnd + EE_Prom.getPreAmp());
}

void GainDial::select(void)
{
	gainFunc++;
	if(gainFunc == END_GAINS) gainFunc = START_GAINS + 1;
	func_LCD1.update(GAIN_Cmnd + gainFunc - 1);
	switch (gainFunc)
	{
		case AF:                      
    	func_LCD1.dsplyGain(EE_Prom.getAF_Gain());
    	break;

    	case RF:
    	func_LCD1.blankGain();
    	break;

    	case IF:
    	func_LCD1.dsplyGain((EE_Prom.getIF_Gain()-9)*5);
    	break;

    	case LINE_OUT:
    	func_LCD1.dsplyGain(EE_Prom.getLN_Gain());
    	break;
	}
}


void GainDial::CCW(void)
{
	switch (gainFunc)
  	{
    	case AF:                         
    	incrementAFGain();
    	break;

    	case RF:
    	incrementRFAttn();
    	break;

    	case IF:
    	incrementIFGain();
    	break;

    	case LINE_OUT:
    	incrementLineGain();
    	break;
   }
}


void GainDial::CW(void)
{
	switch (gainFunc)
  	{
    	case AF:                         
    	decrementAFGain();
    	break;

    	case RF:
    	decrementRFAttn();
    	break;

    	case IF:
    	decrementIFGain();
    	break;

    	case LINE_OUT:
    	decrementLineGain();
    	break;
	}
}


void GainDial::incrementAFGain(void)
{
	uint8_t AF_Gain = EE_Prom.getAF_Gain();
    if(AF_Gain == 64) AF_Gain = 64;
	else AF_Gain = AF_Gain + 4;
	setAFGain(AF_Gain);
	func_LCD1.dsplyGain(AF_Gain);
    EE_Prom.storeAF_Gain(AF_Gain);
}


void GainDial::decrementAFGain(void)
{
	uint8_t AF_Gain = EE_Prom.getAF_Gain();
    if(AF_Gain == 0) AF_Gain = 0;
	else AF_Gain = AF_Gain - 4;
	setAFGain(AF_Gain);
	func_LCD1.dsplyGain(AF_Gain);
    EE_Prom.storeAF_Gain(AF_Gain);
}


void GainDial::incrementRFAttn(void)
{
	uint8_t RF_Attn = EE_Prom.getRF_Attn();
    uint8_t Pre_Amp = EE_Prom.getPreAmp();
    if(RF_Attn == 0)
    {
        Pre_Amp = 1;
        func_LCD1.update(AMP_Cmnd + Pre_Amp);
        EE_Prom.storePreAmp(Pre_Amp);
        return;
    }
    else RF_Attn = RF_Attn - 1;
    setRFAttn(RF_Attn);
    func_LCD1.update(ATTN_Cmnd + RF_Attn);
    EE_Prom.storeRF_Attn(RF_Attn);
}


void GainDial::decrementRFAttn(void)
{
	uint8_t RF_Attn = EE_Prom.getRF_Attn();
    uint8_t Pre_Amp = EE_Prom.getPreAmp();
    if(Pre_Amp == 1)
    {
        Pre_Amp = 0;
        func_LCD1.update(AMP_Cmnd + Pre_Amp);
        EE_Prom.storePreAmp(Pre_Amp);
        return;
    }
    if(RF_Attn == 15) RF_Attn = 15;
    else RF_Attn = RF_Attn + 1;
    setRFAttn(RF_Attn);
    func_LCD1.update(ATTN_Cmnd + RF_Attn);
    EE_Prom.storeRF_Attn(RF_Attn);
}


void GainDial::incrementIFGain(void)
{
	uint8_t IF_Gain = EE_Prom.getIF_Gain();
    if(IF_Gain == 18) IF_Gain = 18;
    else IF_Gain = IF_Gain+1;
    setIFGain(IF_Gain);                           
    func_LCD1.dsplyGain((IF_Gain - 9)*5);           // Normalize to a value between -45 and +45dB
    EE_Prom.storeIF_Gain(IF_Gain);
}


void GainDial::decrementIFGain(void)
{
	uint8_t IF_Gain = EE_Prom.getIF_Gain();
    if(IF_Gain == 0) IF_Gain = 0;
    else IF_Gain = IF_Gain-1;
    setIFGain(IF_Gain);  
    func_LCD1.dsplyGain((IF_Gain - 9)*5);           // Normalize to a value between -45 and +45dB
    EE_Prom.storeIF_Gain(IF_Gain);
}


void GainDial::incrementLineGain(void)
{
	uint8_t LN_Gain = EE_Prom.getLN_Gain();
    if(LN_Gain == 64) LN_Gain = 64;
	else LN_Gain = LN_Gain + 4;
	setLineGain(LN_Gain);
	func_LCD1.dsplyGain(LN_Gain);
    EE_Prom.storeLN_Gain(LN_Gain);
}


void GainDial::decrementLineGain(void)
{
	uint8_t LN_Gain = EE_Prom.getLN_Gain();
    if(LN_Gain == 0) LN_Gain = 0;
	else LN_Gain = LN_Gain - 4;
	setLineGain(LN_Gain);
	func_LCD1.dsplyGain(LN_Gain);
    EE_Prom.storeLN_Gain(LN_Gain);
}


void GainDial::setAFGain(uint8_t level)
{
	setI2CForDS1807();
    Wire.beginTransmission(I2C_DS1807);
    Wire.write(AF_SPKR_GAIN);
    Wire.write(0x40-level);
    Wire.endTransmission();
}


void GainDial::setLineGain(uint8_t level)
{
	setI2CForDS1807();
    Wire.beginTransmission(I2C_DS1807);
    Wire.write(AF_LINE_GAIN);
    Wire.write(0x40-level);
    Wire.endTransmission();
}


void GainDial::setRFAttn(uint8_t level)
{
    _relay_Driver.RFATTN(level);
}


void GainDial::setIFGain(uint8_t level)
{
    _if_Board.setManIFGain(level);
}


uint8_t GainDial::getManIFGain(void)
{
    return(EE_Prom.getIF_Gain());
}


void GainDial::disableZeroCrossingDetection(void)
{
    setI2CForDS1807();
    Wire.beginTransmission(I2C_DS1807);
    Wire.write(DISABLE_ZEROCROSSING_DETECTION);
    Wire.endTransmission();
}


void GainDial::setI2CForDS1807(void)
{
  Wire.beginTransmission(0x70);
  Wire.write(0x01);
  Wire.endTransmission();
}