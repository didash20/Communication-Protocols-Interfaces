////////////////////////////////////////////////////////////////////////////
////                             KP4X4.C                                ////
////                 Driver for common 4x4 Keypads                      ////
////                                                                    ////
////  kp_init()   Must be called before any other function.             ////
////                                                                    ////
////  kp_getc(m)   Returns character entered in keypad                  ////
////                                                                    ////
////  kp_getn(m)   Returns hexadecimal number entered in keypad         ////
////                                                                    ////
////  keydown =    case false: return key when released (default)       ////
////               case  true: return key when pressed                  ////
////                                                                    ////
////  kp_def_ast(c)   Defines character returned when * is pressed      ////
////                                                                    ////
////  kp_def_tag(c)   Defines character returned when # is pressed      ////
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

struct kp_pin_map {        //This structure is overlayed on to a I/O P
           int col : 4;    //to gain access to the keypad pins.
           int row : 4;    // The bits are allocated from lower to upper
        } kp;
        
#locate kp = getenv("SFR:PORTD")    // This puts the entire structure
                                    // on to port D
                                     
struct kp_pin_map const KP_READ = {0b1111,0b0000}; 
                                    // For standard read mode
                                    // Columns are inputs
                                    // Rows are outputs
                                    
char ast = '*';      //Standard assignation to asterisk
char tag = '#';      //Standard assignation to hashtag
int8 skeyc = 255;       //The saved key pressed in char
int8 skeyn = 255;       //The saved key pressed in num
 

void kp_init( void )
{
   set_tris_d(KP_READ);
}

char kp_getc( int1 keydown = 0 )
{
   int8 key = 255;

   kp.row = 0b0111;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = '1'; break;
      case 0b1011: key = '2'; break;
      case 0b1101: key = '3'; break;
      case 0b1110: key = 'A'; break;
      default: key = '\0';
   }
   
   if(keydown && key != 255)
      return key;
   
   kp.row = 0b1011;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = '4'; break;
      case 0b1011: key = '5'; break;
      case 0b1101: key = '6'; break;
      case 0b1110: key = 'B'; break;
      default: key = '\0';
   }
   
   if(keydown && key != 255)
      return key;
      
   kp.row = 0b1101;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = '7'; break;
      case 0b1011: key = '8'; break;
      case 0b1101: key = '9'; break;
      case 0b1110: key = 'C'; break;
      default: key = '\0';
   }  
   
   if(keydown && key != 255)
      return key;
      
   kp.row = 0b1110;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = ast; break;
      case 0b1011: key = '0'; break;
      case 0b1101: key = tag; break;
      case 0b1110: key = 'D'; break;
      default: key = '\0';
   }  
   
   if(keydown && key != 255)
      return key;
      
   if(!keydown && key == 255){
      key = skeyc;
      skeyc = 255;
      return key;
   }
   
   skeyc = key;
   return 255;
}

int8 kp_getn( int1 keydown = 0  )
{
   int8 key = 255;

   kp.row = 0b0111;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = 0x1; break;
      case 0b1011: key = 0x2; break;
      case 0b1101: key = 0x3; break;
      case 0b1110: key = 0xA; break;
      default: key = 254;
   }
   
   if(keydown && key < 0x10)
      return key;
   
   kp.row = 0b1011;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = 0x4; break;
      case 0b1011: key = 0x5; break;
      case 0b1101: key = 0x6; break;
      case 0b1110: key = 0xB; break;
      default: key = 254;
   }
   
   if(keydown && key < 0x10)
      return key;
      
   kp.row = 0b1101;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = 0x7; break;
      case 0b1011: key = 0x8; break;
      case 0b1101: key = 0x9; break;
      case 0b1110: key = 0xC; break;
      default: key = 254;
   }  
   
   if(keydown && key < 0x10)
      return key;
      
   kp.row = 0b1110;
   switch(kp.col)
   {
      case 0b1111: break;
      case 0b0111: key = 0xF; break;
      case 0b1011: key = 0x0; break;
      case 0b1101: key = 0xE; break;
      case 0b1110: key = 0xD; break;
      default: key = 254;
   }  
   
   if(keydown && key < 0x10)
      return key;
      
   if(!keydown && key == 255){
      key = skeyn;
      skeyn = 255;
      return key;
   }
   
   skeyn = key;
   return 255;
}

void kp_def_ast( char c )
{
   ast = c;
}

void kp_def_tag ( char c )
{
   tag = c;
}
