# loud-o-meter
An Arduino controlled vu-meter used for measuring the volume of a school lunchroom.  If you have anyquestions or issues, please feel free submit an issue.

> Created by Matt Ricks

## Purpose
Requested by James P., a teacher, this project was inspired by [this instructable](https://www.instructables.com/id/VU-Meter-LED-Noise-o-Meter-for-Classrooms/).    It's purpose is to measure the volume levels during lunchtime at school, and give a number representation of how loud the students were during their lunch time.  There are some children who are sensitive to loud noises, and are unable to each lunch in such a loud environment.  To help those students, this device is used to reward the student body as a whole based on their quite volume level.  Additionally, this may be used in assemblies as a method to encourage cheering, or quiter moments.

### Hardware requirements:
  [Most Arduino or Arduino-compatible boards (ATmega 328P or better).](https://www.amazon.com/Elegoo-EL-CB-003-ATmega2560-ATMEGA16U2-Arduino/dp/B01H4ZLZLQ/)
  [Any sort of RGB light strand that FastLED supports](https://www.adafruit.com/product/1376?length=1)
  [Adafruit 1.2" 4-Digit 7-Segment Display w/I2C Backpack - Green](https://www.adafruit.com/product/1268)
  [Adafruit 1.2" 4-Digit 7-Segment Display w/I2C Backpack - Red](https://www.adafruit.com/product/1270)
  [1000 μF, 6.3V or higher](https://www.newark.com/panasonic-electronic-components/eca-0jhg102/aluminum-electrolytic-capacitor/dp/96K9139?st=1000%20μF%20%206.3V%20capacitor)
  [Electret Microphone Amplifier - MAX4466 with Adjustable Gain](https://www.adafruit.com/product/1063)

### Software requirements:
  Adafruit_NeoPixel
  FastLED
  Adafruit_GFX
  Adafruit_LEDBackpack

## Connections:
### Microphone
  3.3V to mic amp +
  GND to mic amp -
  Analog pin 9 (A9) to microphone output
### Arduino
  3.3V to AREF
  Vin to 5v power supply +
  GND to power supply -
### Option Button
  Button + to pin 4
  Button - to GND
### LED Strip
  5v to LED Strip +
  GND to LED Strip -
  Digital pin 6 (6) to LED data input
  Bridge + and - of Neopixels with the Capacitor
### 7 Segment Displays
  D -> SDA
  C -> SCL
  + -> 5v
  - -> GND
  IO -> jumper to + for 5v
### Additional Wiring Help
  https://learn.adafruit.com/assets/2326
  https://learn.adafruit.com/adafruit-led-backpack/1-2-inch-7-segment-backpack
  https://learn.adafruit.com/adafruit-led-backpack/connecting-multiple-backpacks
  https://cdn-learn.adafruit.com/downloads/pdf/adafruit-neopixel-uberguide.pdf
  See Code

## Code Used:
  https://www.hackster.io/wannaduino/vu-meter-on-steroids-arduino-nano-and-neopixel-ws2812b-7f78e1

## Adjustments:
Be sure to count the number of leds you have on your led strip, and adjust these settings in the code to match your setup:
  "#define N_PIXELS"
  "#define LED_TYPE WS2812B"
  "#define COLOR_ORDER RGB"
  "#define BRIGHTNESS 100"
After everything is hooked up and installed, it's recommended that the vu meter be calibrated.  This can be acheived by adjusting the gain on the back of the mic, and by adjusting any of the following values in the code:
  "#define NOISE"
  "#define SAMPLES"
  "#define COUNT_SPEED"