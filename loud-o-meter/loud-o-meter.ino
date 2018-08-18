//Libraries
#include "FastLED.h"
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


//Constants (Feel free to edit these)
#define ZONE_RED_PIXEL_QUANTITY 5
#define ZONE_YELLOW_PIXEL_QUANTITY 15
#define ZONE_GREEN_PIXEL_QUANTITY 30
#define ZONE_RED_HUE 0
#define ZONE_YELLOW_HUE 50
#define ZONE_GREEN_HUE 100
#define SATURATION 255
#define BRIGHTNESS 100
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

#define MIC_PIN A0
#define LED_PIN 12

#define SAMPLE_WINDOW_TIME 100
#define INPUT_FLOOR 10 //Lower range of mic input
#define INPUT_CEILING 30 //Max range of mic input, the lower the value the more sensitive (1023 = max)
#define COUNTING_MIN_THRESHOLD 10 //The volume has to be at least this many pixels before the counters will increment

#define COUNTING_MULTIPLE 1 //The amount of times that the system needs to sample the audio before it will count


//Global Vars
const int NUMBER_OF_PIXELS = ((ZONE_RED_PIXEL_QUANTITY) + (ZONE_YELLOW_PIXEL_QUANTITY + ZONE_GREEN_PIXEL_QUANTITY));
unsigned int sample;
CRGB strip[NUMBER_OF_PIXELS];
Adafruit_7segment zoneRedCounter = Adafruit_7segment();
//Adafruit_7segment zoneYellowCounter = Adafruit_7segment();
Adafruit_7segment zoneGreenCounter = Adafruit_7segment();
int zoneRedCounterQuantity;
//int zoneYellowCounterQuantity;
int zoneGreenCounterQuantity;
int loopNumber = 0;



//Functions
unsigned int soundMeasurement();
unsigned int pixelDisplay(float);
void counterDisplay(int);
float fscale(float, float, float, float, float, float);



void setup() {
  Serial.begin(9600);
  analogReference(EXTERNAL);
  FastLED.addLeds < LED_TYPE, LED_PIN, COLOR_ORDER > (strip, NUMBER_OF_PIXELS);
  for (int x = 0; x < NUMBER_OF_PIXELS; x++) {
    strip[x] = CRGB::Black;
  }
  FastLED.show();
  zoneRedCounter.begin(0x70);
  //zoneYellowCounter.begin(0x75);
  zoneGreenCounter.begin(0x71);
  zoneRedCounterQuantity = 0;
  //zoneYellowCounterQuantity = 0;
  zoneGreenCounterQuantity = 0;
}



void loop() {
  Serial.println(zoneRedCounterQuantity);
  Serial.println(zoneGreenCounterQuantity);
  unsigned int averageLevel = soundMeasurement();
  unsigned int zone = pixelDisplay(averageLevel);
  if (loopNumber >= (COUNTING_MULTIPLE - 1) || zone == 1) {
    counterDisplay(zone);
    loopNumber = 0;
  } else {
    loopNumber++;
  }
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

unsigned int soundMeasurement() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0; // peak-to-peak level
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

unsigned int pixelDisplay(float peakToPeak) {
  unsigned int c;
  //Colorize All of the Pixels
  for (int i = 0; i <= ZONE_GREEN_PIXEL_QUANTITY - 1; i++) {
    strip[i] = CHSV(ZONE_GREEN_HUE, SATURATION, BRIGHTNESS);
  }
  for (int i = 0; i <= ZONE_YELLOW_PIXEL_QUANTITY - 1; i++) {
    strip[i + ZONE_GREEN_PIXEL_QUANTITY] = CHSV(ZONE_YELLOW_HUE, SATURATION, BRIGHTNESS);
  }
  for (int i = 0; i <= ZONE_RED_PIXEL_QUANTITY - 1; i++) {
    strip[i + ZONE_YELLOW_PIXEL_QUANTITY + ZONE_GREEN_PIXEL_QUANTITY] = CHSV(ZONE_RED_HUE, SATURATION, BRIGHTNESS);
  }
  c = fscale(INPUT_FLOOR, INPUT_CEILING, NUMBER_OF_PIXELS, 0, peakToPeak, 2);  //Scale the input logarithmically instead of linearly
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
  unsigned int zone = 0;
  if (c > COUNTING_MIN_THRESHOLD) {
    zone = 3;
  }
  if (c > (ZONE_GREEN_PIXEL_QUANTITY)) {
    zone = 2;
  }
  if (c > (ZONE_GREEN_PIXEL_QUANTITY + ZONE_YELLOW_PIXEL_QUANTITY)) {
    zone = 1;
  }
  return zone;
}

void counterDisplay(unsigned int zone) {
  switch (zone) {
    case 1:
      zoneRedCounterQuantity += 1;
      break;
    case 2:
      //zoneYellowCounterQuantity++;
      break;
    case 3:
      zoneGreenCounterQuantity++;
      break;
    default:
      //If it ever hits this, we got something bad going on...
      break;
  }
  if (zone == 1) {zoneRedCounterQuantity++;}
  if (zone == 3) {zoneGreenCounterQuantity++;}
    
  zoneRedCounter.print(zoneRedCounterQuantity, DEC);
  //zoneYellowCounter.print(zoneYellowCounterQuantity, DEC);
  zoneGreenCounter.print(zoneGreenCounterQuantity, DEC);
  zoneRedCounter.writeDisplay();
  //zoneYellowCounter.writeDisplay();
  zoneGreenCounter.writeDisplay();
}
