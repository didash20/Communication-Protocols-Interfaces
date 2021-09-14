////////////////////////////////////////////////////////////////////////////
////                            DS1307.C                                ////
////                 Driver for DS1307 Real Time Clock                  ////
////                                                                    ////
//// rtc_init() - Enable oscillator without clearing the seconds        ////
////              register - used when PIC loses power and DS1307       ////
////              run from 3V BAT                                       ////
////            - Disable squarewave output                             ////
////                                                                    ////
//// rtc_set_date_time(date,time)   - Set the date/time                 ////
////                                                                    ////
//// rtc_get_date(date)             - Get the date                      ////
////                                                                    ////
//// rtc_get_time(time)             - Get the time                      ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

#ifndef RTC_SDA

#define RTC_SDA PIN_B0
#define RTC_SCL PIN_B1

#endif

#use i2c(master, sda=RTC_SDA, scl=RTC_SCL, SLOW)

// Internal Functions
BYTE bin2bcd(BYTE binary_value);
BYTE bcd2bin(BYTE bcd_value);

void rtc_init(void)
{
   BYTE seconds = 0;

   i2c_start();
   i2c_write(0xD0);      // SELECCIONA RTC CON  ORDEN WR
   i2c_write(0x00);      // REG 0
   i2c_start();
   i2c_write(0xD1);      // RD from RTC
   seconds = bcd2bin(i2c_read(0)); // Read current "seconds" in DS1307
   i2c_stop();
   seconds = bin2bcd(seconds & 0x7F);

   delay_us(3);

   i2c_start();
   i2c_write(0xD0);      // WR to RTC
   i2c_write(0x00);      // REG 0
   i2c_write(seconds);     // Start oscillator with current "seconds value
   i2c_start();
   i2c_write(0xD0);      // WR to RTC
   i2c_write(0x07);      // Control Register
   i2c_write(0x10);     // Enable squarewave output pin
   i2c_stop();

}

void rtc_OUT(void){
   i2c_start();
   i2c_write(0xD0);      // WR to RTC
   i2c_write(0x07);      // WR to RTC
   i2c_write(0x10);     // Enable squarewave output pin
   i2c_stop();
}

void rtc_set_date_time(Date date, Time time)
{
  time.sec &= 0x7F;
  time.hour &= 0x3F;

  i2c_start();
  i2c_write(0xD0);            // I2C write address
  i2c_write(0x00);            // Start at REG 0 - Seconds
  i2c_write(bin2bcd(time.sec));      // REG 0
  i2c_write(bin2bcd(time.min));      // REG 1
  i2c_write(bin2bcd(time.hour));      // REG 2
  i2c_write(bin2bcd(date.dow));      // REG 3
  i2c_write(bin2bcd(date.day));      // REG 4
  i2c_write(bin2bcd(date.mth));      // REG 5
  i2c_write(bin2bcd(date.year));      // REG 6
  //i2c_write(0x80);            // REG 7 - Disable squarewave output pin
   i2c_write(0x10);     // Enable squarewave output pin
  i2c_stop();
}

void rtc_get_date(Date &date)
{
  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x03);            // Start at REG 3 - Day of week
  i2c_start();
  i2c_write(0xD1);
  date.dow  = bcd2bin(i2c_read() & 0x7f);   // REG 3
  date.day  = bcd2bin(i2c_read() & 0x3f);   // REG 4
  date.mth  = bcd2bin(i2c_read() & 0x1f);   // REG 5
  date.year = bcd2bin(i2c_read(0));         // REG 6
  i2c_stop();
  
  set_mth_str(date);
  set_dow_str(date);
}

void rtc_get_time(Time &time)
{
  i2c_start();
  i2c_write(0xD0);
  i2c_write(0x00);            // Start at REG 0 - Seconds
  i2c_start();
  i2c_write(0xD1);
  time.sec = bcd2bin(i2c_read() & 0x7f);
  time.min = bcd2bin(i2c_read() & 0x7f);
  time.hour  = bcd2bin(i2c_read(0) & 0x3f);
  i2c_stop();

}

BYTE bin2bcd(BYTE binary_value)
{
  BYTE temp;
  BYTE retval;

  temp = binary_value;
  retval = 0;

  while(temp>0)
  {
    // Get the tens digit by doing multiple subtraction
    // of 10 from the binary value.
    if(temp >= 10)
    {
      temp -= 10;
      retval += 0x10;
    }
    else // Get the ones digit by adding the remainder.
    {
      retval += temp;
      temp = 0;
    }
  }

  return(retval);
}


// Input range - 00 to 99.
BYTE bcd2bin(BYTE bcd_value)
{
  BYTE temp;

  temp = bcd_value;
  // Shifting upper digit right by 1 is same as multiplying by 8.
  temp >>= 1;
  // Isolate the bits for the upper digit.
  temp &= 0x78;

  // Now return: (Tens * 8) + (Tens * 2) + Ones

  return(temp + (temp >> 2) + (bcd_value & 0x0f));
}
