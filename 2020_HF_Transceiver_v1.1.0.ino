
/****************************************************************************************************************************

  2020 HF Transceiver
  High Performance HF Band Amateur Radio Transceiver

  by Rod Gatehouse

  Distributed under the terms of the MIT License:
  http://www.opensource.org/licenses/mit-license

  VERSION 1.1.0
  November 26, 2020

*****************************************************************************************************************************/

#include <ft857d.h>
#include <AmpLink.h>
#include <EEProm.h>
#include <FreqControl.h>
#include <FuncDial.h>
#include <FuncLCD.h>
#include <GainDial.h>
#include <IFBoard.h>
#include <Mixer_Board_v2.h>
#include <MeterLCD.h>
#include <ProdDetAudio_Board.h>
#include <PTTControl.h>
#include <Rotary.h>

ft857d              cat_Intfc = ft857d();
AmpLink             ampLink;
EEProm              eeprom;
FreqControl         freqControl;
FuncLCD             funcLCD;
FuncDial            funcDial;
GainDial            gainDial;
IFBoard             ifBoard;
Mixer_Board_v2      mixerBoard;
MeterLCD            meterLCD;
PTTControl          pttControl;
ProdDetAudio_Board  prodDetAudioBoard;


uint16_t currentVersion  = 10102;                       // current software version X.YY.ZZ, i.e., 1.00.00


/********************Frequency, Band, & Mode Variables************************************************************************/

//Index to band arrays:
//0=160M, 1=80M, 2=40M, 3=30M, 4=20M, 5=17M, 6=15M, 7=12M, 8=10Ma, 9=10Mb, 10=10Mc

int16_t freqUpdate = 0;
int16_t lastFreqUpdate = 0;
uint8_t VFO_Flag;                                       // 0 = VFO A, 1 = VFO B

extern uint16_t tuningRate;

uint8_t tuneOld = 0;
uint8_t rfDial = 0x30;


/******************** for Memory Display *************************************************************************************/

#define MEM_HIDE_PERIOD 100000                          // time of memory display after last memory button press

unsigned long memoryDisplayHide = MEM_HIDE_PERIOD;
uint8_t MEM_NUM = 0;
uint8_t MEM_Disp_Flag = 0;


/******************** for loop ***********************************************************************************************/

#define  HEART_BEAT 4000                                // heart beat period
uint16_t heartBeat = 0;                                 // heart beat counter


/******************** I/O Port Definitions ***********************************************************************************/

#define TUNE_ENCDR_A    2
#define TUNE_ENCDR_B    3
#define TUNE_ENCDR_SEL  4

#define GAIN_ENCDR_A    5
#define GAIN_ENCDR_B    6
#define GAIN_ENCDR_SEL  7

#define FUNC_ENCDR_A    8
#define FUNC_ENCDR_B    9
#define FUNC_ENCDR_SEL  10

#define ON_AIR_LED      11

#define PA_ENB          22
#define PA_PWR_ON       23
#define PWR_SENS        24
#define PWR_HOLD        25
#define STATUS_LED      26
#define PTT             27

Rotary gainRotary = Rotary(GAIN_ENCDR_A, GAIN_ENCDR_B);
Rotary funcRotary = Rotary(FUNC_ENCDR_A, FUNC_ENCDR_B);

boolean pttStatus = RCV;
boolean lastPTTStatus = XMT;


/******************** for Serial Ports ***************************************************************************************/


/******************** setup() ************************************************************************************************/

void setup()
{
  Serial.begin(9600);
  Serial.println("Serial port ready...");

  delay(500);

  pinMode(TUNE_ENCDR_A, INPUT);
  pinMode(TUNE_ENCDR_B, INPUT);

  if (digitalRead(TUNE_ENCDR_A)) bitSet(tuneOld, 4);
  if (digitalRead(TUNE_ENCDR_B)) bitSet(tuneOld, 5);

  attachInterrupt(digitalPinToInterrupt(TUNE_ENCDR_A), tuneEncdrISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TUNE_ENCDR_B), tuneEncdrISR, CHANGE);

  pinMode(GAIN_ENCDR_A, INPUT);
  pinMode(GAIN_ENCDR_B, INPUT);
  pinMode(GAIN_ENCDR_SEL, INPUT);

  pinMode(FUNC_ENCDR_A, INPUT);
  pinMode(FUNC_ENCDR_B, INPUT);
  pinMode(FUNC_ENCDR_SEL, INPUT);

  pinMode(ON_AIR_LED, OUTPUT);
  digitalWrite(ON_AIR_LED, HIGH);

  pinMode(PA_ENB, OUTPUT);
  digitalWrite(PA_ENB, LOW);

  pinMode(PA_PWR_ON, OUTPUT);
  digitalWrite(PA_PWR_ON, HIGH);

  pinMode(PWR_SENS, INPUT);

  pinMode(PWR_HOLD, OUTPUT);
  digitalWrite(PWR_HOLD, HIGH);

  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);

  pinMode(PTT, INPUT);

  if (digitalRead(TUNE_ENCDR_A)) bitSet(tuneOld, 4);
  if (digitalRead(TUNE_ENCDR_B)) bitSet(tuneOld, 5);

  meterLCD.begin();                                       //Necessary to call meterLCD twice to intialize both displays successfully. Issue is with TCA9548A I2C Bus Mux at power on.
  funcLCD.begin();
  meterLCD.begin();

  funcLCD.splash();
  meterLCD.splash();
  delay(1000);

  funcLCD.clear();
  meterLCD.clear();

  eeprom.readSettings(&currentVersion);
  meterLCD.clear();

  funcDial.begin();
  gainDial.begin();

  prodDetAudioBoard.begin();
  mixerBoard.begin();
  freqControl.begin();
  freqControl.initVFOA();
  ampLink.begin();

  cat_Intfc.addCATFSet(catSetFreq);
  cat_Intfc.begin();

}

