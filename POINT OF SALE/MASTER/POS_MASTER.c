////////////////////////////////////////////////////////////////////////////
////                           POS_Master.C                             ////
////                 Seller Interface Microcontroller                   ////
////                                                                    ////
////  This program controls the interaction of the seller with the      ////
////  device. The seller can either add a new product to the database   ////
////  or select the sale mode, which searches through products and      ////
////  adds up prices to calculate the total amount a client need to     ////
////  pay, once the products are selected, the seller enters the        ////
////  amount, and the device calculates the change to be provided.      ////
////                                                                    ////
////  The seller can interact with the device with the 4x4 keypad.      ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

// Microcontroller used in this project
#include <18F4550.h>

/******* Include POS COMMUNICATION Library *******/
#include <../POS_COMMUNICATION.c>

/*******  Include Peripherical Libraries  *******/
#include <LCD420.c>

/*******  Include Custom Libraries  *******/
#include <../../Libraries/KP4X4.c>
#include <../../Libraries/RGBLED.c>

/*******  Global Variables  *******/
int1 render = false;
int1 blink = false;
unsigned int8 rendercont = 0;

/*******  Timer0 interrupt function  *******/
// Renders screen
#int_timer0
void rendering( void )
{
   rendercont++;
   if(rendercont > 3){
      rendercont = 0;
      render = true;
   }
   set_timer0( 200 );
   clear_interrupt( INT_TIMER0 );
}

/*******  Timer0 interrupt function  *******/
// Makes adjusting data blink
#int_timer1
void cursor_blink( void )
{
   blink = (blink + 1) % 2;
   set_timer1( 220 );
   clear_interrupt( INT_TIMER1 );
}

/*******          FUNCTIONS          *******/
Product createNewProduct( void );
Product set_Product(char* sku,char* name,int16 price);
int1  check_Product( Product prod );
int1  save_Product( Product prod );
int16 selectProducts( void );
int1  payProducts( int16 );
void  printProd( Product prod );

/*******          MAIN CODE          *******/
void main( void )
{
   // Local Variable Declaration
   unsigned int8 key = 0;
   unsigned int8 position = 0;
   unsigned int16 total = 0;
   Product prod; 
   
   //Peripherical Initialization
   lcd_init();
   kp_init();
   
   //Timer configuration
   enable_interrupts ( GLOBAL );
   enable_interrupts ( INT_TIMER0 );
   enable_interrupts ( INT_TIMER1 );
   setup_timer_0( T0_INTERNAL | T0_DIV_256 | T0_8_BIT);
   setup_timer_1( T1_INTERNAL | T1_DIV_BY_8 );
   
   //Empty serial buffer
   while(kbhit())
      getc();   
   
   // Endless Loop
   for(;;) {
   
      // Only display messages when render is true    
      if(render) {
         // Display Menu selection 
         printf(lcd_putc,"\f Electro-FruitStore\n");
         printf(lcd_putc,"1. New Product\n");
         printf(lcd_putc,"2. Purchase Mode");
         
         // Add Arrow to current option
         lcd_gotoxy(18,position+2);
         printf(lcd_putc,"<-");
         
         // Reset renderization
         render = false;
      }
      
      // Get key from key pad
      key = kp_getn(); 
      
      // If entered key is '#' or '*' change key to selected option
      if( key == 0x0D || key == 0x0E) {
         key = position+1;
      }
      
      // 
      switch(key) {
         case NOKEYPRESS: break;
         
         // Change selected position up
         case 0x0A: position = (position + 1) % 2; break;
         
         // Change selected position down
         case 0x0B: position = (position + 3) % 2; break;
         
         // Register new product
         case 0x01:  
            
            // Assign created product to variable
            prod = createNewProduct();
            
            // Check if new product is valid before saving it
            if( prod.price < 10000 ) { 
            
               // Check if new product is valid before saving it
               key = check_Product(prod);
               
               // Print message depending of checking output
               lcd_putc('\f'); lcd_gotoxy(4,2);
               switch(key) {
                  
                  // Save product into database if validation was successful
                  case Valid:
                     putc(SaveProd);
                     if(send_Product(prod)){
                        printf(lcd_putc,"Successful Save");
                     } else {
                        printf(lcd_putc,"The Save Failed");
                     }
                     break;
                     
                  case InvalidPrice:   printf(lcd_putc,"Invalid  Price"); break;
                  case InvalidSKU:     printf(lcd_putc," Existing SKU "); break;
                  case InvalidName:    printf(lcd_putc,"Existing  Name"); break;
                  default :            printf(lcd_putc," Check Failed "); break;
               }
            }
            // Product creation was canceled
            else {
               lcd_putc('\f'); lcd_gotoxy(4,2);
               printf(lcd_putc,"    Cancel    ");
            }
            // Display message for 2 seconds before returning to menu
            delay_ms(2000); 
            break;
                 
         // Purchase Mode
         case 0x02:  
         
            // Make a list of products and return the total
            total = selectProducts();
            
            // Only pay products if the total was greater than 0
            key = (total>0)? payProducts(total):false;
            
            lcd_putc('\f'); lcd_gotoxy(1,2);
            if(key) {
               printf(lcd_putc,"     Successful\n    Transaction");
            } else {
               printf(lcd_putc,"      Canceled");
            }
            
            // Display Come Back Soon in other screen 
            putc(PrintMessage);
            printf("Come Back Soon%c%c",0,13);
            
            // Clear screen after 2 seconds
            delay_ms(2000);
            putc(ClearScreen);
            break;
      }
      
   }
}


