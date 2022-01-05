// Circular LED for magnifying lamp controller - by posicat@pobox.com
// Public domain, everyone should have a smart-magnifying lamp!!!
// Originally programmed on a Digispark Oak, should work on most ESP8266's

// Based on code from test sketch which also supports RGBW strips like SK6812 based:
// http://www.szledcolor.com/download/SK6812RGBW.pdf
// Should be used with Adafruit NeoPixel version 1.0.3
// Hacked by G. Knauf 2015-12-06

#include <Adafruit_NeoPixel.h>
#include <CFRotaryEncoder.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Enable serial output
#define SERIAL_DEBUG

                               //0, 1, 2, 3, 4, 5,  6,  7,  8,  9, 10, 11
uint8_t esp8266_pinToGpio[12] = {2, 5, 0, 3, 1, 4, 15, 13, 12, 14, 16, 17};

// Define IO pins for the lamp
const int ROT_PIN_OUTPUT_A = esp8266_pinToGpio[8];
const int ROT_PIN_OUTPUT_B = esp8266_pinToGpio[7];
const int ROT_PIN_PUSH_BUT = esp8266_pinToGpio[9];
const int NEO_PIN          = esp8266_pinToGpio[4];


#define RED   0x00FF0000 // --> RED
#define GREEN   0x0000FF00 // --> GREEN
#define BLUE    0x000000FF // --> BLUE
#if (((NEO_PTYPE & 0xC0) >> 2) != (NEO_PTYPE & 0x30)) // check for RGBW type
#define WHITE   0xFF000000 // --> WHITE LED
#else
#define WHITE   0x00FFFFFF // --> WHITE RGB composed
#endif

uint32_t colors[4] = {RED, GREEN, BLUE, WHITE};

// The LEDs in your ring, one entry per ring, in the order the rings are wired
// Needed to calculate the arc-segment of the ring to light
uint32_t pixelRing[2] = {60,48}; 

// INDICATOR + sum of all pixels in pixelRing
#define NUMPIXELS 108

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (f.e. SK6812)

// What type of NeoPixel strip is attached to the Arduino?
#define NEO_PTYPE NEO_GRBW  // f.e. SK6812
//#define NEO_PTYPE NEO_GRB   // most NeoPixel products
#define BRIGHTNESS  256 // set max brightness

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, esp8266_pinToGpio[NEO_PIN], NEO_PTYPE + NEO_KHZ800);

CFRotaryEncoder rotaryEncoder(ROT_PIN_OUTPUT_A, ROT_PIN_OUTPUT_B, ROT_PIN_PUSH_BUT);

#define MODES 3
#define MODE_BRIGHT 0
#define MODE_ARC_START 1
#define MODE_ARC_END 2
  
int mode = 0; // When the encoder is pressed we switch modes, this keeps track of the mode number.

int modeValue[MODES]; // For each mode rotating the encoder will alter it's own value
for (int i=0;i<MODES;i++) {modeValue[i]=0;}

bool rangeChange=false;
bool buttonPress=false;

void setup() {
  setup_serial();
  setup_pixels();
  setup_encoder();

  setAllPixels(WHITE);
  strip.show();
}

void loop() {
  rotaryEncoder.loop();

  bool updatePixels = false;

  if (rangeChange || buttonPress) {
    updatePixels=true;
    rangeChange=false;
    buttonPress=false;    
  }
  
  if (updatePixels) {
      // Here we loop through the 4 colors in the colors array:
      // RED, GREEN, BLUE, LED_WHITE

      setPixelArc(modeValue[MODE_ARC_START],255-modeValue[MODE_ARC_END],WHITE );
      strip.setBrightness(BRIGHTNESS-modeValue[MODE_BRIGHT]); // set brightness
      updatePixels=false;
  }
}

int constrainRange(int curVal,int minVal,int maxVal) {
  while (curVal < minVal) {
    curVal+=maxVal;
  }
  while (curVal > maxVal) {
    curVal-=maxVal;
  }
  return curVal;
}

void setPixelArc(int startArc,int endArc, uint32_t setColor) {
  String msg="";
  int offset=0;
  int pixelRingCount= sizeof(pixelRing)/sizeof(uint32_t);
  
  for(int i=0 ; i < pixelRingCount ; i++) { // Go through all rings in the list
    msg = "Ring : " + String(i) + " [0 .. " + String(pixelRing[i]) + "] ... ";

    float sa = (startArc*pixelRing[i])/256; // First pixel lit
    float ea = (endArc*pixelRing[i])/256; // Last pixel lit

    msg += "["+String(startArc)+"="+String(sa)+".."+String(endArc)+"="+String(ea)+"] ... ";
    
    for (int o=0 ; o < pixelRing[i] ; o++) { // Loop all pixels, default off
      int color = 0;
      if (o > sa && o < ea) { // In the arc
          color=setColor;
          msg+="C";
      }else{
          msg+="_";
          color=0;
      }
      strip.setPixelColor(offset+o, color);
    }
    debug(msg);
    offset+=pixelRing[i]; // Start the next offset at the begining of the next ring
  }
}

void setAllPixels(uint32_t c) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}

void rotaryAfterChangeValueCallback() {
    if (rotaryEncoder.getDirection() > 0) {
        modeValue[mode]++;
        //debug("Clockwise: ");
    } else {
        modeValue[mode]+-;
        //debug("Counter Clockwise: ");
    }
    modeValue[mode]=constrainRange(modeValue[mode],0,255);
    rangeChange=true;
}

void rotaryOnPressCallback() {
    //debug("Button was pressed.");
    mode++;
    mode=constrainRange(mode,0,MODES);
    buttonPress=true;
}

void setup_pixels() {
    // ESP8266 pin mapping to GPIO is confusing, fix that by setting the RIGHT pins here.
    pinMode ( esp8266_pinToGpio[NEO_PIN] , OUTPUT );
    digitalWrite ( esp8266_pinToGpio[NEO_PIN], 0 );
    strip.begin();
  
    // Return to normal code with no hacks
    strip.setBrightness(BRIGHTNESS-modeValue[MODE_BRIGHT]); // set brightness
    strip.show(); // Initialize all pixels to 'off'
}

void setup_serial() {
  #ifdef SERIAL_DEBUG
    // initialize serial communication:
    while (!Serial);  // for Leonardo/Micro/Zero
    Serial.begin(74880);
    debug("Starting.");
    delay(100);
  #endif
}

void setup_encoder() {
    rotaryEncoder.setAfterRotaryChangeValueCallback(rotaryAfterChangeValueCallback);
    rotaryEncoder.setPushButtonOnPressCallback(rotaryOnPressCallback);
    rotaryEncoder.begin();
    debug("rotaryEncoder.begin()");
    delay(100);
}

void debug (String msg) {
  #ifdef SERIAL_DEBUG
    Serial.println(msg);
  #endif
}
