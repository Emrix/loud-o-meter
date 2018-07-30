//Libraries
#include "FastLED.h"
#include <math.h>

//Constants (Feel free to edit these)
#define ZONE_1_PIXEL_QUANTITY 20 //Number of pixels in strand
#define ZONE_2_PIXEL_QUANTITY 30 //Number of pixels in strand
#define ZONE_3_PIXEL_QUANTITY 50 //Number of pixels in strand
#define ZONE_1_HUE 0 //Color for the "Too Loud" section of the strip
#define ZONE_2_HUE 170 //Color for the "Getting" section of the strip
#define ZONE_3_HUE 21 //Color for the "Just right" section of the strip
#define SATURATION 255 //Saturation of all colors in the LED strip
#define BRIGHTNESS 255 //Brightness of all colors in the LED strip
#define LED_TYPE WS2811 //Type of LED strip used
#define COLOR_ORDER GRB //Order of the Colors in the Pixel

#define MIC_PIN A0 //Microphone is attached to this analog pin
#define LED_PIN 13 //LED strand is connected to this pin

#define SAMPLE_WINDOW_TIME 10 //Sample window for average level
#define INPUT_FLOOR 10 //Lower range of mic input
#define INPUT_CEILING 30 //Max range of mic input, the lower the value the more sensitive (1023 = max)
#define COUNTING_THRESHOLD 4 //The volume has to be at least this loud before the counters will increment


//Input Vars
const int NUMBER_OF_PIXELS = ((ZONE_1_PIXEL_QUANTITY) + (ZONE_2_PIXEL_QUANTITY + ZONE_3_PIXEL_QUANTITY));
int SENSITIVITY = 30;
int SAMPLE_TIME = 30;
int ANIMATION_TYPE = 30;
int RESET = 30;

//Global Vars
unsigned int sample;
CRGB strip[NUMBER_OF_PIXELS];


void setup() {
  Serial.begin(9600);
  Serial.println(CRGB::Red);
  analogReference(EXTERNAL);
  FastLED.addLeds < LED_TYPE, LED_PIN, COLOR_ORDER > (strip, NUMBER_OF_PIXELS);
  for (int x = 0; x < NUMBER_OF_PIXELS; x++) {
    strip[x] = CRGB::Black;
  }
  FastLED.show();
}


float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve) {
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;

  // condition curve parameter
  // limit range
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1); //  invert and scale  this seems more intuitive   postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  } else {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal = zeroRefCurVal / OriginalRange; // normalize to 0   1 float

  // Check for originalMin > originalMax  the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax) {
    return 0;
  }

  if (invFlag == 0) {
    rangedValue = (pow(normalizedCurVal, curve) * NewRange) + newBegin;
  } else // invert the ranges
  {
    rangedValue = newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}

float soundMeasurement() {
  unsigned long startMillis = millis(); // Start of sample window
  float peakToPeak = 0; // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y;

  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW_TIME) {
    sample = analogRead(MIC_PIN);
    if (sample < 1024) { // toss out spurious readings
      if (sample > signalMax) {
        signalMax = sample; // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample; // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin; // max  min = peak-peak amplitude
  return peakToPeak;
}

int pixelDisplay(float peakToPeak) {
  unsigned int c;
  //Colorize the Good Pixels
  for (int i = 0; i <= ZONE_3_PIXEL_QUANTITY - 1; i++) {
    strip[i] = CHSV(ZONE_3_HUE, SATURATION, BRIGHTNESS);
  }
  //Colorize the Warning Pixels
  for (int i = 0; i <= ZONE_2_PIXEL_QUANTITY - 1; i++) {
    strip[i + ZONE_3_PIXEL_QUANTITY] = CHSV(ZONE_2_HUE, SATURATION, BRIGHTNESS);
  }
  //Colorize the Bad Pixels
  for (int i = 0; i <= ZONE_1_PIXEL_QUANTITY - 1; i++) {
    strip[i + ZONE_2_PIXEL_QUANTITY + ZONE_3_PIXEL_QUANTITY] = CHSV(ZONE_1_HUE, SATURATION, BRIGHTNESS);
  }

  //Scale the input logarithmically instead of linearly
  c = fscale(INPUT_FLOOR, INPUT_CEILING, NUMBER_OF_PIXELS, 0, peakToPeak, 2);

  if (c <= NUMBER_OF_PIXELS) { // Fill partial column with off pixels
    uint8_t from = NUMBER_OF_PIXELS;
    uint8_t to = NUMBER_OF_PIXELS - c;
    uint8_t fromTemp;
    if (from > to) {
      fromTemp = from;
      from = to;
      to = fromTemp;
    }
    for (int i = from; i <= to; i++) {
      strip[i] = CRGB::Black;
    }
  }

  FastLED.show();
  return 0;
}

void counterDisplay(int zone) {
  
}

void loop() {
  float averageLevel = soundMeasurement();
  unsigned int zone = pixelDisplay(averageLevel);
  pixelDisplay(zone);
}


