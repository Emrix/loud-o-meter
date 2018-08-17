//Libraries
#include "FastLED.h"
#include <math.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


//Constants (Feel free to edit these)
#define ZONE_1_PIXEL_QUANTITY 20 //Number of pixels in strand
#define ZONE_2_PIXEL_QUANTITY 30 //Number of pixels in strand
#define ZONE_3_PIXEL_QUANTITY 50 //Number of pixels in strand
#define ZONE_1_HUE 0 //Color for the "Too Loud" section of the strip
#define ZONE_2_HUE 50 //Color for the "Getting" section of the strip
#define ZONE_3_HUE 100 //Color for the "Just right" section of the strip
#define SATURATION 255 //Saturation of all colors in the LED strip
#define BRIGHTNESS 255 //Brightness of all colors in the LED strip
#define LED_TYPE WS2811 //Type of LED strip used
#define COLOR_ORDER GRB //Order of the Colors in the Pixel

#define MIC_PIN A0 //Microphone is attached to this analog pin
#define LED_PIN 13 //LED strand is connected to this pin

#define SAMPLE_WINDOW_TIME 10000 //Sample window for average level //Currently set to 10 Seconds
#define INPUT_FLOOR 10 //Lower range of mic input
#define INPUT_CEILING 30 //Max range of mic input, the lower the value the more sensitive (1023 = max)
#define COUNTING_THRESHOLD 4 //The volume has to be at least this loud before the counters will increment

#define COUNTING_MULTIPLE 1 //The amount of times that the system needs to sample the audio before it will count


//Global Vars
const int NUMBER_OF_PIXELS = ((ZONE_1_PIXEL_QUANTITY) + (ZONE_2_PIXEL_QUANTITY + ZONE_3_PIXEL_QUANTITY));
unsigned int sample;
CRGB strip[NUMBER_OF_PIXELS];
Adafruit_7segment zone1Counter = Adafruit_7segment();
//Adafruit_7segment zone2Counter = Adafruit_7segment();
Adafruit_7segment zone3Counter = Adafruit_7segment();
int zone1CounterQuantity;
//int zone2CounterQuantity;
int zone3CounterQuantity;
int testCounter = 0;
int testCounter1 = 0;
int loopTime = 0;



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
  zone1Counter.begin(0x70);
  //zone2Counter.begin(0x75);
  zone3Counter.begin(0x71);
  zone1CounterQuantity = 0;
  //zone2CounterQuantity = 0;
  zone3CounterQuantity = 0;
}



void loop() {
  Serial.println(testCounter);
  Serial.println(testCounter1);
  unsigned int averageLevel = soundMeasurement();
  unsigned int zone = pixelDisplay(averageLevel);
  if (loopTime >= (COUNTING_MULTIPLE - 1) || zone == 1) {
    counterDisplay(zone);
    loopTime = 0;
  } else {
    loopTime++;
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
  
  unsigned int zone = 0;
  if (c > COUNTING_THRESHOLD) {
    zone = 3;
  }
  if (c > (ZONE_3_PIXEL_QUANTITY)) {
    zone = 2;
  }
  if (c > (ZONE_3_PIXEL_QUANTITY + ZONE_2_PIXEL_QUANTITY)) {
    zone = 1;
  }
  return zone;
}

void counterDisplay(unsigned int zone) {
  switch (zone) {
    case 1:
      zone1CounterQuantity += 1;
      break;
    case 2:
      //zone2CounterQuantity++;
      break;
    case 3:
      zone3CounterQuantity++;
      break;
    default:
      //If it ever hits this, we got something bad going on...
      break;
  }
  if (zone == 1) {testCounter++;}
  if (zone == 3) {testCounter1++;}
    
  zone1Counter.print(testCounter, DEC);
  //zone2Counter.print(zone2CounterQuantity, DEC);
  zone3Counter.print(testCounter1, DEC);
  zone1Counter.writeDisplay();
  //zone2Counter.writeDisplay();
  zone3Counter.writeDisplay();
}
