/**
 * blinds-rx
 * 
 * This routine is mostly for testing.  It uses an Arduino and a 433.92 MHz receiver to receive 
 * and decode signals from a Pellini Screenline remote controller.  It only looks for channel 0
 * codes and it can decode the up/down signals.  It flashes one of two LEDs to indicate correct
 * reception. Note, my blinds are wired backwards so the up button lowers them and vice versa.
 * Yours may be correct, and therefore the functions will be inverted.
 */

const byte PIN = 2; //This pin should be connected to the data out pin of the 433MHz receiver
const byte RED = 8; //This pin is for the RED 'lower' command LED
const byte GREEN = 7; //This pin is for the GREEN 'raise' command LED
const unsigned int _0 = 460; //not actually used but it's the length of a '0' pulse in us
const unsigned int _1 = 830; //not actually used but it's the length of a '1' pulse in us
const unsigned int _SHORT = 4375; //not used, the length of the 'short' gap, in us, before the 48 bytes of data is transmitted
const unsigned int _LONG = 28000; // not used,the length of long gap after the data is transmitted, in ms
const unsigned int ZERO_LLT = 300; //any pulse shorter than this is most likley noise
const unsigned int ZERO_ULT = 500; //the maximum length of pulse that will count as a '0', in us
const unsigned int ONE_ULT = 850; //the maximum length of pulse that will count as a '1', in us 
const unsigned int SHORT_ULT = 4500; //the maximum length of the 'short' gap, in us, before the 48 bytes of data is transmitted
const unsigned int SEQUENCE_TIME = 58000;//not used, the total time for a transmit sequence, including 12 sync pulses.
const byte BUFFER_SIZE = 48; 
//The six bytes of data that are received for a 'raise' and a 'lower' command, at least on my controller
const byte RAISE[BUFFER_SIZE] = {1,1,1,1, 1,1,1,1, 1,1,1,0, 0,1,0,0, 0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,0, 0,0,0,0, 1,1,1,1, 0,1,1,0, 0,0,0,1};
const byte LOWER[BUFFER_SIZE] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,0, 0,1,0,0, 1,1,1,1, 0,1,1,0, 0,1,0,1};
volatile unsigned long t;
volatile unsigned long tau = 0;
volatile byte count = 0;
volatile byte buf[BUFFER_SIZE];
unsigned long rx;
int i;
long tL;
bool rse;
bool lwr;

void setup() {
  //Serial.begin(19200); //uncomment if you want to print out the decoded data in buf[]
  attachInterrupt(digitalPinToInterrupt(PIN), report, CHANGE);  // Receiver on interrupt 0 that is pin 2 for an UNO, pin 3 for a Sparkfun Pro-Micro
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {
  if (count == BUFFER_SIZE) { //we appear to have a complete frame
    delay(10);
    if (count == BUFFER_SIZE) { //we still do after 10ms (near the middle of a long gap) so this frame is valid
      rse = true;
      lwr = true;
      for(i=0;i<BUFFER_SIZE;i++) { //check if we have a raise or lower command
        if (buf[i] != RAISE[i]) rse = false;
        if (buf[i] != LOWER[i]) lwr = false;
        //Serial.print(buf[i]); //uncomment to print out the received data
      }
      //Serial.println(); //uncomment to print out the received data
      
      //flash LEDs to indicate activity
      if (rse) {digitalWrite(GREEN, HIGH); tL = millis();} 
      if (lwr) {digitalWrite(RED, HIGH); tL = millis();}
    }
  }
  if (millis()-tL > 100) {digitalWrite(GREEN, LOW); digitalWrite(RED, LOW);} //turn off indicators after 100ms if no activity
}

void report(){
  //how long since the last interrupt?
  tau = micros() - t; //this long
  t = micros();
  
  //the pin states are read after the pulse has completed hence the apparently reversed state
  if (tau<ZERO_LLT){ //this is just noise
    return;
  }
  
  if ((tau<ZERO_ULT) && (digitalRead(PIN)==HIGH)) { //ZERO pulse
    if (count == BUFFER_SIZE) count=0; //prevent overflow
    buf[count++] = 0;
    return;
  }   
  
  if ((tau<ONE_ULT) && (digitalRead(PIN)==HIGH)) { //ONE pulse
    if (count == BUFFER_SIZE) count=0; //prevent overflow
    buf[count++] = 1;
    return;
  }
  
  if ((tau<SHORT_ULT) && (digitalRead(PIN)==HIGH)) { //SHORT sync pulse 
    count = 0; //reset to the start of the buffer
    return;
  }

  //if we get to here then we have a long pulse or a gap between pulses. These don't actually
  //do anything other than give us main loop processing time so there's no code to enact.
}


