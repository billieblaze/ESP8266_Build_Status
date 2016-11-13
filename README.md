#ESP8266 Build Status Monitor

Inspired by the Dynatrace UFO.  ESP8266 with REST API and Angular Dashboard controlling Dotstar LED Strip

I managed to build this without the 3d printed enclosure!
First, I cut the top off of a cd spindle, spray painted it lightly with white paint and laser cut some plywood circles to seperate the led strips..
I spray painted a piece of acrylic black on the front and lightly with white on the back and laser cut the logo into the black part.    I glued this onto the CD spindle with 2 strips of 3 leds behind it.   Beneath that is a wooden circle (about 125mm, measure you spindle cover).  Next, a piece of LED strip, i think i got like 11 pixels in there.  Secure with a little hot glue and repeat for the next layer.

wiring is pretty much from here: https://github.com/Dynatrace/ufo/tree/master/howto
check the sketch for the exact pin mapping!

basically, wire all the + / - back to a perfboard with the ESP on it..    all LED strips share CI, and have seperate DI connections to the uC board..

the dynatrace ufo recommends a 2A charger for power, so i'm doing that too.