// Changes screen for Seller to create new product
Product createNewProduct() {

   //Variable declaration
   unsigned int8 key = 0;
   unsigned int16 letter = 'A';
   unsigned int8 position = 0;
   unsigned int8 attribute = 1;
   Product prod;
   
   // Ask database for total products
   putc(ProdNum);
   key = getc();
   
   // Clear Screen
   lcd_putc('\f');
   
   // Set default SKU number to Total of Products
   for(int8 i=5; i < 6; i--){
      prod.sku[i] = (key % 10) + 48;
      key /= 10;
   }
   prod.sku[6] = '\0';  // End of SKU
   
   // Assign empty name to product
   for(i=0; i < 10; i++){
      prod.name[i] = ' ';
   }
   prod.name[i] = '\0'; // End of Name
   
   // Default price to 0
   prod.price = 0;
   
   // Infinite Loop
   for(;;) {
   
      // Get key from keypad
      key = kp_getn(); 
      
      // Default values of 
      switch(key) {
      
         // Go to next attribute when '#' is pressed
         case 0x0E: 
            attribute++; 
            position = 0; // Reset position
            letter = 'A'; // Reset letter to A
            
            // Return product if all attributes have been defined
            if(attribute > 2) {
               return prod;
            }
            break;
         
         // Cancel product when '*' is pressed
         case 0x0F: 
            prod.price = 0xffff; // Defines that the product was canceled
            return prod;
      }
      
      // Adjust current attribute
      switch(attribute) {
      
         // Adjust SKU
         case 0: 
            switch(key) {
               
               // If no key was pressed do nothing
               case NOKEYPRESS: break;
               
               // If 'A' is pressed increase SKU
               case 0x0A: 
                  prod.sku[5]++;
                  for( i=5; i>0; i-- ) {
                     if( prod.sku[i] == '9' + 1 ) {
                        prod.sku[i] = '0';
                        prod.sku[i-1]++;
                     }
                  }
                  if( prod.sku[i] == '9' + 1 ) {
                        prod.sku[i] = '0';
                  }
                  break;
                  
               // If 'B' is pressed decrease SKU
               case 0x0B:
                  prod.sku[5]--;
                  for( i=5; i>0; i-- ) {
                     if( prod.sku[i] == '0' - 1 ) {
                        prod.sku[i] = '9';
                        prod.sku[i-1]--;
                     }
                  }
                  if( prod.sku[i] == '0' - 1 ){
                     prod.sku[i] = '9';
                  }
                  break;
                  
               // If 'C' is pressed clear SKU
               case 0x0C: 
                  strcpy(prod.sku,"000000"); 
                  break;
               
               // If 'D' os pressed go to next attribute
               case 0x0D: 
                  attribute++; 
                  position = 0; 
                  letter = 'A'; 
                  break;
               
               // If a number is pressed shift SKU to the left and add key
               default: 
                  if( key>=0 && key<10 ) {
                     for( i=0; i<5; i++) {
                        prod.sku[i] = prod.sku[i+1];
                     }
                     prod.sku[5] = '0' + key;
                  }
            }
            break;
            
         // Adjust Name
         case 1: 
            switch(key) {
            
               // If no key was pressed do nothing
               case NOKEYPRESS: break;
               
               // If 'A' is pressed increase current letter
               case 0x0A: 
                  if(letter == ' ') {
                     letter = 'A';
                  } else  {
                     letter++;
                     if(letter>'Z') {
                        letter = ' ';
                     }
                  }
                  break;
                  
               // If 'B' is pressed decrease current letter
               case 0x0B: 
                  if(letter == ' ') {
                     letter = 'Z';
                  } else {
                     letter--;
                     if(letter<'A'){
                        letter = ' ';
                     }
                  }
                  break;
                  
               // If 'C' is pressed return to previous position
               case 0x0C: 
                  prod.name[position] = ' '; // reset current position
                  position--;
                  letter = prod.name[position];
                  
                  // If position is < 0 return to previous attribute
                  if(position > 200) {
                     attribute--;
                  }
                  break;
                  
               // If 'D' is pressed go to next position
               case 0x0D: 
                  position++;
                  letter = 'A'; // set current letter to 'A'
                  
                  // If the whole name was set go to next attribute
                  if(position >= 9){
                     attribute++;
                  }
                  break;
            
               // If number is pressed set current letter to that number
               default: 
                  if( key>=0 && key<10 )
                     letter = '0' + key;
            }
            
            // Assign current position the current letter
            prod.name[position] = letter;
            break;
            
         // Adjust Product Price (Not greater than 10,000)
         case 2: 
            switch(key) {
               // If no key press do nothing
               case NOKEYPRESS: break;
               
               // If 'A' is pressed increase price
               case 0x0A: 
                  prod.price = (prod.price + 1) % 10000; 
                  break;
                  
               // If 'B' is pressed decrease price
               case 0x0B: 
                  prod.price = (prod.price + 9999) % 10000; 
                  break;
                  
               // If 'C' is pressed clear price and go to previous attribute
               case 0x0C: 
                  prod.price = 0; 
                  attribute--; 
                  break;
                  
               // If number is pressed shift price to the left and add key
               default: 
                  if( key>=0 && key<10 ) {
                     if( prod.price > 999 ) {
                        prod.price %= 1000;
                     }
                     prod.price = prod.price * 10 + key;
                  }
            }
            break;
      }
      
      // Render only when true
      if(render) {
      
         // Print current product on display
         printProd(prod);
         
         // Makes current attribute blink (to know it is being adjusted)
         if(blink) {
            switch(attribute) {
               // Makes SKU blink
               case 0: 
                  lcd_gotoxy(9,1); 
                  printf(lcd_putc,"      "); 
                  break;
                  
               // Makes Name blink 
               case 1: 
                  lcd_gotoxy(9+position,2); 
                  printf(lcd_putc,"%c",255); 
                  break;
                  
               // Makes Price blink
               case 2: 
                  lcd_gotoxy(11,3); 
                  printf(lcd_putc,"       "); 
                  break;
                  
               // To make sure attribute is the appropriate
               default: attribute = 0;
            }
         }
         
         // Resets render to false
         render = false;
      }
   }
}


