/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* FUNCTION (RIGHT) LCD ROUTINES
*
* Copyright 20202 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* NOVEMBER 26, 2020
*
******************************************************************************/

#include "Adafruit_LiquidCrystal.h"
#include <arduino.h>
#include <FreqControl.h>
#include <FuncLCD.h>
#include <IFBoard.h>
#include <MeterLCD.h>
#include <RelayDriver.h>

Adafruit_LiquidCrystal  _funcLCD(1);
FreqControl             _FreqControl;
IFBoard                 _IFBoard;
MeterLCD                _meterLcd;
RelayDriver             _relayDriver;


char Function_Row0[21] = {"ATT AMP MOD AGC RATE"};
char Function_Row1[21] = {"                    "};
char Function_Row2[21] = {"        BAND    FUNC"};
char Function_Row3[21] = {"                    "};

const char BAND_1[11] = {"1       111"};
const char BAND_2[11] = {"68432111000"};
const char BAND_3[11] = {"00000752abc"};

const char MODE_1[3] = {"UL "};
const char MODE_2[3] = {"SSC"};
const char MODE_3[3] = {"BBW"};

const char AGC_1[3] = {"SFM"};
const char AGC_2[3] = {"LSA"};
const char AGC_3[3] = {"WTN"};

const char RATE_1[4] = {"1   "};
const char RATE_2[4] = {"021 "};
const char RATE_3[4] = {"0501"};
const char RATE_4[4] = {"0000"};

const char ATTN_1[16] = {"0000011111222223"};
const char ATTN_2[16] = {"0246802468024680"};

const char AMP_1[2] = {"O "};
const char AMP_2[2] = {"FO"};
const char AMP_3[2] = {"FN"};

const char TR_1[2] = {" *"};
const char TR_2[2] = {"RX"};
const char TR_3[2] = {"CM"};
const char TR_4[2] = {"VT"};
const char TR_5[2] = {" *"};

const char FUNC_1[9] = {"BAMRSGMMMS"};
const char FUNC_2[9] = {"aGoacP122i"};
const char FUNC_3[9] = {"nCdtaSCBC5"};
const char FUNC_4[9] = {"d eel KLK7"};
const char FUNC_5[9] = {"        0"};

const char GAIN_1[5] = {"ARIL "};
const char GAIN_2[5] = {"FFFN "};

char BAND_Disp[3];
char MODE_Disp[3];
char AGC_Disp[3];
char RATE_Disp[4];
char ATTN_Disp[2];
char AMP_Disp[3];
char GAIN_DISP[3];
char TR_Disp[2];


FuncLCD::FuncLCD(void)
{

}


void FuncLCD::begin(void)
{
  setI2CForLCD();
  _funcLCD.begin(20, 4);
}


void FuncLCD::clear(void)
{
	setI2CForLCD();
  _funcLCD.clear();
}


void FuncLCD::splash(void)
{
  setI2CForLCD();
  _funcLCD.setCursor(0, 0);
 	_funcLCD.print(F("    FUNCTION LCD    "));
  _funcLCD.setCursor(0, 1);
  _funcLCD.print(F("  HIGH PERFORMANCE  "));
  _funcLCD.setCursor(0, 2);
  _funcLCD.print(F("   HF TRANSCEIVER   "));
  _funcLCD.setCursor(0, 3);
  _funcLCD.print(F("      BY AD5GH      "));
}


void FuncLCD::init(uint8_t _AGC, uint8_t _RATE, uint8_t _AMP, uint8_t _MODE, uint8_t _BAND, uint8_t _TR)
{
  setI2CForLCD();
  _funcLCD.setCursor(0, 0);
  _funcLCD.print(Function_Row0);

  _funcLCD.setCursor(0, 1);
  _funcLCD.print(Function_Row1);

  _funcLCD.setCursor(0, 2);
  _funcLCD.print(Function_Row2);

  _funcLCD.setCursor(0, 3);
  _funcLCD.print(Function_Row3);

  update(AGC_Cmnd + _AGC);
  update(RATE_Cmnd + _RATE);
  update(AMP_Cmnd + _AMP);
  update(MODE_Cmnd + _MODE);
  update(BAND_Cmnd + _BAND);
  update(FUNC_Cmnd + 0);
  //update(TR_Cmnd + _TR);
}


