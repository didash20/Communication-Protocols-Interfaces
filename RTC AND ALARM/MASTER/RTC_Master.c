////////////////////////////////////////////////////////////////////////////
////                           RTC_Master.C                             ////
////                  User Interface Microcontroller                    ////
////                                                                    ////
////  This program manages displays the current date and time, and      ////
////  even an alarm. This three variables are displayed on a 4x20       ////
////  LCD and can be adjusted by the user with a 4x4 keypad. The        ////
////  data is stored on a RTC and an EEPROM managed by a slave that     ////
////  communicates through RS232.                                       ////
////                                                                    ////
////  To configure the variables just enter the following key:          ////
////        A - Adjust Alarm        # - Save Changes                    ////
////        C - Adjust Time         * - Cancel Changes                  ////
////        D - Adjust Date                                             ////
////                                                                    ////
////  To move the cursor inside of the adjustable mode:                 ////
////        2 - Increase data       4 - Move cursor left                ////
////        8 - Decrease data       6 - Move cursor right               ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

// Microcontroller used in this project
#include <18F4550.h>

/******* Include RTC COMMUNICATION Library *******/
#include <../RTC_COMMUNICATION.c>

/*******  Include Peripherical Libraries  *******/
#include <LCD420.c>

/*******  Include Custom Libraries  *******/
#include <../../Libraries/KP4X4.c>
#include <../../Libraries/RGBLED.c>

/*******            ENUMS             *******/

// Datetime Changes enum to determine which variable is being changed
enum changes{
   ChangeNone,
   ChangeDate,
   ChangeTime,
   ChangeAlarm,
   SendChanges
};

/*******  Global Variables  *******/
unsigned int16 count = 0;
unsigned int16 count_length = 100;
unsigned char  keypress = 0;

/*******  Timer0 interrupt function  *******/

// Used to make adjusting data blink
#int_timer0
void cursor_blink( void )
{
   count++;
   count %= count_length;
   set_timer0( 220 );
   clear_interrupt( INT_TIMER0 );
}

/*******          FUNCTIONS          *******/
void print_date(Date date);
void print_time(Time time);
void print_alarm(Time alarm);
void switchpos(int8 &cursor_position,int8 &var);
enum changes change_date(Date &date, int8 &cursor_position);
enum changes change_time(Time &time, int8 &cursor_position);
enum changes change_alarm(Time &alarm, int8 &cursor_position);


/*******          MAIN CODE          *******/
void main ( void )
{
   // Adjustable Variable Data Declaration
   Date date; 
   Time alarm;
   Time alarmend;
   Time time; 
   
   // Adjustable Variable Helper Declaration
   int8 cursor_position = 0;     // Determines the current cursor position
   int8 alarmduration = 30;      // Sets the alarm duration (sec)
   enum changes change = ChangeNone;  // Determines which data is adjusted
   
   // Peripherical Initialization
   lcd_init();
   kp_init();
   led_init();
   led_off();
   
   //Timer configuration
   enable_interrupts ( GLOBAL | INT_TIMER0 );
   setup_timer_0( T0_INTERNAL | T0_DIV_256 | T0_8_BIT);

   for(;;) {
      // Saves current pressed key
      keypress = kp_getc();
      
      // Blinking condition when adjusting data
      if(count>=count_length/2-10) {
         // Creates blinking effect in current cursor position
         // when something is being adjusted
         printf(lcd_putc,"   ");
      } 
      else {
         // Print Data
         print_date(date);
         print_time(time);
         print_alarm(alarm);
      }
      
      //Do not update date, time and alarm when sending changes
      if(change != SendChanges) {
      
         // Get RTC Date if it is not being adjusted
         if(change != ChangeDate) {
            putc(SendDate);
            receive_date(date);
         }
         
         // Get RTC Time if it is not being adjusted
         if(change != ChangeTime) {
            putc(SendTime);
            receive_time(time);
            print_time(time);
         } 
         
         // Get alarm from EEPROM if it is not being adjusted
         if(change != ChangeAlarm) {
            putc(SendAlarm);
            receive_time(alarm);
            
            // Update alarm end
            alarmend.hour = alarm.hour;
            alarmend.min = alarm.min;
            alarmend.sec = alarm.sec + alarmduration;
         }
      }
      
      // If key is pressed then change cursor position to 0
      if(keypress != NOKEYPRESS){
            cursor_position = 0; 
      }
      
      // Go to desired change configuration
      switch(keypress) {
         case 'D': change = ChangeDate;  break; //Adjust Date
         case 'C': change = ChangeTime;  break; //Adjust Time
         case 'A': change = ChangeAlarm; break; //Adjust Alarm
         case '#': change = SendChanges; break; // Send changes to slave
         case '*': change = ChangeNone;  break; //Cancel
      }
      
      // If a change is being made adjust the data
      switch(change) {
         
         // Adjusts the date on the device 
         case ChangeDate: 
            change = change_date(date,cursor_position); 
            break;
         
         // Adjusts the time on the device
         case ChangeTime: 
            change = change_time(time,cursor_position); 
            break;
         
         // Adjusts the alarm on the device
         case ChangeAlarm: 
            change = change_alarm(alarm,cursor_position); 
            break;
         
         // Sends the new information to the slave 
         case SendChanges: 
            putc(ReceiveDate);
            Send_date(date);
            putc(ReceiveTime);
            Send_time(time);
            putc(ReceiveAlarm);
            Send_time(alarm);
            putc(SetRTC);

         // Sets the state back to idle
         default: 
            change = ChangeNone; 
            lcd_gotoxy(1,3);
      }
      
      // Comparing times to turn on Alarm (Blinking LED)
      if( !change && timecmp(alarm,time) >= 0 && timecmp(alarmend,time) < 0) {
      
         // LED Blinking condition
         if(count>=0 && count<=count_length/2) {
            led_setcolor(WHITE);
         } else {
            led_off();
         }
      }
      
      // If alarm is not ON maintain LED off
      else {
         led_off();
      }
   }
}

