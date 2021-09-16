////////////////////////////////////////////////////////////////////////////
////                        RTC_COMMUNICATION.C                         ////
////        Common structures and functions for master and slave        ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////                       Structures and enums                         ////
////                                                                    ////
////  struct Date { dow, day, mth, year, dow_str, mth_str }             ////
////                                                                    ////
////  struct Time { dow, day, mth, year, dow_str, mth_str }             ////
////                                                                    ////
////  enum CommunicationComands { SendDate, SendTime, SendAlarm,        ////
////           ReceiveDate, ReceiveTime, ReceiveAlarm, SetRTC }         ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////                             Functions                              ////
////                                                                    ////
////  int1 send_date(Date)    - Sends date to master/slave              ////
////                                                                    ////
////  int1 send_time(Time)    - Sends time to master/slave              ////
////                                                                    ////
////  void recieve_date(Date) - Recieves date from master/slave         ////
////                                                                    ////
////  void recieve_time(Time) - Recieves time from master/slave         ////
////                                                                    ////
////  void set_dow_str(Date)  - Sets the day of the week string         ////
////                                                                    ////
////  void set_mth_str(Date)  - Sets the month string                   ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

/*******      Communication standards      *******/
#Fuses HS
#use delay( clock = 5000000 )
#use rs232( baud=9600 , xmit=pin_c6 , rcv=pin_c7 , parity=N , bits=8 , stop=1 )

/*******        Common Structures          *******/

// Date Structure
typedef struct date{
   unsigned int8 dow;
   unsigned int8 day;
   unsigned int8 mth;
   unsigned int8 year;
   unsigned char dows[4];
   unsigned char mths[4];
} Date;

// Time Structure
typedef struct time{
   unsigned int8 hour;
   unsigned int8 min;
   unsigned int8 sec;
} Time;

/*******            ENUMS             *******/
// Master - Slave Communication Commands
enum CommunicationComands{
   SendDate,
   SendTime,
   SendAlarm,
   ReceiveDate,
   ReceiveTime,
   ReceiveAlarm,
   SetRTC
};

/*******          FUNCTIONS           *******/
// Send Date value to master/slave
// Returns true if communication was successful
int1 send_date(Date date) {
   printf("%c%c%c%c",date.dow,date.day,date.mth,date.year);
   return getc();
}

// Send Time value to master/slave
// Returns true if communication was successful
int1 send_time(Time time) {
   printf("%c%c%c",time.sec,time.min,time.hour);
   return getc();
}

// Recieve Date value from master/slave
void receive_date(Date &date) {
   date.dow = getc();
   date.day = getc();
   date.mth = getc();
   date.year = getc();
   putc('\0');
}

// Recieve Time value between master/slave
void receive_time(Time &time) {
   time.sec = getc();
   time.min = getc();
   time.hour = getc();
   putc('\0');
}

// Set the day of the week string value to date.dows (Spanish)
void set_dow_str(Date &date) {

   switch(date.dow) {
      case 0: strcpy(date.dows,"Dom"); break;
      case 1: strcpy(date.dows,"Lun"); break;
      case 2: strcpy(date.dows,"Mar"); break;
      case 3: strcpy(date.dows,"Mie"); break;
      case 4: strcpy(date.dows,"Jue"); break;
      case 5: strcpy(date.dows,"Vie"); break;
      case 6: strcpy(date.dows,"Sab"); break;
      default: strcpy(date.dows,"   ");
   }
   
}

// Set the month string value to date.mths (Spanish)
void set_mth_str(Date &date) {

   switch(date.mth) {
      case 1: strcpy(date.mths,"Ene"); break;
      case 2: strcpy(date.mths,"Feb"); break;
      case 3: strcpy(date.mths,"Mar"); break;
      case 4: strcpy(date.mths,"Abr"); break;
      case 5: strcpy(date.mths,"May"); break;
      case 6: strcpy(date.mths,"Jun"); break;
      case 7: strcpy(date.mths,"Jul"); break;
      case 8: strcpy(date.mths,"Ago"); break;
      case 9: strcpy(date.mths,"Sep"); break;
      case 10: strcpy(date.mths,"Oct"); break;
      case 11: strcpy(date.mths,"Nov"); break;
      case 12: strcpy(date.mths,"Dic"); break;
      default: strcpy(date.mths,"   ");
   }
   
}

// Compares the values of two time data types and returns:
// time1 > time2   1
// equal           0
// time1 < time2  -1
signed int8 timecmp(struct Time time1,struct Time time2) {
   
   if( time1.hour != time2.hour ){
      return (time1.hour > time2.hour) ? 1 : -1;
   }
   else if( time1.min != time2.min ){
      return (time1.min > time2.min) ? 1 : -1;
   }
   else if( time1.sec != time2.sec ){
      return (time1.sec > time2.sec) ? 1 : -1;
   }

   return 0;
}
