////////////////////////////////////////////////////////////////////////////
////                          RGBLED_TEST.C                             ////
////                     RGB LED driver test code                       ////
////                                                                    ////
////  Test the functions available for the RGBLED library               ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#fuses HS
#use delay( clock = 20M )

#include <LCD420.c>
#include <../Libraries/RGBLED.c>

void main( void )
{
   //Configuration
   lcd_init();
   led_init();
   
   //Infinite Loop
   for(;;)
   {
      led_setcolor(PURPLE);
      printf(lcd_putc,"%d",led_getcolrn());
   }
}
