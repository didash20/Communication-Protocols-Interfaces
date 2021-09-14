////////////////////////////////////////////////////////////////////////////
////                             RGBLED.C                               ////
////                     RGB LED driver in port A                       ////
////                                                                    ////
////  led_init()   Must be called before any other function.            ////
////                                                                    ////
////  led_setcolor(c) Sets color to c                                   ////
////                                                                    ////
////  led_getcolrn()  Returns the number of the color on the led        ////
////                                                                    ////
////  led_off()  Turns LED OFF                                          ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////

// Color Definitions
#define OFF    0
#define RED    1
#define GREEN  2
#define YELLOW 3
#define BLUE   4
#define PURPLE 5
#define CYAN   6
#define WHITE  7

struct led_pin_map {        // This structure is overlayed on to a I/O PORTA
           int color : 3;   // to gain access to the LED.
           int und : 5;     // The bits are allocated from lower to upper
        } led;
        
#locate led = getenv("SFR:PORTA")    // This puts the entire structure
                                     // on to port A
                                     
struct led_pin_map const LED_WRITE = {0b000,0b11111}; 
                                    // For standard read mode
                                    // Color are outputs
                                    // Undefined are inputs

// Initializes the port A as LED output
void led_init( void ) {
   set_tris_a(LED_WRITE);
}

// Sets the color of the LED
void led_setcolor( int8 col ) {
   output_a(col);
}

// Returns the current color number 
int8 led_getcolrn( void ) {
   return input_a();
}

// Turns the LED OFF
void led_off() {
   output_a(OFF);
}

