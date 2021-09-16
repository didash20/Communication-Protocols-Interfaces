# TMR0.C
Make a RGB LED blink every second

This is a sample program to show how to configure and use a
timer in a microcontroller.

The program makes use of timer 0 to count how much time has
elapsed since the microcontroller is turned on.

Two things happen in the program:
* The elapsed time is displayed on a 4x20 LCD
* An RGB LED blinks every second with different colors
  * Green - 1 second
  * Blue  - 10 seconds
  * Red   - 1 minute
