////////////////////////////////////////////////////////////////////////////
////                             KP4X4.C                                ////
////                 Driver for common 4x4 Keypads                      ////
////                                                                    ////
////  kp_init()   Must be called before any other function.             ////
////                                                                    ////
////  kp_getc(keydown)   Returns character entered in keypad            ////
////                                                                    ////
////  kp_getn(keydown)   Returns hexadecimal number entered in keypad   ////
////                                                                    ////
////  keydown = case  true: return key when pressed (default)           ////
////            case false: return key when released                    ////
////                                                                    ////
////  kp_def_ast(c)   Defines returned character when * is pressed      ////
////                                                                    ////
////  kp_def_tag(c)   Defines returned character when # is pressed      ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

// As defined in the following structure the pin connection is as follows:
//     D0  COL4
//     D1  COL3
//     D2  COL2
//     D3  COL1
//     D4  ROW4
//     D5  ROW3
//     D6  ROW2
//     D7  ROW1
//

#define NOKEYPRESS 255  // Standard assignation to key when not pressed

struct kp_pin_map {     // This structure is overlayed on to a I/O P
   int col : 4;         // to gain access to the keypad pins.
   int row : 4;         // The bits are allocated from lower to upper
} kp;
        
// Locate the entire structure on port D
#locate kp = getenv("SFR:PORTD")
                   
// For standard read mode
// Columns are inputs
// Rows are outputs
struct kp_pin_map const KP_READ = {0b1111,0b0000}; 
                             
// Global Variables
int8 saved_key = NOKEYPRESS;      //The saved key pressed in char

// Char Keys found on keypad
char charkeys[16] = {
   '1','2','3','A',
   '4','5','6','B',
   '7','8','9','C',
   '*','0','#','D'
};

// Num Keys found on keypad
int8 numkeys[16] = {
   0x1,0x2,0x3,0xA,
   0x4,0x5,0x6,0xB,
   0x7,0x8,0x9,0xC,
   0xF,0x0,0xE,0xD
};

// Set port D for keypad configuration
void kp_init( void ) {
   set_tris_d(KP_READ);
}

char kp_getc( int1 keydown = true ) {
   
   // Default assignment to keypad
   int8 key = NOKEYPRESS;
   
   // Check all rows from keypad
   for(int8 row = 0; row < 4; row++){
      
      // Keypad assignment is done by shifting the 0 right 'row' times
      kp.row = 0b11110111 >> row;
      
      // Check all columns in each row
      for(int8 col = 0; col < 4; col++){
      
         // If both zeros overlap save char found in that key
         if( kp.col == (0b11110111 >> col) ){
            key = charkeys[row*4 + col];
         }
      }
   }
   
   // Return key when new key is pressed and keydown is true
   if(keydown && saved_key != key){
      saved_key = key;
      return key;
   }
   // Return key when released and keydown is false
   else if(key == NOKEYPRESS){
      key = saved_key;
      saved_key = NOKEYPRESS;
      return key;
   }
   // Save key and return nothing when pressed and keydown is false
   saved_key = key;
   return NOKEYPRESS;
   
}

int8 kp_getn( int1 keydown = true ) {
   
   // Default assignment to keypad
   int8 key = NOKEYPRESS;
   
   // Check all rows from keypad
   for(int8 row = 0; row < 4; row++){
      
      // Keypad assignment is done by shifting the 0 right 'row' times
      kp.row = 0b11110111 >> row;
      
      // Check all columns in each row
      for(int8 col = 0; col < 4; col++){
      
         // If both zeros overlap save num found in that key
         if( kp.col == (0b11110111 >> col) ){
            key = numkeys[row*4 + col];
         }
      }
   }
   
   // Return key when new key is pressed and keydown is true
   if(keydown && saved_key != key){
      saved_key = key;
      return key;
   }
   // Return key when released and keydown is false
   else if(key == NOKEYPRESS){
      key = saved_key;
      saved_key = NOKEYPRESS;
      return key;
   }
   // Save key and return nothing when pressed and keydown is false
   saved_key = key;
   return NOKEYPRESS;
   
}

void kp_def_ast( char c ) {
   charkeys[12] = c;
}

void kp_def_tag ( char c ) {
   charkeys[14] = c;
}
