////////////////////////////////////////////////////////////////////////////
////                        POS_COMMUNICATION.C                         ////
////       Common structures and functions for master and slave         ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////                       Structures and enums                         ////
////                                                                    ////
////  struct Product { sku, name, price }                               ////
////                                                                    ////
////  enum CommunicationComands { ProdNum, SendProd, ReceiveProd,       ////
////           SaveProd, PrintMessage, ClearScreen }                    ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////                             Functions                              ////
////                                                                    ////
////  int1 send_product(Product)    - Sends product to master/slave     ////
////                                                                    ////
////  int1 receive_product(Product) - Receives product to master/slave  ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

/*******      Communication standards      *******/
#Fuses HS
#use delay( clock = 5000000 )
#use rs232( baud=9600 , xmit=pin_c6 , rcv=pin_c7 , parity=N , bits=8 , stop=1 )

#include <string.h>

/*******        Common Structures          *******/

// Product Structure
typedef struct product{
   char  sku[7];
   char  name[11];
   int16 price;
} Product;

/*******            ENUMS             *******/
// Master - Slave Communication Commands
enum CommunicationCommands{
   ProdNum,
   SendProd,
   ReceiveProd,
   SaveProd,
   PrintMessage,
   ClearScreen
};


// CheckCodes
enum CheckCodes{
   Valid,
   InvalidSKU,
   InvalidName,
   InvalidPrice
};

/*******          FUNCTIONS           *******/

/*** Send Product between master/slave ***/
// Returns true if communication was successful
int1 send_product(Product prod) {

   // Starts communication
   getc();
   
   // Send Product values separated by zeros - end with return (13) value
   printf("%s%c%s%c%c%c%c",prod.sku,0,prod.name,0,prod.price,(prod.price)>>8,13);
   
   // End Communication
   return getc();
}

/*** Recieve Product between master/slave ***/
// Returns true if communication was successful
int1 receive_product(Product &prod) {

   // Start Communication
   putc(true);
   
   // Receive Product values 
   gets(prod.sku);
   
   // Validate if product was received correctly
   if( strlen(prod.sku)==6 && 
       strlen(prod.name)==10 && 
       prod.price > 0 && 
       prod.price < 10000 ) 
   {
      // End Communication
      putc(true);
      return true;
   }
   
   // Communication was unsuccessful
   putc(false);
   return false;
}
