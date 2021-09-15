////////////////////////////////////////////////////////////////////////////
////                           KP4X4_TEST.C                             ////
////                  KEY PAD 4X4 driver test code                      ////
////                                                                    ////
////  Test the functions available in the KP4X4 library.                ////
////                                                                    ////
////  Prints the number and character pressed on the keypad             ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#fuses HS
#use delay( clock = 20M )

// Include peripherical Libraries
#include <LCD420.c>
#include <../Libraries/KP4X4.c>

void main ( void )
{
   //Configuration
   lcd_init();
   kp_init();
   
   // Redefine characters for tag and ast
   kp_def_tag('E');
   kp_def_ast('F');   
   
   //Infinite Loop
   for(;;) {
   
      // Get the values from the keypad
      int8 number = kp_getn(true);       // Get when key down
      char character = kp_getn(false);   // Get when key up
      
      //Print number pressed
      lcd_gotoxy(1,1);
      printf(lcd_putc,"Num:  ");
      if(number != NOKEYPRESS) {
         printf(lcd_putc,"%u",number);
      }
      
      //Print character pressed
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Char: ");
      if(character != NOKEYPRESS) {
         printf(lcd_putc,"%c",character);
      }
      
      // Wait 50 ms
      delay_ms(50);
   }
}
