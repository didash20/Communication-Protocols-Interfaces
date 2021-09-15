////////////////////////////////////////////////////////////////////////////
////                             LOCK.C                                 ////
////               Create a password lock with a relay                  ////
////                                                                    ////
////  This program makes use of a relay, a 4x4 keypad and a 4x20 LCD    ////
////  Display to simulate a lock. To unlock the relay the user must     ////
////  enter a unique password into the device by using the keypad.      ////
////  After a certain amount of tries, the device will be blocked       ////
////  for a couple of seconds. The user has a limited number of tries   ////
////  before the lock stays locked indefinitely.                        ////
////                                                                    ////
////  The first time the program runs in the device, it will ask the    ////
////  user to enter a 20 character long password. After this, the       ////
////  password wont be able to change any more.                         ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#include <18F4550.h>
#Fuses HS
#use delay( clock = 20M )

// Include Peripherical Drivers
#include <LCD420.c>
#include <stdlib.h>

// Include Custom Drivers
#include <../Libraries/RGBLED.c>
#include <../Libraries/KP4X4.c>

// Function Declaration
void display_title( void );
void enter_password(char pass[21]);
void save_password(char pass[21]);
void get_password(char pass[21]);
signed int8 strcmp(unsigned char *s1, unsigned char *s2);

// Main Code
void main ( void )
{
   // Local Variable Declaration
   char saved_pass[21] = {'\0'};
   char pass[21] = {'\0'};
   
   int8 try = 0;  // Number of tries
   int8 seg = 0;  // Seconds to wait if incorrect password is entered
   int1 done = 0; // Finishes process after many attempts 
   
   // Peripherical Configuration
   lcd_init();
   led_init();
   kp_init();
   set_tris_c(0b11111110); // Configure Relay in Port C
   srand(get_rtcc());      // Seed for random numbers
   
   // Peripherical default values
   led_off();
   kp_def_ast('\b');    // Change '*' to delete 
   kp_def_tag('\0');    // Change '#' to empty
   output_low(PIN_C0);  // Close Relay
      
   // Display title for half second
   led_setcolor(BLUE);
   display_title();
   delay_ms(500);
   
   // Clear LCD
   lcd_putc('\f');
   
   // Enter Password if EEPROM is empty
   if( read_eeprom(0x00) == 0xFF ) {
   
      led_setcolor(PURPLE);
      printf(lcd_putc,"\nCreate New Password:\n");
      enter_password(saved_pass);
      save_password(saved_pass);
      
      // Display Complete Saved Password for half a second
      printf(lcd_putc,"\f\nSaved Password:\n");
      printf(lcd_putc,"%s",saved_pass);
      delay_ms(500);
      
   }
   // Get previously saved password from EEPROM 
   else {
      get_password(saved_pass);
   }
   
   
   //Main Loop
   while(!done)
   {
      // User tries to enter password
      led_setcolor(CYAN);
      enter_password(pass);
      try++;
            
      // Simulate validating process
      printf(lcd_putc,"\f\n   Validating");
      delay_ms(200);
      for( int8 i=0 ; i<5 ; i++) {
         lcd_putc('.');
         delay_ms(200);
      }
      
      // Invalid Password
      if(strcmp(saved_pass,pass) != 0) {
      
         // Display Incorrect Password Message
         led_setcolor(RED);
         printf(lcd_putc,"\f\nInvalid Password\n");
         delay_ms(500);
         
         // Set for how long the user has to wait to try again
         switch(try) {
            case 3: seg = 30; break;
            case 5: seg = 60; break;
            case 10: seg = 120; break;
            
            case 15: 
               done = 1; 
               printf(lcd_putc,"Blocked");
               seg = rand() % 180 + 120;
               break;
               
            default: seg = 0;
         }
         
         // Wait until time has elapsed
         while(seg != 0) {
            printf(lcd_putc,"Try again in %3us",seg);
            delay_ms(1000);
            seg--;
         }
      }
      // Correct Password
      else {
         // Display Correct Password Message
         led_setcolor(GREEN);
         printf(lcd_putc,"\f\nCorrect Password");
         
         // Open Relay until key is pressed
         output_high(PIN_C0);
         while(kp_getc() != NOKEYPRESS);
         output_low(PIN_C0);
         
         // Reset number of tries
         try=0;
      } 
   }
}


// Functions

// Display title on LCD
void display_title( void ){
   lcd_gotoxy(1,1);
   printf(lcd_putc,"********************\n");
   printf(lcd_putc,"*  Dielectronic's  *\n");
   printf(lcd_putc,"*                  *\n");
   printf(lcd_putc,"********************\n");
}

// Ask user for password
void enter_password(char pass[21]) {
   
   // Local Variable Declaration
   char key = NOKEYPRESS;

   // Enter Password
   for ( int8 i = 0; i < 20 && key != '\0'; i++){
   
      // Get Key from keypad
      do key = kp_getc(); while(key == NOKEYPRESS);
      
      // Clear password if delete is pressed
      if( key == '\b') {
         lcd_gotoxy(1,3);
         printf(lcd_putc,"                    ");
         lcd_gotoxy(1,3);
         i = -1;
      }
      // Display and save key
      else {
         lcd_putc(key);
         pass[i] = key;
      }
   }
}

// Save the password to the EEPROM
void save_password(char pass[21]) {

   for(int8 i = 0; pass[i] != '\0'; i++){
      write_eeprom(0x00+i,pass[i]);
   }

}

// Get the password from the EEPROM
void get_password(char pass[21]) {

   for( int8 i=0 ; pass[i] != '\0'; i++) {
      pass[i] = read_eeprom(0x00+i);
   }
   
}
