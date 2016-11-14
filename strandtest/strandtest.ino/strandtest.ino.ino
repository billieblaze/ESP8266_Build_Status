// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

// todo: pixels per strip 
#define INDICATOR_PIXELS  6
#define RING1_PIXELS  11
#define RING2_PIXELS  11 

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4  // 5,4,0

// todo: seperate data lines for each strip
#define INDICATOR_DATA  5
#define RING1_DATA  4
#define RING2_DATA  0

//shared clock pin..    always be clocking, let each row send data when it needs to.. 
#define CLOCKPIN   16

// one instance per led strip 
Adafruit_DotStar indicator = Adafruit_DotStar( INDICATOR_PIXELS, INDICATOR_DATA, CLOCKPIN, DOTSTAR_BRG);
Adafruit_DotStar ring1 = Adafruit_DotStar( RING1_PIXELS, RING1_DATA, CLOCKPIN, DOTSTAR_BRG);\
Adafruit_DotStar ring2 = Adafruit_DotStar( RING2_PIXELS, RING2_DATA, CLOCKPIN, DOTSTAR_BRG);

void setup() {

  indicator.begin(); // Initialize pins for output
  indicator.show();  // Turn all LEDs off ASAP

  ring1.begin(); // Initialize pins for output
  ring1.show();  // Turn all LEDs off ASAP

  ring2.begin(); // Initialize pins for output
  ring2.show();  // Turn all LEDs off ASAP
  
}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

int      head_indicator  = 0, tail_indicator = -10; // Index of first 'on' and 'off' pixels
int      head_ring1  = 0, tail_ring1 = -10; // Index of first 'on' and 'off' pixels
int      head_ring2  = 0, tail_ring2 = -10; // Index of first 'on' and 'off' pixels

uint32_t color_indicator = 0xFF0000;      // 'On' color (starts red)
uint32_t color_ring1 = 0xFF0000;      // 'On' color (starts red)
uint32_t color_ring2 = 0xFF0000;      // 'On' color (starts red)

void loop() {
 // indicator
  indicator.setPixelColor(head_indicator, color_indicator); // 'On' pixel at head
  indicator.setPixelColor(tail_indicator, 0);     // 'Off' pixel at tail
  indicator.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if(++head_indicator >= INDICATOR_PIXELS) {         // Increment head index.  Off end of strip?
    head_indicator = 0;                       //  Yes, reset head index to start
    if((color_indicator >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
      color_indicator = 0xFF0000;             //   Yes, reset to red
  }
  if(++tail_indicator >= INDICATOR_PIXELS) tail_indicator = 0; // Increment, reset tail index

 // ring 1
 ring1.setPixelColor(head_ring1, color_ring1); // 'On' pixel at head
  ring1.setPixelColor(tail_ring1, 0);     // 'Off' pixel at tail
  ring1.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if(++head_ring1 >= RING1_PIXELS) {         // Increment head index.  Off end of strip?
    head_ring1 = 0;                       //  Yes, reset head index to start
    if((color_ring1 >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
      color_ring1 = 0xFF0000;             //   Yes, reset to red
  }
  if(++tail_ring1 >= RING1_PIXELS) tail_ring1 = 0; // Increment, reset tail index

// ring 2
 ring2.setPixelColor(head_ring2, color_ring2); // 'On' pixel at head
  ring2.setPixelColor(tail_ring2, 0);     // 'Off' pixel at tail
  ring2.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)

  if(++head_ring2 >= RING2_PIXELS) {         // Increment head index.  Off end of strip?
    head_ring2 = 0;                       //  Yes, reset head index to start
    if((color_ring2 >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
      color_ring2 = 0xFF0000;             //   Yes, reset to red
  }
  if(++tail_ring2 >= RING2_PIXELS) tail_ring2 = 0; // Increment, reset tail index


}
