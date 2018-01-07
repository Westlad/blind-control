# Blind Control

I recently had some Pellini/Screenline blinds installed.  These are great but the radio control leaves something to be desired. The basic up-down control has to be pressed and held for several seconds while the blinds sort themselves out and start to open or close.  Also, there's no possibility to integrate with my home automation system.

I had a lightwaveRF hub and there is a great library (https://github.com/pkourany/LightWaveRF_Tidey_Library) for working with an Arduino.  All I needed now was a way to transmit blind control codes to the blinds.  I considered buying a second controller and using the Arduino to electronically press the buttons but the controllers are a very steep 156 Euros.  Instead, I came up with this code which enables an Arduino to transmit the comands via a 433 MHz low power transmitter. The code also allows for reception of LightwaveRF signals with a 433 MHz receiver.

I had some difficulty with the polarity of the signal: my receiver seems to invert low and high signal levels.  If yours doesn't, you may need to adjust the number of sync pulses as shown in the code.

Finally, please note this is a quick hack - not designed for serious production use.  Use at your own risk.
