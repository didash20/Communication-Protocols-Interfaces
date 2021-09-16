////////////////////////////////////////////////////////////////////////////
////                              TMR0.C                                ////
////                Make a RGB LED blink every second                   ////
////                                                                    ////
////  This is a sample program to show how to configure and use a       ////
////  timer in a microcontroller.                                       ////
////                                                                    ////
////  The program makes use of timer 0 to count how much time has       ////
////  elapsed since the microcontroller is turned on.                   ////
////                                                                    ////
////  Two things happen in the program:                                 ////
////  * The elapsed time is displayed on a 4x20 LCD                     ////
////  * An RGB LED blinks every second with different colors            ////
////        * Green - 1 second                                          ////
////        * Blue  - 10 seconds                                        ////
////        * Red   - 1 minute                                          ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#fuses HS
#use delay ( clock = 5M )

//Include drivers
#include <LCD420.c>
#include <../Libraries/RGBLED.c>

//Global variables
unsigned int16 count = 0;
unsigned int8 min = 0;
const int8 time = 206;

//Timer0 interrupt function 
#int_timer0
void prendido_led( void )
{
   count++;
   
   // One minute count
   if(count%6000==0) {
      led_setcolor(RED);   // Blink Red
      count = 0;           // Restart cont
      min++;               // Increase minutes
      if(min==60)
         min=0;            // Restart minutes every hour
   }
   
   // Ten seconds count
   else if(count%1000==0){
      led_setcolor(BLUE);  // Blink blue
   }
   
   // One second count
   else if(count%100==0) {
      led_setcolor(GREEN); // Blink green
   }
   
   // 30 ms after every second
   // * makes LED blink
   else if(count%100==30) {
      led_off();           // Turn the LED off
   }
   
   set_timer0( time );
   clear_interrupt( INT_TIMER0 );
}


void main (void)
{
   //variable declaration
   
   //Driver configuration
   lcd_init();
   led_init();
   
   //Timer configuration
   enable_interrupts ( GLOBAL | INT_TIMER0 );
   setup_timer_0( T0_INTERNAL | T0_DIV_256 | T0_8_BIT);
   
   //infinite loop
   for(;;)
   {
      // Display the time that has passed since the start
      lcd_gotoxy(2,2);
      printf(lcd_putc,"Cont: %2u m %2lu.%2lu s",min,count/100,count%100);
   }
}
