#ESP8266 Build Status Monitor

Inspired by the Dynatrace UFO.  ESP8266 with REST API and Angular Dashboard controlling Dotstar LED Strip

I managed to build this without the 3d printed enclosure!
First, I cut the top off of a cd spindle, spray painted it lightly with white paint and laser cut some plywood circles to seperate the led strips..
I spray painted a piece of acrylic black on the front and lightly with white on the back and laser cut the logo into the black part.    I glued this onto the CD spindle with 2 strips of 3 leds behind it.   Beneath that is a wooden circle (about 125mm, measure you spindle cover).  Next, a piece of LED strip, i think i got like 11 pixels in there.  Secure with a little hot glue and repeat for the next layer.

wiring is pretty much from here: https://github.com/Dynatrace/ufo/tree/master/howto
check the sketch for the exact pin mapping!

basically, wire all the + / - back to a perfboard with the ESP on it..    all LED strips share CI, and have seperate DI connections to the uC board..

the dynatrace ufo recommends a 2A charger for power, so i'm doing that too.

## Installing 
* Install Arduino IDE 1.6.7+ and in the Preferences dialog, type this board manager URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json Arduino IDE preferences Make sure to restart the IDE afterwards.

* Install the ESP8266 board using Arduino IDE Board Manager under the Tools menu. Arduino board manager

* Configure the microcontroller in IDE

** Adafruit Huzzah ESP8266 board
** 3MB SPIFFS
** serial port that's connected to the FTDI serial cable
** USBtinyISP as Programmer
** 115200bps board config

* Add Adafruit DotStar library and arduinoJSON libraries in the library manager
