////////////////////////////////////////////////////////////////////////////
////                         BinaryCounter.C                            ////
////             Counts in binary and outputs the number                ////
////                                                                    ////
////  This program makes use of the Port B to output a 4 bit number     ////
////  which increases until a certain limit and then decreases once     ////
////  more to 0 infinitely.                                             ////
////                                                                    ////
////  This is a sample program to show how to configure and use the     ////
////  digital outputs from the microcontroller.                         ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#fuses INTRC_IO
#use delay( clock = 4M )

int8 i;

void countUntil (int8 lim);

void main( void )
{
   //Configuration
   set_tris_b(0xF0);    // Set the last 4 bits as outputs
   
   //Endless Loop
   for(;;){
      countUntil(10);
   }
}

/* Counts up and down in binary from 0 to lim 
every 100 ms and outputs the number on port B*/
void countUntil(int8 lim)
{
   // Count up
   for(i=0; i<lim ; i++){
      output_b(i);   // Output i to the Port B
      delay_ms(100); // Wait 100 ms
   }
   
   // Count Down
   for(i-=2; i>0 ; i--){
      output_b(i);
      delay_ms(100);
   }
}
