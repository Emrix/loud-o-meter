# loud-o-meter
An Arduino controlled vu-meter used for measuring the volume of a school lunchroom

> Created by Matt Ricks

## Purpose
Requested by James P., a teacher, this project was inspired by [this instructable](https://www.instructables.com/id/VU-Meter-LED-Noise-o-Meter-for-Classrooms/).    It's purpose is to measure the volume levels during lunchtime at school in order to reward the students based on their volume level.

Hardware requirements:
  Most Arduino or Arduino-compatible boards (ATmega 328P or better).
  Any sort of RGB light strand that FastLED supports
Software requirements:
  FastLED

Connections:
  3.3V to mic amp +
  GND to mic amp -
  Analog pin 0 (A0) to microphone output
  Digital pin 13 (13) to LED data input
  3.3V to AREF

fscale function:
Floating Point Autoscale Function V0.1
Written by Paul Badger 2007
Modified from code by Greg Shakar


Hardware:
https://www.adafruit.com/product/1268
https://www.adafruit.com/product/1270
https://www.amazon.com/Elegoo-EL-CB-003-ATmega2560-ATMEGA16U2-Arduino/dp/B01H4ZLZLQ/