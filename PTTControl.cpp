/******************************************************************************
*
* 2020 HF Transceiver by AD5GH
* (http://www.ad5gh.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* PTT TRANSMIT & RECEIVE CHANGE OVER ROUTINES
*
* Copyright 2021 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.1.0
* August 21, 2021
*
******************************************************************************/

#include <arduino.h>
#include <AmpLink.h>
#include <Mixer_Board_v2.h>
#include <ProdDetAudio_Board.h>
#include <PTTControl.h>
#include <RelayDriver.h>

AmpLink              _ampLink;
Mixer_Board_v2       _mixer_Board;
ProdDetAudio_Board   _prodDetAudioBoard;
RelayDriver          relayDriver;


PTTControl::PTTControl(void)
{

}


void PTTControl::begin(void)
{
  
}

void PTTControl::Xmt(void)   
{
   _prodDetAudioBoard.disableBFO();
   _mixer_Board.txMode();
   relayDriver.txMode();
   _prodDetAudioBoard.enableCarrOsc();
   _ampLink.ampToTxMode();
}

void PTTControl::Rcv(void)
{
   _ampLink.ampToRxMode();
   _prodDetAudioBoard.disableCarrOsc();
   _mixer_Board.rxMode();
   relayDriver.rxMode();
   _prodDetAudioBoard.enableBFO();
}