// Check product with data base
int8 check_Product( Product prod ) {

   // Local Variable Declaration
   int8 numprod;
   int8 max;
   Product comp;
   
   // Get number of products in database
   putc(ProdNum);
   max = getc();
   
   // If the price is 0 return 
   if(prod.price == 0){
         return InvalidPrice;
   }
    
   // Check with every product from database
   for(numprod = 0; numprod<max; numprod++) {
      
      // Set Communication interval to 100 ms
      delay_ms(10);
      
      // Get Product from database
      putc(SendProd);
      putc(numprod);
      receive_Product(comp);
      
      // Compare SKU
      if(strcmp(prod.sku,comp.sku) == 0) {
         return InvalidSKU;
      }
         
      // Compare Name
      if(strcmp(prod.name,comp.name) == 0) {
         return InvalidName;
      }
         
      // Display Checking message to let user know device is working 
      if( numprod % 50 == 0 ) {
         lcd_putc('\f'); lcd_gotoxy(4,2);
         printf(lcd_putc,"Checking ");
      } else if ( numprod % 10 == 0 ) {
         lcd_putc('.');
      }
   }
   
   // Return valid if product is unique
   return Valid;
}

int16 selectProducts() {

   // Local Variable declaration
   unsigned int8 num = 0;
   unsigned int8 key = 0;
   unsigned int8 prevnum = 255;
   unsigned int8 prodnum = 0;
   unsigned int8 prodquan = 0;
   int16 total = 0;
   Product product;
   
   // Get number of products in database
   putc(ProdNum);
   prodnum = getc();
   
   for(;;) {
   
      // Get key from keypad
      key = kp_getn(); 
      switch(key) {
         
         // Do nothing if no key is pressed
         case NOKEYPRESS: break;
         
         // When 'A' is pressed go to next product
         case 0x0A: 
            num = (num + 1) % prodnum; 
            prodquan=0; 
            break;
            
         // When 'B' is pressed go to previous product
         case 0x0B: 
            num = (num + prodnum - 1) % prodnum; 
            prodquan=0; 
            break;
            
         // If '#' is pressed return total (Finish)
         case 0x0E: return total;
         
         // If '*' is pressed return 0 (Cancel)
         case 0x0F: return 0;
         
         // When 'C' is pressed subtract current products from total
         case 0x0C: 
            // Only do this when total is greater than the price
            if( total >=  product.price * prodquan ) {
               total -=  product.price * prodquan;
               
               // Display subtraction message to client
               putc(PrintMessage);
               printf(" -%02u %s\n -$  %lu.00%c%c", prodquan,product.name,(product.price*prodquan),0,13);
            }
            prodquan=0; 
            break;
           
         // When 'D' is pressed add current products to total
         case 0x0D:
            // Only allow this if it does not increase 50,000
            if( total + product.price * prodquan < 50000) {
               total +=  product.price * prodquan;
               
               // Display addition message to client
               putc(PrintMessage);
               printf(" %02u %s\n $  %lu.00%c%c", prodquan,product.name, (product.price*prodquan),0,13);
            }
            prodquan=0; 
            break;
         
         // When number is pressed change current quantity (max 99)
         default: 
            if(key>=0 && key<10) {
               if(prodquan >= 10) {
                  prodquan = key;
               }
               else {
                  prodquan = (prodquan*10) + key;
               }
            }
      }
      
      // Only display products when render is true
      if(render) {
      
         // Get product from database when it has changed
         if(prevnum != num) {
            prevnum = num;
            delay_us(100);
            putc(SendProd);
            putc(num);
            receive_Product(product);
         }
         
         // DISPLAY SALE INFORMATION
         printf(lcd_putc,"\f%s: %s\n",product.sku,product.name);
         printf(lcd_putc,"PRICE: $ %lu.00\n",product.price);
         printf(lcd_putc,"QUANTITY: %u\n",prodquan);
         printf(lcd_putc,"TOTAL:  $ %lu.00\n",total);
         render = false;
      }
   }
}