void FuncLCD::update(uint8_t command)
{
	setI2CForLCD();
  switch (command >> 4)
  {
    case 1:                                   //NOT USED
      
      break;

    case 2:                                   //NOT USED

      break;

    case 3:                                   //RF ATTENUATOR
      command = command & 0x0F;
      Function_Row1[0] = ATTN_1[command];
      Function_Row1[1] = ATTN_2[command];
      _funcLCD.setCursor(0, 1);
      _funcLCD.print(Function_Row1);
      break;

    case 4:                                   //RF PRE-AMP
      command = command & 0x0F;
      Function_Row1[4] = AMP_1[command];
      Function_Row1[5] = AMP_2[command];
      Function_Row1[6] = AMP_3[command];
      _funcLCD.setCursor(0, 1);
      _funcLCD.print(Function_Row1);
      break;

    case 5:                                   //BAND
      command = command & 0x0F;
      Function_Row3[8] = BAND_1[command];
      Function_Row3[9] = BAND_2[command];
      Function_Row3[10] = BAND_3[command];
      _funcLCD.setCursor(0, 3);
      _funcLCD.print(Function_Row3);
      _relayDriver.BPF(command);
      break;

    case 6:                                   //MODE
      command = command & 0x0F;
      Function_Row1[8] = MODE_1[command];
      Function_Row1[9] = MODE_2[command];
      Function_Row1[10] = MODE_3[command];
      _funcLCD.setCursor(0, 1);
      _funcLCD.print(Function_Row1);
      break;

    case 7:                                   //AGC
      command = command & 0x0F;
      Function_Row1[12] = AGC_1[command];
      Function_Row1[13] = AGC_2[command];
      Function_Row1[14] = AGC_3[command];
      _funcLCD.setCursor(0, 1);
      _funcLCD.print(Function_Row1);
      _IFBoard.setAGCMode(command);
      break;

    case 8:                                   //NOT USED
      
      break;

    case 9:                                   //NOT USED
      
      break;

    case 10:                                  //RATE
      command = command & 0x0F;
      Function_Row1[16] = RATE_1[command];
      Function_Row1[17] = RATE_2[command];
      Function_Row1[18] = RATE_3[command];
      Function_Row1[19] = RATE_4[command];
      _funcLCD.setCursor(0, 1);
      _funcLCD.print(Function_Row1);
      _FreqControl.setTuningRate(command);
      break;

    case 11:                                  // TR
      command = command & 0x0F;
      break;

      case 12:                                // FUNCTION
      command = command & 0x0F;
      Function_Row3[16] = FUNC_1[command];
      Function_Row3[17] = FUNC_2[command];
      Function_Row3[18] = FUNC_3[command];
      Function_Row3[19] = FUNC_4[command];
      //Function_Row3[19] = FUNC_5[command];
      _funcLCD.setCursor(0, 3);
      _funcLCD.print(Function_Row3);
      break;

      case 13:                                // GAIN
      command = command & 0x0F;
      Function_Row2[0] = GAIN_1[command];
      Function_Row2[1] = GAIN_2[command];
      _funcLCD.setCursor(0, 2);
      _funcLCD.print(Function_Row2);
      break;

      case 14:                                // S-METER CALIBRATION
      break;

      case 15:                                // GPS ERROR DISPLAY
      break;
  }
}


void FuncLCD::dsplyGain(int8_t gain)
{
  setI2CForLCD();
  if(gain < 0) 
    {
      GAIN_DISP[0] = 0x2D;
      gain = -gain;
    }
  else if(gain == 0) GAIN_DISP[0] = 0x20;
  else GAIN_DISP[0]= 0x2B;

  if (gain < 10) 
    {
        GAIN_DISP[1] = 0x20;
        GAIN_DISP[2] = gain + 0x30;
    }
    else
    {
        GAIN_DISP[1] = (gain/10 + 0x30);
        GAIN_DISP[2] = ((gain % 10) + 0x30);
    }
    
    Function_Row3[0] = GAIN_DISP[0];
    Function_Row3[1] = GAIN_DISP[1];
    Function_Row3[2] = GAIN_DISP[2];
    _funcLCD.setCursor(0, 3);
    _funcLCD.print(Function_Row3);
}

void FuncLCD::blankGain(void)
{
  setI2CForLCD();
  Function_Row3[0] = 0x20;
  Function_Row3[1] = 0x20;
  Function_Row3[2] = 0x20;
  _funcLCD.setCursor(0, 3);
  _funcLCD.print(Function_Row3);
}

void FuncLCD::setI2CForLCD(void)
{
  Wire.beginTransmission(0x70);
  Wire.write(0x01);
  Wire.endTransmission();
}