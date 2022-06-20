/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* FUNCTION DIAL ROUTINES
*
* Copyright 2020 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* NOVEMBER 27, 2020
*
******************************************************************************/

//Band Indexing 0=160m, 1=80m, 2=40m, 3=30m, 4=20m, 5=17m, 6=15m, 7=12m, 8=10am, 9=10bm, 10=10cm

#include <EEProm.h>
#include <FreqControl.h>
#include <FuncDial.h>
#include <FuncLCD.h>
#include <MeterLCD.h>
#include <Mixer_Board_v2.h>
#include <ProdDetAudio_Board.h>
#include <RelayDriver.h>

EEProm              _EEProm;
FreqControl         freq_Control;
FuncLCD 	       func_LCD;
MeterLCD            meter_LCD;
Mixer_Board_v2      mixer_Board;
ProdDetAudio_Board  prod_DetAudioBoard;
RelayDriver	        relay_Driver;

uint8_t functionNum;

extern uint8_t  agcMode;
extern uint8_t  rate;
extern uint16_t _TuningRate;

uint8_t TR = 0;

enum 									// function menu items are displayed in this order
{
	START_FUNCTIONS,					// START_FUNCTIONS & END_FUNCTIONS are used to automatically define the start and end of the function list
	BAND_SELECT,
	RATE_SELECT,
    AGC_SELECT,
	MODE_SELECT,
    SCAL_SELECT,
    GPS_SELECT,
	END_FUNCTIONS
};


FuncDial::FuncDial(void)
{

}

void FuncDial::begin(void)
{
  	functionNum = 1;
    agcMode = _EEProm.getAGC();
    rate = _EEProm.getRate();

  	func_LCD.init(agcMode, rate, _EEProm.getPreAmp(), _EEProm.getVFOA_Mode(), _EEProm.getVFOA_Band(), TR);
}

void FuncDial::select(void)
{
	if(functionNum == SCAL_SELECT) meter_LCD.stopSMeterCalibration();  // Necessary to exit S-Meter calibration
    
    functionNum++;
	if(functionNum == END_FUNCTIONS)
    {
        functionNum = START_FUNCTIONS + 1;
        meter_LCD.clear();
    }
	func_LCD.update(FUNC_Cmnd + functionNum - 1);
    meter_LCD.functionDisplay(functionNum);
}

void FuncDial::CW(void)
{
	if (functionNum == 1)                          //Increment Band
  	{
        uint8_t Band = _EEProm.getVFOA_Band();
        if(Band == 10) Band = 0;
        else Band++;
        freq_Control.bandChangeVFOA(Band);
    }

    else if (functionNum == 2)                     //Increment AGC     
    {
        if(agcMode == 2) agcMode = 0;
        else agcMode++;
        func_LCD.update(AGC_Cmnd + agcMode);
    }         

    else if (functionNum == 3)                     //Increment Mode
    {
        uint8_t mode = _EEProm.getVFOA_Mode();
        if(mode == 2) mode = 0;
        else mode++;
        prod_DetAudioBoard.setBFO_CarrierOscMode(mode);
        func_LCD.update(MODE_Cmnd + mode);
        _EEProm.storeVFOA_Mode(mode);
    }

    else if (functionNum == 4)                     //Increment Rate
    {
        if(rate == 3) rate = 0;
        else rate++;
        func_LCD.update(RATE_Cmnd + rate);
    }

    else if (functionNum == 5)                     //S-Meter Calibration
    {
        
    }

    else if (functionNum == 6)                     //GPS Frequency Error Display On
    {
        
    }
}

void FuncDial::CCW(void)
{
	if(functionNum == 1)                           //Decrement Band
    {
        uint8_t Band = _EEProm.getVFOA_Band();
        if(Band == 0) Band = 10;
        else Band--;
        freq_Control.bandChangeVFOA(Band);
    }

    else if(functionNum == 2)                      //Decrement AGC
    {
        if(agcMode == 0) agcMode = 2;
        else agcMode--;
        func_LCD.update(AGC_Cmnd + agcMode);
    }

    else if(functionNum == 3)                      //Decrement Mode
    {
        uint8_t mode = _EEProm.getVFOA_Mode();
        if(mode == 0) mode = 2;
        else mode--;
        prod_DetAudioBoard.setBFO_CarrierOscMode(mode); 
        func_LCD.update(MODE_Cmnd + mode);
        _EEProm.storeVFOA_Mode(mode);
    }

    else if(functionNum == 4)                      //Decrement Rate
    {
        if(rate == 0) rate = 3;
        else rate--;
        func_LCD.update(RATE_Cmnd + rate);
    }

    else if (functionNum == 5)                     //S-Meter Calibration
    {
        
    }

    else if (functionNum == 6)                     //GPS Frequency Error Display Off
    {
        
    }
}