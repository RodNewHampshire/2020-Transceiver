/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* IN219 ROUTINES
*
* Copyright 20202 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.0
* OCTOBER 2, 2021
*
******************************************************************************/

#include <arduino.h>
#include <Adafruit_INA219.h>
#include <FuncLCD.h>
#include <PA_INA219.h>
#include <Wire.h>

Adafruit_INA219 _ina219;
FuncLCD         setI2CBus;


PA_INA219::PA_INA219(void)
{

}


void PA_INA219::begin(void)
{
    setI2CBus.setI2CForLCD();
    _ina219.setCalibration_32V_1A();
    _ina219.begin();
}


void PA_INA219::getPAVoltAmps(float *shuntVoltage, float *busVoltage, float *loadAmpTicks)
{ 
 	setI2CBus.setI2CForLCD();
    *shuntVoltage = _ina219.getShuntVoltage_mV();
    *busVoltage	= _ina219.getBusVoltage_V();
    *loadAmpTicks = _ina219.getCurrent_mA();
}