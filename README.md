# clock8
Arduino code to drive an 8 digit, 7 segment LED display that is driven by a pair of chained 74HC595 shift registers.  Includes code which uses direct PORT access to limit the time spent in interrupt routines.  

Rather than using the conventional Arduino IDE to build sketches, I like using the platformio system.
If you don't, you can just load the clock8.ino file into the Arduino IDE.  You'll also need the TimerOne
library to make this work.
