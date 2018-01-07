/**
 * blinds-tx
 * 
 * This sketch operates a 433.92 MHz transmitter to control some Pellini/Screenline blinds.  It works by re-transmitting the 
 * data that I decoded from a Pellini remote.  See other info in blinds-rx. Note that my definition of '0' and '1' is 
 * arbitrary.  This is a test routine.
 */


const int ZERO = 398; //length of a '0' pulse
const int ONE = 785; //length of a '1' pulse
const int SHORT = 4408; //length of the short gap between the 12 sync pulses and the 6 bytes of data
const int LONG = 28; //ms not us, the length of the long gap before the whole tx sequence repeats
const int SYNC = 12; //number of sync pulses to send: try 11 if this doesn't work
const int BITS = 48;
//the actual data to send to command the blinds
const byte RAISE[BITS] = {1,1,1,1, 1,1,1,1, 1,1,1,0, 0,1,0,0, 0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,0, 0,0,0,0, 1,1,1,1, 0,1,1,0, 0,0,0,1};
const byte LOWER[BITS] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,0, 0,1,0,0, 1,1,1,1, 0,1,1,0, 0,1,0,1};
const int OUT = 4; //the pin to which the transmitter is connected

void setup() {
  pinMode(OUT, OUTPUT);
  digitalWrite(OUT, LOW); //just to make sure

}

void loop() {
  sendCommand(OUT, RAISE); //change RAISE to LOWER to lower the blinds
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
    if (data[i] == 1) {
      sendOne(OUT); 
    } else {
      sendZero(OUT);
    }
  }
}
