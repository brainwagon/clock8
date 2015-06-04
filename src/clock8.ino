//     _         _   ___ 
//  __| |___  __| |_( _ )
// / _| / _ \/ _| / / _ \
// \__|_\___/\__|_\_\___/
//                      
// A very simple (and not terribly accurate) clock 
// that drives an 8 digit, 7 segment LED display
// such as this one from deal extreme:
//     http://www.dx.com/p/250813
//
// Code by Mark VandeWettering, released to the public 
// domain.  It would be nice to send me a note if
// you use it.
//
// Note: to really make an accurate clock, you probably
// want to pair this code with a module that uses the 
// DS1307 (mediocre) or DS3231 (pretty accurate).
// 

#include <RTClib.h>
#include <TimerOne.h>

RTC_Millis rtc ;

#define NDIGITS         8
#define NDIGITS_MASK    7

const int latchpin = 12 ;       // connect to RCK
const int clockpin = 11 ;       // connect to SCK
const int datapin = 10 ;        // connect to DIO

byte col[NDIGITS] = {
    0b00000001,
    0b00000010,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b10000000
} ;

byte seg[] = {
    0b11000000,  // 0
    0b11111001,  // 1
    0b10100100,  // 2
    0b10110000,  // 3
    0b10011001,  // 4
    0b10010010,  // 5
    0b10000010,  // 6
    0b11111000,  // 7
    0b10000000,  // 8
    0b10010000,  // 9  
} ;     

byte segbuf[NDIGITS] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } ;
byte segcnt = 0;

// If you don't have a real Arduino UNO (or a compatible with the 
// same port mappings), then you should comment out the following
// define, and use the portable but slow version.
//
// There is probably a better way to write this for maximum 
// portability, but this was just a simple test.
//
#define USE_DIRECT_PORT_ACCESS	

#ifndef USE_DIRECT_PORT_ACCESS

void
LED_irq(void)
{
    digitalWrite(latchpin, LOW);

    // select the digit...
    shiftOut(datapin, clockpin, MSBFIRST, col[segcnt]) ;
    // and select the segments
    shiftOut(datapin, clockpin, MSBFIRST, segbuf[segcnt]) ;

    digitalWrite(latchpin, HIGH) ;
    segcnt ++ ;
    segcnt &= NDIGITS_MASK ;
}

#else

// 
// The code below is hard coded to run fast on conventional 
// Arduino UNO boards.  Instead of calling digitalWrite, 
// it uses simple bitwise operations directly on the output
// port buffers.  WARNING: this will not work if you use
// something other than a truly compatible Arduino UNO board.
// 
// So why use this code?  It's around 8x faster than the 
// code above. We could make it even faster in a number of 
// of ways (unroll loops), but right now it consumes about 
// 48useconds out of every 2 ms, or roughly 2.4% of the 
// available runtime. The slower code above takes about 
// 18% of available cpu cycles.
//

void
LED_irq(void)
{
    PORTB &= ~0b00010000 ;
   
    for (int i=0; i<8; i++) {
	if (col[segcnt] & (0x80 >> i))
	    PORTB |= 0b00000100 ;
	else
	    PORTB &= ~0b00000100 ;
	PORTB |= 0b00001000 ;
	PORTB &= ~0b00001000 ;
    }

    for (int i=0; i<8; i++) {
	if (segbuf[segcnt] & (0x80 >> i))
	    PORTB |= 0b00000100 ;
	else
	    PORTB &= ~0b00000100 ;
	PORTB |= 0b00001000 ;
	PORTB &= ~0b00001000 ;
    }

    PORTB |= 0b00010000 ;

    segcnt ++ ;
    segcnt &= NDIGITS_MASK ;
}
#endif

void
setup()
{
    // setup all the data pins...

    pinMode(latchpin, OUTPUT) ;
    pinMode(clockpin, OUTPUT) ;
    pinMode(datapin, OUTPUT) ;

    Serial.begin(9600) ;
    Serial.println("8x7 TEST") ;

    Timer1.initialize(2000);      // 4 ms
    Timer1.attachInterrupt(LED_irq) ;
    Timer1.start() ;

    // Should auto-run at this point... 

    rtc.begin(DateTime(F(__DATE__), F(__TIME__))) ;
}


void
loop()
{
    for (;;) {
	DateTime now = rtc.now() ;
	int Hour = now.hour() ;
	int Minute = now.minute() ;
	int Second = now.second() ;

	segbuf[0] = seg[Hour / 10] ;
	segbuf[1] = seg[Hour % 10] ;

	segbuf[3] = seg[Minute / 10] ;
	segbuf[4] = seg[Minute % 10] ;

	segbuf[6] = seg[Second / 10] ;
	segbuf[7] = seg[Second % 10] ;

	delay(1000) ;
    }
}
