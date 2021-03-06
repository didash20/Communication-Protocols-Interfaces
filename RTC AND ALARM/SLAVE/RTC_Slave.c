////////////////////////////////////////////////////////////////////////////
////                            RTC_Slave.C                             ////
////              DS1307 and 2404 Handler Microcontroller               ////
////                                                                    ////
////  This program is used to handle the DS1307 RTC IC and the 2404     ////
////  EEPROM IC with I2C communication. And communicating by RS232      ////
////  with a master to send the data stored on the periphericals.       ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

// Microcontroller used in this project
#include <18F4550.h>

/******* Include RTC COMMUNICATION Library *******/
#include <../RTC_COMMUNICATION.c>

/*******         Define I2C PINS           *******/
#define RTC_SDA  PIN_B0
#define RTC_SCL  PIN_B1
#define EEPROM_SDA  PIN_B0
#define EEPROM_SCL  PIN_B1

/*******  Include Custom Libraries  *******/
#include <../../Libraries/DS1307.c>
#include <../../Libraries/2404.c>

/*******          FUNCTIONS          *******/
void get_alarm(Time &alarm);
void set_alarm(Time alarm);

/*******          MAIN CODE          *******/
void main(void)
{
   // Adjustable Variable Data Declaration
   Date date; 
   Time alarm;
   Time time;
   
   // Peripherical Initialization
   rtc_init();
   init_ext_eeprom();
   
   // Endless Loop
   for(;;) {
   
      // If the microcontroller receives something through RS232
      if(kbhit()) {
      
         // Command Processes
         switch(getc()) {
         
            // Return the current date to the master
            case SendDate: 
               rtc_get_date(date);
               send_date(date);  
               break;
               
            // Return the current time to the master
            case SendTime: 
               rtc_get_time(time);
               send_time(time); 
               break;
               
            // Return the saved alarm to the master
            case SendAlarm: 
               get_alarm(alarm);
               send_time(alarm); 
               break;
               
            // Get the date from the master
            case ReceiveDate: 
               receive_date(date); 
               break;
               
            // Get the time from the master
            case ReceiveTime: 
               receive_time(time); 
               break;
               
            // Get the alarm from the master and save it
            case ReceiveAlarm:
               receive_time(alarm); 
               set_alarm(alarm); 
               break;
               
            // Save the date and time to the RTC
            case SetRTC: 
               rtc_set_date_time(date,time); 
               break;
         }
      }
   }
}

// Retrieve the alarm from the eeprom
void get_alarm(Time &alarm) {
   
   // If the external eeprom is empty then asign the default alarm to 8:00
   if(read_ext_eeprom(0x00) == 0xFF) {
      write_ext_eeprom(0x00,8);  // alarm hour
      write_ext_eeprom(0x01,0);   // alarm min
      write_ext_eeprom(0x02,0); // alarm sec
   }
   
   // Read the alarm values from the eeprom
   alarm.hour = read_ext_eeprom(0x00);
   alarm.min = read_ext_eeprom(0x01);
   alarm.sec = read_ext_eeprom(0x02);
   
}

// Set the alarm to the eeprom
void set_alarm(Time alarm) {
   write_ext_eeprom(0x00,alarm.hour);
   write_ext_eeprom(0x01,alarm.min);
   write_ext_eeprom(0x02,alarm.sec);
}
