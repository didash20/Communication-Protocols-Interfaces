////////////////////////////////////////////////////////////////////////////
////                             RS232.C                                ////
////              Change RGB LED Color and toggle relay                 ////
////                                                                    ////
////  This is a sample program to show how to configure and use a       ////
////  serial communication with RS232 in a microcontroller.             ////
////                                                                    ////
////  The program makes use of the RS232 Serial Communicaton to send    ////
////  and recieve data. To use this program, two PCBs with RS232        ////
////  should be used. It is recommended that both have the same         ////
////  programm installed, so one board can control the other one.       ////
////                                                                    ////
////  The purpose of this software is controlling the other board by    ////
////  sending a command when the keypad is pressed. The numbers 0-8     ////
////  will change the other board's RGB LED color, and the 'A' key      ////
////  will toggle the other board's relay between open and closed.      ////
////                                                                    ////
////  When the a key is pressed on the keypad from the other board,     ////
////  this board will change its status depending on the key recieved   ////
////  from the RS232 Serial Communication.                              ////
////                                                                    ////
////  The available commands are:                                       ////
////           OFF    0            BLUE   4           Relay  A          ////
////           RED    1            PURPLE 5                             ////
////           GREEN  2            CYAN   6                             ////
////           YELLOW 3            WHITE  7                             ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#Fuses HS
#use delay( clock = 20M )

// Initialize Serial Communication via RS232
#use rs232( baud=9600 , xmit=pin_c6 , rcv=pin_c7 , parity=N , bits=8 , stop=1 )

// Include drivers
#include <LCD420.c>

// Include Custom Drivers
#include <../Libraries/RGBLED.c>
#include <../Libraries/KP4X4.c>

// Main Code
void main ( void )
{
   // Local Variables declaration
   int8 key = 0;
   
   // Peripherical Configuration
   lcd_init();
   kp_init();
   led_init();
   set_tris_c(0b11111110); // Configure Relay in Port C
   
   // Peripherical default values
   led_off();
   output_low(PIN_C0);  // Closes Relay
   
   // Infinite Loop
   for(;;) {
   
      // Get number from the key pad
      key = kp_getn();
      
      // Send through rs232 when pressed
      if(key != NOKEYPRESS ) {
         putc( key );
      }
      
      // Check if something was recieved from rs232
      if( kbhit() ) {
      
         // Get key from RS232 serial buffer
         key = getc();
         
         // Change LED color depending on recieved key
         if(key < 8) {
            led_setcolor(key);
         }
         // Open/close relay when 10 is recieved
         else if(key == 0xA) {
            output_toggle(PIN_C0);
         }
         
      }
   }
}