/******************** loop() *************************************************************************************************/

void loop ()
{

  if(digitalRead(PTT) == LOW) pttStatus = RCV;
  else pttStatus = XMT;

  ampLink.available();                                          // Check for Amplifier
  cat_Intfc.check();

  if(pttStatus == RCV)                                          // RECEIVE LOOP
  {
    if(lastPTTStatus != RCV) 
    {
      digitalWrite(PA_ENB, LOW);
      pttControl.Rcv();
      digitalWrite(ON_AIR_LED, HIGH);
      meterLCD.initializeRxDisplay();
      lastPTTStatus = RCV;
    }
    
    if (freqUpdate != 0)                                      // update VFO frequency if main tuning dial turned
    {
      freqControl.updateVFOA(&freqUpdate);
      freqUpdate = 0;
    }

    if (!digitalRead(FUNC_ENCDR_SEL))
    {
      funcDial.select();
      delay(250);
    }

    if (!digitalRead(GAIN_ENCDR_SEL))
    {
      gainDial.select();
      delay(250);
    }

    if (!digitalRead(TUNE_ENCDR_SEL))
    {
      delay(250);
    }

    if (gainEncdr() != 0)                                 // check if gain dial turned
    {

    }
    
    if (funcEncdr() != 0)                                 // check if function dial turned
    {

    }
  }


  if (pttStatus == XMT)                                  // TRANSMIT LOOP
  {
    if(lastPTTStatus != XMT) 
    {
      pttControl.Xmt();
      digitalWrite(PA_ENB, HIGH);
      digitalWrite(ON_AIR_LED, LOW);
      meterLCD.initializeTxDisplay();
      lastPTTStatus = XMT;
    }
  }


  if (++heartBeat == HEART_BEAT)                        // HEART BEAT
    {
      heartBeat = 0;
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
      meterLCD.indicateVFOA(pttStatus);
      if(pttStatus == RCV) ifBoard.updateSMeter();
      if(pttStatus == XMT) meterLCD.updatePADisplay();
    }


  if (!digitalRead(PWR_SENS))                           // POWER DOWN
  {
    eeprom.saveSettings();                              // save settings
    digitalWrite(PWR_HOLD, LOW);                        // power down
  }

}


/******************** Gain Encoder Routine *******************************************************************************/

uint8_t gainEncdr (void)
{
  unsigned char result = gainRotary.process();

  if (result == DIR_CW)
  {
    gainDial.CW();
    return;
  }

  else if (result == DIR_CCW)
  {
    gainDial.CCW();
    return;
  }
  return 0;
}


/******************** Function Encoder Routine *******************************************************************************/

uint8_t funcEncdr (void)
{
  unsigned char result = funcRotary.process();

  if (result == DIR_CW)
  {
    funcDial.CCW();
    return;
  }

  else if (result == DIR_CCW)
  {
    funcDial.CW();
    return;
  }
  return 0;
}


/******************** Interrupt Routines ************************************************************************************/

void tuneEncdrISR (void)
{
  uint8_t tuneNew = 0;
  uint8_t tuneDiff;

  //if (DH_FLAG) return;
  if (freqUpdate != 0) return;

  if (digitalRead(TUNE_ENCDR_A)) bitSet(tuneNew, 4);
  if (digitalRead(TUNE_ENCDR_B)) bitSet(tuneNew, 5);

  tuneDiff = tuneNew ^ tuneOld;

  switch (tuneDiff)
  {
    case 0x20:
      if (tuneNew == 0x00 | tuneNew == 0x30) freqUpdate += tuningRate;
      else freqUpdate -= tuningRate;
      break;

    case 0x10:
      if (tuneNew == 0x00 | tuneNew == 0x30) freqUpdate -= tuningRate;
      else freqUpdate += tuningRate;
      break;
  }
  tuneOld = tuneNew;

  if (lastFreqUpdate != freqUpdate)
  {
    lastFreqUpdate = freqUpdate;
    freqUpdate = 0;
  }
}


/******************** CAT Routines ******************************************************************************************/

void catSetFreq(long freq)                   // Set Frequency
{
  freqControl.catVFO_A_FreqUpdate(freq);
  digitalWrite(STATUS_LED, LOW);
}


/******************** END OF PROGRAM ****************************************************************************************/