// Let client make the product checkout
int1 payProducts( int16 total) {

   // Local Variable Declaration
   unsigned int8 key = 0;
   unsigned int16 paid = 0;
   int16 change = null;
   
   // Endless Loop
   for(;;) {

      // Get key from keypad
      key = kp_getn(); 
      
      
      switch(key) {
      
         // If nothing is pressed do nothing
         case NOKEYPRESS: break;
         
         // If 'A' increase amount
         case 0x0A: paid++; break;
         
         // If 'B' decrease amount
         case 0x0B: paid--; break;
         
         // If 'C' clear amount
         case 0x0C: paid = 0; break;
         
         // If 'D' calculate difference for change
         case 0x0D: change = (paid - total < 0x8000)? paid - total: 0; break;
         
         // If '#' allow checkout
         case 0x0E: if(change != 0) return true; break;
         
         // If '*' cancel
         case 0x0F: return false;
         
         // If number shift amount left and add key
         default: 
            if(key>=0 && key<10){
               paid = (paid*10) + key; 
            }
            break;
         
      }
      
      // Only display message when render
      if(render) {
      
         // Set Client message
         putc(PrintMessage);
         if(change==0){
            printf("TOTAL: $ %lu.00\nPAY:   $ %lu.00%c%c",total,paid,0,13);
         } else{
            printf("PAY:  $ %lu.00\nCHNG: $ %lu.00%c%c",paid,change,0,13);
         }
         
         // Set Selller Message
         printf(lcd_putc,"\fTOTAL:  $ %lu.00\n",total);
         printf(lcd_putc,"PAY:   $ %lu.00\n",paid);
         printf(lcd_putc,"CHANGE: $ %ld.00\n",change);
         render = false;
      }
   }
   
}

// DISPLAYS PRODUCT INFORMATION ON LCD
void printProd(Product prod) {
   printf(lcd_putc,"\fSKU:   %s\n",prod.sku);
   printf(lcd_putc,"NAME:  %s \n",prod.name);
   printf(lcd_putc,"PRICE: $ %04lu.00\n",prod.price);
}
