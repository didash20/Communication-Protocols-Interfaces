////////////////////////////////////////////////////////////////////////////
////                            POS_Slave.C                             ////
////                    2404 Handler Microcontroller                    ////
////                                                                    ////
////  This program is used to handle the 2404 EEPROM IC with I2C        ////
////  communication. And using RS232 Serial Communication to send       ////
////  data stored on the periphericals to the master.                   ////
////                                                                    ////
////  The board with this microcontroller also includes a LCD that      ////
////  displays product information and messages from the master.        ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

// Microcontroller used in this project
#include <18F4550.h>

/******* Include POS COMMUNICATION Library *******/
#include <../POS_COMMUNICATION.c>

/*******         Define I2C PINS           *******/
#define RTC_SDA  PIN_B0
#define RTC_SCL  PIN_B1
#define EEPROM_SDA  PIN_B0
#define EEPROM_SCL  PIN_B1

/*******  Include Peripherical Libraries  *******/
#include <LCD.c>
#include <../../Libraries/2404.c>

/*******   Save Default Products in ROM    *******/
#ROM int8 getenv("EEPROM_ADDRESS") = { 
   10, // Fist value stores number of items
   "000000","APPLE     ",0,38,
   "000001","ORANGE    ",0,19,
   "000002","LEMON     ",0,42,
   "000003","STRAWBERRY",0,59,
   "000004","RASPBERRY ",0,29,
   "000005","MANGO     ",0,32,
   "000006","BANANA    ",0,19,
   "000007","WATERMELON",0,15,
   "000008","MELON     ",0,26,
   "000009","AVOCADO   ",0,69
}


/*******          FUNCTIONS          *******/
void load_products( void );
Product read_Product( int8 num );
int1 save_product( Product prod );
void print_product( Product prod );

/*******          MAIN CODE          *******/
void main( void )
{
   // Local Variable Declaration
   Product prod;
   char message[32];
   
   // Peripherical Initialization
   lcd_init();
   init_ext_eeprom();
   
   // Empty serial buffer
   while(kbhit()) {
      getc();
   }
   
   // Reload default items if External EEPROM is empty
   if( read_ext_eeprom(0x00) == 0xFF ) {
      load_products();   
   }
   
   // Read the first product from the external EEPROM
   prod = read_Product(0);

   // Endless Loop
   for(;;) {
   
      // Received command from Master
      if(kbhit()) {
      
         // Command Processes
         switch(getc()) {
         
            // Return number of products to master
            case ProdNum: 
               putc(read_ext_eeprom(0x00)); 
               break;
            
            // Return the specified product to master
            case SendProd: 
               prod = read_Product(getc());
               send_Product(prod);
               break;
               
            // Receive and display product on LCD
            case ReceiveProd: 
               if(receive_Product(prod)) {
                  print_product(prod);
               }
               break;
            
            // Receive and save product on EEPROM
            case SaveProd: 
               if(receive_Product(prod)){
                  save_Product(prod);
                  print_product(prod);
               }
               break;
               
            // Print specified message on LCD
            case PrintMessage: 
               gets(message);
               printf(lcd_putc,"\f%s",message);
               break;
               
            // Clear LCD
            case ClearScreen: 
               lcd_putc('\f'); 
               break;
               
            default:
         }
      }
   }
}

// Read products from ROM and save them on EEPROM
void load_products( void ){

   // Declare Local Variables
   Product prod;
   int8 i;

   // First value establishes number of items available
   write_ext_eeprom(0x00,0);
   
   // Add 10 predefined values to external EEPROM
   for(int8 num=0; num<10; num++) {
      
      // Read product SKU from ROM
      for(i = 0; i<7; i++) {
         prod.sku[i] = read_eeprom(0x01+i+num*20);
      }
      
      // Read product name from ROM
      for(i = 0; i<11; i++) {
         prod.name[i] = read_eeprom(0x08+i+num*20);
      }
         
      // Read Product price from ROM
      prod.price = (read_eeprom(0x13+num*20)<<8) 
                      + read_eeprom(0x14+num*20);
                 
      // Save product to external EEPROM
      save_Product(prod);
   }
}

// Read and return product from external EEPROM
Product read_product( int8 num ) {
   
   // Declare Local Variables
   Product prod;
   int8 i;
   
   // Read product SKU from EEPROM
   for(i=0; i<7; i++) {
      prod.sku[i] = read_ext_eeprom(0x01+i+num*20);
   }
      
   // Read product name from EEPROM
   for(i=0; i<11; i++) {
      prod.name[i] = read_ext_eeprom(0x08+i+num*20);
   }
      
   // Read Product price from EEPROM
   prod.price = (read_ext_eeprom(0x13+num*20)<<8) 
                   + read_ext_eeprom(0x14+num*20);
   
   // Return obtained product
   return prod;
}

// Save product on external EEPROM
int1 save_product( Product prod ) {

   // Get item number from EEPROM
   int8 max = read_ext_eeprom(0x00);
   int8 i;
   
   // Save product SKU on EEPROM
   for(i=0; i<7; i++) {
      write_ext_eeprom(0x01+i+max*20,prod.sku[i]);
   }
      
   // Save product name on EEPROM
   for(i=0; i<11; i++) {
      write_ext_eeprom(0x08+i+max*20,prod.name[i]);
   }
      
   // Save product price on EEPROM
   write_ext_eeprom(0x13+max*20,prod.price>>8); 
   write_ext_eeprom(0x14+max*20,prod.price);
   
   // Increase number of saved items 
   write_ext_eeprom(0x00,max+1);
   
   // Saved Successfully
   return true;
}

// Display product information on LCD
void print_product(Product prod) {
   printf(lcd_putc,"\f%s %s\n$ %04lu",prod.sku,prod.name,prod.price);
}
