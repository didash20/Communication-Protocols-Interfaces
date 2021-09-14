////////////////////////////////////////////////////////////////////////////
////                             LCD_TEST.C                             ////
////                      LCD420 driver test code                       ////
////                                                                    ////
////  Test the functions available in the LCD420 library.               ////
////                                                                    ////
////  Move a string along a LCD.                                        ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18f4550.h>
#fuses HS
#use delay( clock = 20M )

// Include LCD Library
#include <LCD420.c>

// LCD defintions
#define LCD_WIDTH 20
#define LCD_HEIGHT 4

// Main Code
void main ( void )
{
   // LCD Configuration
   lcd_init();
   lcd_gotoxy(1,1);
   
   // Declare local Variables
   int8 pos = 0;
   int8 x = 1;
   int8 y = 1;
   char* str = "Diego\0";
   for(int8 strl = 0; *(str+strl)!='\0'; strl++);  // Get string lenght
   
   //Infinite Loop
   for( ; ; )
   {   
      // Print string character found in pos
      lcd_putc(*(str+pos));
            
      // Restart in next position when end of string is reached
      if( pos == strl ){
      
         // Waits 100 ms before printing string again
         delay_ms(100);
      
         // Restart string position
         pos = 0;
         
         //Determine new place where the string will start
         x++;
         if( x > LCD_WIDTH ) {
            x = 1;
            y = (y+1) % LCD_HEIGHT + 1; // Go to next line
         }
         
         // Go to the start of the string
         lcd_gotoxy(x,y);
         printf(lcd_putc,"\b "); // Delete previous character
         
      }
      // Go to next line if the string exceeds lcd width
      else if( (x+pos) >= LCD_WIDTH ){
         lcd_putc('\n');
      }
   }
}