// Prints the date (DOW DAY de MTH de YEAR) on the first line of the LCD
void print_date(Date date) {
   lcd_gotoxy(1,1);
   printf(lcd_putc,"%3s %02u de %3s de %02u ",date.dows,date.day,date.mths,date.year);
}

// Prints the time (HOUR:MIN:SEC) on the second line of the LCD
void print_time(Time time) {
   lcd_gotoxy(1,2);
   printf(lcd_putc,"%02u:%02u:%02u ",time.hour,time.min,time.sec);
}

// Prints the alarm (Alarma: HOUR:MIN:SEC) on the forth line of the LCD
void print_alarm(Time alarm) {
   lcd_gotoxy(1,4);
   printf(lcd_putc,"Alarma: %02u:%02u:%02u ",alarm.hour,alarm.min,alarm.sec);
}

// Changes the current position of the cursor or the variable
// depending on the adjusted data
void switchpos(int8 &cursor_position,int8 &var) {

   switch(keypress) {
      case '2': var++; break;             // Up increases the variable
      case '4': cursor_position--; break; // Left decreases the cursor pos
      case '6': cursor_position++; break; // Right increases the cursor pos
      case '8': var--; break;             // Down decreases the variable
   }
   
}

// Adjust the date by selecting data with a cursor
int8 change_date(Date &date,int8 &cursor_position) {

   switch(cursor_position) {
   
      // Cursor 0 adjusts day of the week
      case 0: 
         lcd_gotoxy(1,1);
         switchpos(cursor_position,date.dow); 
         date.dow = (date.dow + 7) % 7;
         set_dow_str(date);
         break;
      
      // Cursor 1 adjusts day number
      case 1: 
         lcd_gotoxy(4,1);
         switchpos(cursor_position,date.day);
         date.day = (date.day + 31) % 31;
         if(date.day == 0)
            date.day = 31;
         break;
      
      // Cursor 2 adjusts month
      case 2: 
         lcd_gotoxy(11,1);
         switchpos(cursor_position,date.mth);
         date.mth = (date.mth + 12) % 12;
         set_mth_str(date);
         break;
      
      // Cursor 3 adjusts year
      case 3: 
         lcd_gotoxy(18,1);
         switchpos(cursor_position,date.year);
         date.year = (date.year + 100) % 100;
         break;
      
      // Send changes to slave when cursor is out of bounds
      default: 
         cursor_position = 0; 
         return SendChanges;
   }
   
   // Continue adjusting date if not done
   return ChangeDate;
}

// Adjust the time by selecting data with a cursor
int8 change_time(Time &time,int8 &cursor_position) {

   switch(cursor_position) {
   
      // Cursor 0 adjusts hour
      case 0: 
         lcd_gotoxy(1,2);
         switchpos(cursor_position,time.hour); 
         time.hour = (time.hour + 24) % 24;
         break;
         
      // Cursor 1 adjusts minutes
      case 1: 
         lcd_gotoxy(4,2);
         switchpos(cursor_position,time.min); 
         time.min = (time.min + 60) % 60;
         break;
         
      // Cursor 2 adjusts seconds
      case 2: 
         lcd_gotoxy(7,2);
         switchpos(cursor_position,time.sec);
         time.sec = (time.sec + 60) % 60;
         break;
         
      // Send changes to slave when cursor is out of bounds
      default: 
         cursor_position = 0; 
         return SendChanges;
   }
   
   // Continue adjusting time if not done
   return ChangeTime;
}

// Adjust the time by selecting data with a cursor
int8 change_alarm(Time &time,int8 &cursor_position) {

   switch(cursor_position) {
   
      // Cursor 0 adjusts hour
      case 0: 
         lcd_gotoxy(9,4);
         switchpos(cursor_position,time.hour); 
         time.hour = (time.hour + 24) % 24;
         break;
         
      // Cursor 1 adjusts minutes
      case 1: 
         lcd_gotoxy(12,4);
         switchpos(cursor_position,time.min);
         time.min = (time.min + 60) % 60;
         break;
         
      // Cursor 2 adjusts seconds
      case 2: 
         lcd_gotoxy(15,4);
         switchpos(cursor_position,time.sec);
         time.sec = (time.sec + 60) % 60;
         break;
      
      // Send changes to slave when cursor is out of bounds
      default: 
         cursor_position = 0; 
         return SendChanges;
   }
   
   // Continue adjusting alarm if not done
   return ChangeAlarm;
}
