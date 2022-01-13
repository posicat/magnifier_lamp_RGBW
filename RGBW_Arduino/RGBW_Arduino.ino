// Circular LED for magnifying lamp controller - by posicat@pobox.com
// Public domain, everyone should have a smart-magnifying lamp!!!
// Originally programmed on a Digispark Oak, should work on most ESP8266's

// Based on code from test sketch which also supports RGBW strips like SK6812 based:
// http://www.szledcolor.com/download/SK6812RGBW.pdf
// Should be used with Adafruit NeoPixel version 1.0.3
// Hacked by G. Knauf 2015-12-06

// Edit config.h for configuration values
#include "config.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define RED   0x00FF0000 // --> RED
#define GREEN   0x0000FF00 // --> GREEN
#define BLUE    0x000000FF // --> BLUE
//#if (((NEO_PTYPE & 0xC0) >> 2) != (NEO_PTYPE & 0x30)) // check for RGBW type
//#define WHITE   0xFF000000 // --> WHITE LED
#define WHITE   0xFFFFFFFF // --> WHITE LED
//#else
//#define WHITE   0x00FFFFFF // --> WHITE RGB composed
//#endif

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (f.e. SK6812)

// What type of NeoPixel strip is attached to the Arduino?
#define NEO_PTYPE NEO_GRBW

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, esp8266_pinToGpio[NEO_PIN], NEO_PTYPE + NEO_KHZ800);

#define MODES 3
#define MODE_BRIGHT 0
#define MODE_ARC_START 1
#define MODE_ARC_OFFSET 2
  
int mode = 0; // When the encoder is pressed we switch modes, this keeps track of the mode number.

int modeValue[MODES]; // For each mode rotating the encoder will alter it's own value
int modeDeltas[MODES] = {8,4,4};
bool modeRolls[MODES] = {false,false,true};

bool rangeChange=false;
bool buttonPress=false;

void callbackMovement(int dir,int spd) {
  modeValue[mode]+=modeDeltas[mode]*dir;
  modeValue[mode]=constrainRange(modeValue[mode],0,255,modeRolls[mode]);
  debug("Movement : " + String(dir) + " (" + String(spd) + "ms) Mode : " + String(mode) + " Val : " + String(modeValue[mode]));
  rangeChange=true;
}

void callbackButton(int spd) {
  mode++;
  mode=constrainRange(mode,0,MODES-1,true);
  debug("Button : " + String(spd) + " Mode : " + String(mode));
  buttonPress=true;
}

void setup() {
  for (int i=0;i<MODES;i++) {modeValue[i]=0;}
  
  setup_serial();
  setup_pixels();
  setup_encoder();

  setAllPixels(WHITE);
  strip.show();
}

void loop() {
  bool updatePixels = false;

  if (rangeChange || buttonPress) {
    updatePixels=true;
    rangeChange=false;
    buttonPress=false;    
  }
  
  if (updatePixels) {
      // Here we loop through the 4 colors in the colors array:
      // RED, GREEN, BLUE, LED_WHITE

      int bright = 255 - (modeValue[MODE_BRIGHT]);
      strip.setBrightness(bright); // set brightness

      setPixelArc(255-modeValue[MODE_ARC_START],modeValue[MODE_ARC_OFFSET],WHITE );

      strip.show();
      updatePixels=false;
  }
}

int constrainRange(int curVal,int minVal,int maxVal,bool rollValue) {
  if (curVal < minVal) {
    if (rollValue) {
        curVal=maxVal;
    }else{
        curVal=minVal;
    }
  }
  if (curVal > maxVal) {
    if (rollValue) {
      curVal=minVal;
    }else{
      curVal=maxVal;
    }
  }
  return curVal;
}

void setPixelArc(int arcSize,int arcOffset, uint32_t setColor) {
  //String msg="";
  int offset=0;
  int pixelRingCount= sizeof(pixelRing)/sizeof(uint32_t);
  
  for(int i=0 ; i < pixelRingCount ; i++) { // Go through all rings in the list
    for (int o=0 ; o < pixelRing[i] ; o++) { // Loop all pixels, default off

      int arcPos = o*256 / pixelRing[i];
      int offPos = (arcPos + arcOffset) % 256;

      //msg = String(o)+"->"+String(arcPos)+"->"+String(offPos);
       
      int color = 0;
      if (offPos<=arcSize) { // In the arc
          color=setColor;
          //msg+="C";
      }else{
          //msg+="_";
          color=0;
      }
      strip.setPixelColor(offset+o, color);
      //debug(msg);
    }
    offset+=pixelRing[i]; // Start the next offset at the begining of the next ring
  }
}

void setAllPixels(uint32_t c) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}

void setup_pixels() {
    // ESP8266 pin mapping to GPIO is confusing, fix that by setting the RIGHT pins here.
    pinMode ( esp8266_pinToGpio[NEO_PIN] , OUTPUT );
    digitalWrite ( esp8266_pinToGpio[NEO_PIN], 0 );
    strip.begin();
  
    // Return to normal code with no hacks
    strip.setBrightness(255); // set brightness
    setAllPixels(WHITE);
    strip.show();
}
