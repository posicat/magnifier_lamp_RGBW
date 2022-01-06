// Enable serial output
#define SERIAL_DEBUG

                               //0, 1, 2, 3, 4, 5,  6,  7,  8,  9, 10, 11
uint8_t esp8266_pinToGpio[12] = {2, 5, 0, 3, 1, 4, 15, 13, 12, 14, 16, 17};

// Define IO pins for the lamp
const int ROT_PIN_OUTPUT_A = esp8266_pinToGpio[8];
const int ROT_PIN_OUTPUT_B = esp8266_pinToGpio[7];
const int ROT_PIN_PUSH_BUT = esp8266_pinToGpio[9];
const int NEO_PIN          = esp8266_pinToGpio[4];

// INDICATOR + sum of all pixels in pixelRing
#define NUMPIXELS 108

// The LEDs in your ring, one entry per ring, in the order the rings are wired
// Needed to calculate the arc-segment of the ring to light
uint32_t pixelRing[2] = {60,48}; 
