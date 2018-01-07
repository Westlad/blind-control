/**
 * Blinds
 * 
 * This sketch combines blinds-tx with the LwRx library so that it can recive a command from a lightwaveRF hub
 * and then control some Pellini/Screenline blinds. I took a simple approach with LwRx and just set up a device
 * then used it to look at the code for that device and hardcoded the result.  A more complete approach would allow
 * for device pairing as LwRx has that functionality.
 */

#include <LwRx.h>

const int ZERO = 398; //the time for a zero bit in us
const int ONE = 785; //the time for a one bit in us
const int SHORT = 4408; //the time for the short low before data transmission
const int LONG = 28; //ms not us
const int SYNC = 12; //number of sync pulses to send: try 11 if this doesn't work
const int BITS = 48;
//the Pellinin raise and lower codes
const byte RAISE[BITS] = {1,1,1,1, 1,1,1,1, 1,1,1,0, 0,1,0,0, 0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,0, 0,0,0,0, 1,1,1,1, 0,1,1,0, 0,0,0,1};
const byte LOWER[BITS] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,0, 0,1,0,0, 1,1,1,1, 0,1,1,0, 0,1,0,1};
const byte msglen = 10;
//The codes that will be received from the LightwaveRF hub
const byte OPEN[msglen] = {1, 15, 1, 7, 0, 4, 8, 1, 7, 0};
const byte CLOSE[msglen] = {0, 0, 1, 7, 0, 4, 8, 1, 7, 0};
const int OUT = 3; //the pin connected to the tx
const int GREEN = 7; //green LED
const int RED = 8; //red LED
const unsigned int OP_TIME = 15000; //the time that the lower/raise command will be repeated for (simulates a button press-and-hold)

const int RX_PIN = 2; //pin to receive rx data on
const int RX_POWER = 4; // connect to receiver Vcc
byte msg[10];
bool cse = false;
bool opn = false;
unsigned long t;

void setup() {
  pinMode(OUT, OUTPUT);
  digitalWrite(OUT, LOW); //turn off the transmit so you can hear the LightwaveRF rception
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(RX_POWER, OUTPUT);
  digitalWrite(RX_POWER, HIGH);
  Serial.begin(19200);
  lwrx_setup(RX_PIN);
  t = millis();
}

void loop() {
  if (lwrx_message()) { //we have received a message from the LightwaveRF hub
    lwrx_getmessage(msg, msglen);
    digitalWrite(RX_POWER, LOW); //the transmitter causes interrupts
    digitalWrite(OUT, HIGH); //turn the TX on
    opn = true;
    cse = true;
    for (int i =0; i<msglen; i++){ //is it an up or a down message?
      if (msg[i] != CLOSE[i]) cse = false;
      if (msg[i] != OPEN[i]) opn = false;
    }
    if (opn) {
      digitalWrite(GREEN,HIGH);
      t = millis() + OP_TIME;
      while(t>millis()) {
        sendCommand(OUT, RAISE);
      }
      digitalWrite(GREEN,LOW);
    }
    if (cse) {
      digitalWrite(RED,HIGH);
      t = millis() + OP_TIME;
      while(t>millis()) {
        sendCommand(OUT, LOWER);
      }
      digitalWrite(RED,LOW);
    }
    digitalWrite(OUT, LOW); //turn the TX off
    digitalWrite(RX_POWER, HIGH);//and the rx on
  }
}

void sendCommand(int pin, byte command[]){
  sendSync(OUT);
  sendShort(OUT);
  sendData(OUT, command);
  sendLong(OUT);
}

void sendOne(int pin){
  delayMicroseconds(ZERO);
  digitalWrite(pin, LOW);
  delayMicroseconds(ONE);
  digitalWrite(pin, HIGH);
}

void sendZero(int pin){
  delayMicroseconds(ONE);
  digitalWrite(pin, LOW);
  delayMicroseconds(ZERO);
  digitalWrite(pin, HIGH);
}

void sendShort(int pin){
  delayMicroseconds(ZERO);
  digitalWrite(pin, LOW);
  delayMicroseconds(SHORT);
  digitalWrite(pin, HIGH);
}

void sendLong(int pin){
  delayMicroseconds(ZERO);
  digitalWrite(pin, LOW);
  delay(LONG); //can't time this long with delayMicroseconds
  digitalWrite(pin, HIGH);
}

void sendSync(int pin){
  for (int i=0; i<SYNC; i++){
    delayMicroseconds(ZERO);
    digitalWrite(pin, LOW);
    delayMicroseconds(ZERO);
    digitalWrite(pin, HIGH);
  }  
}

void sendData(int pin, byte data[]){
  for (int i=0; i<BITS; i++){
    //Serial.print(bitRead(data,i));
    //delay(1000);
    if (data[i] == 1) {
      sendOne(OUT); 
    } else {
      sendZero(OUT);
    }
  }
}

void printMsg(byte *msg, byte len) {
   for(int i=0;i<len;i++) {
      Serial.print(msg[i]);
      Serial.print(" ");
   }
   Serial.println();
}

void flash(byte pin) {
   digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
   delay(100);                       // wait for a second
   digitalWrite(pin, LOW);    // turn the LED off by making the voltage LOW
}


