
/*  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

----------------------  Notes ----------------------  ---------------------- 
This code:
1) Blinks a HEART animation
2) ?? 
3) Determines BPM
4) Prints All of the Above to Serial

Read Me:
https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md   
 ----------------------       ----------------------  ----------------------
*/

#include "LedControl.h"

/* using VCC, GND, DIN 20, CS 21, CLK 5 for the MAX7219 */
//static const int DATA_PIN = 12;
//static const int CS_PIN   = 10;
//static const int CLK_PIN  = 11;
static const int DATA_PIN = 20;
static const int CLK_PIN  = 5;
static const int CS_PIN   = 21;

LedControl lc=LedControl(DATA_PIN, CLK_PIN, CS_PIN, 1);


//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

int heartFade = 0;
int heartOpen = 0;

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 


void setup(){
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
   // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE, 
   // AND APPLY THAT VOLTAGE TO THE A-REF PIN
//   analogReference(EXTERNAL);   
    /*
     The MAX72XX is in power-saving mode on startup,
     we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,1);
  /* and clear the display */
  lc.clearDisplay(0);
}



const byte heart2[8] PROGMEM = {
  B00000000,
  B01100110,
  B10011001,
  B10000001,
  B10000001,
  B01000010,
  B00100100,
  B00011000};

/* here is the data for the characters */
const byte heart1[8] PROGMEM = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000};
  


//  Where the Magic Happens
void loop(){
  
    //serialOutput() ;       
    
  if (QS == true && BPM < 200 && BPM > 50){     //  A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
        digitalWrite(blinkPin,HIGH);     // Blink LED, we got a beat. 
        render(heart1);
        heartFade = 15;
        lc.setIntensity(0,heartFade);
        heartOpen = 100;
        //delay(200);
        
        serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
        QS = false;                      // reset the Quantified Self flag for next time    
       } 
      else { 

      if (heartFade > 0) {
        heartFade--;
        lc.setIntensity(0,heartFade);
      } else if (heartOpen > 0) {
        heartOpen--;
      } else if (heartOpen == 0) {
        heartOpen--;
        render(heart2);
      }
      digitalWrite(blinkPin,LOW);            // There is not beat, turn off pin 13 LED
        
      }
   
  delay(20);                             //  take a break
}

int render(const byte* frame) {
        // render to screen
        for (int r = 0; r < 8; r++) {
          lc.setColumn(0, 7-r, pgm_read_byte(&frame[r]));
        }
}


