/*
  morsetoascii.h
  
  Class to convert morse code to ASCII
    
  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode

  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#ifndef MORSETOASCII_H
#define MORSETOASCII_H

#include "morse.h"

class MorseToAscii
{
  public:  
  // Constructor
  MorseToAscii();
  
  // keypress()
  // Arguments:
  //   key - DOT or DASH.
  //   when - timestamp in milliseconds of the key.
  // Notify the Morse to ASCII class that a keypress has occurred. This function should be called
  // when a DOT or DASH has been keyed on the input.
  void keypress( const Morse::MorseCodeElement key );
  
  // timestamp()
  // Arguments:
  //   now - Time in milliseconds since startup.
  // Notify the Morse to ASCII class of passage of time. This function should be called every time the loop()
  // function is executed, with a call to millis() passed in.
  void timestamp( const unsigned long & now );
  
  private:
  // State machine:
  //                                 keypress: store, timestamp
  //                                           +----+
  //                                           |    V
  // +------+  keypress: store, timestamp   +----------+
  // | IDLE |------------------------------>| ENCODING |
  // +------+                               +----------+
  //    ^                                      ^   |
  //    |                            keypress: |   | delta_t > LETTER_SPACE_DURATION:
  //    |                               store, |   | convert Morse to ASCII, and transmit.
  //    |                            timestamp |   |
  //    |                                      |   V
  //    |                                  +-----------+
  //    +----------------------------------| EOW_CHECK |
  //       delta_t > WORD_SPACE_DURATION:  +-----------+
  //       transmit ASCII SPACE
  //
  enum State { IDLE, ENCODING, EOW_CHECK };
  
  State                   state;
  unsigned long           keypressTimestamp;                  // Time of last keypress.
  Morse::MorseCodeElement codeword[ Morse::SEQUENCE_LENGTH ]; // Keypress storage.
  unsigned int            keyInIdx;                           // Position in codeword to store received key in.
    
  // initializeCodeword()
  // Prepare the codeword buffer to receive data.
  void initializeCodeword();
  
  // keypressIdle()
  // Arguments:
  //   key - DOT or DASH.
  //
  // Process a keypress in the IDLE state.
  void keypressIdle( const Morse::MorseCodeElement key );

  // keypressEncoding()
  // Arguments:
  //   key - DOT or DASH.
  //
  // Process a keypress in the IDLE state.
  void keypressEncoding( const Morse::MorseCodeElement key );

  // keypressEOWCheck()
  // Arguments:
  //   key - DOT or DASH.
  //
  // Process a keypress in the IDLE state.
  void keypressEOWCheck( const Morse::MorseCodeElement key );
  
  // keypressCommon()
  // Arguments:
  //   key - DOT or DASH.
  //
  // Common processing of a keypress to all states.
  void keypressCommon( const Morse::MorseCodeElement key );
  
  // timestampEncoding()
  // Arguments:
  //   now - Time in milliseconds since startup.
  //
  // Process a timestamp in the ENCODING state.
  void timestampEncoding( const unsigned long & now );

  // timestampEOWCheck()
  // Arguments:
  //   now - Time in milliseconds since startup.
  //
  // Process a timestamp in the EOW_CHECK state.
  void timestampEOWCheck( const unsigned long & now );
};

#endif
