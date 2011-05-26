/*
  MorseCode
  
  Application to accept Morse code on a digital input, and output the equivalent text over the serial port.
  In the other direction, the application accepts ASCII text over the serial port, and converts it into
  Morse code on an LED connected to a digital output.

  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode
  
  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#include "asciitomorse.h"
#include "morsetoascii.h"
#include "trace.h"

// Typedefs
enum KeyState { KEY_DOWN, KEY_UP };

// Constants
const int morseKeyPin = 2;
const int morseKeyLED = 3;
const int morseOutputPin = 13;

// Common storage.
unsigned long keyDuration = 0;
KeyState      keyState = KEY_UP;

// Things that actually do stuff.
AsciiToMorse atm;
MorseToAscii mta;

// setup()
//  
// Initialization function called on bootup.
void setup()
{
  Serial.begin( 9600 );
  
  // Set up ASCII-to-Morse output.
  pinMode( morseOutputPin, OUTPUT );
  atm.setOutputLine( morseOutputPin );
  
  // Set up Morse key feedback LED.
  pinMode( morseKeyLED, OUTPUT );
  digitalWrite( morseKeyLED, LOW );

  // Set up Morse key input, pulled up.
  pinMode( morseKeyPin, INPUT );
  digitalWrite( morseKeyPin, HIGH );
}

// loop()
//  
// Application main loop.
void loop()
{  
  // Update timing info.
  atm.timestamp( millis() );
  if ( keyState == KEY_UP )
  {
    mta.timestamp( millis() );
  }
  
  // Look for ASCII input on the serial port.
  if ( Serial.available() > 0 )
  {
    #if TRACE
    char c = Serial.read();
    Serial.print( "( loop() ) Converting to Morse code: " );
    Serial.println( c );
    atm.addChar( c );
    #else
    atm.addChar( Serial.read() );
    #endif
  }
  
  // Look for Morse keypress.
  if ( sampleInput() )
  {
    #if TRACE
    Serial.print( "( loop() ) Key duration: " );
    Serial.println( keyDuration );
    #endif
        
    // DOT or DASH?
    if ( keyDuration >= Morse::DOT_DURATION && keyDuration < Morse::DASH_DURATION )
    {
      // DOT.
      #if TRACE
      Serial.println( "( loop() ) DOT keyed." );
      #endif
      mta.keypress( Morse::DOT );
    }
    else if ( keyDuration >= Morse::DASH_DURATION )
    {
      // DASH.
      #if TRACE
      Serial.println( "( loop() ) DASH keyed" );
      #endif
      mta.keypress( Morse::DASH );
    }
    // else it's noise. Ignore it.
  }
}

// sampleInput()
//
bool sampleInput()
{ 
  const unsigned long debounceThreshold = 50; // ms
  
  static int previousLevel = HIGH;
  static unsigned long edgeTime = 0;
  static unsigned long keyDownTime = 0;
  
  bool keyAvailable = false;
  unsigned long now = millis();
  int level = digitalRead( morseKeyPin );
  digitalWrite( morseKeyLED, level == HIGH ? LOW : HIGH );
  
  if ( level != previousLevel )
  {
    edgeTime = now;
  }
  
  if ( now - edgeTime > debounceThreshold )
  {
    // Switch debounced
    if ( level == LOW and keyState == KEY_UP )
    {
      // Key pressed.
      keyDownTime = now;
      keyState = KEY_DOWN;
    }
    else if ( level == HIGH and keyState == KEY_DOWN )
    {
      // Key released.
      keyDuration = now - keyDownTime;
      keyAvailable = true;
      keyState = KEY_UP;
    }
  }
  
  previousLevel = level;
  return keyAvailable;
}

