/*
  asciitomorse.h
  
  Class to convert ASCII to Morse code
    
  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode

  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#ifndef ASCIITOMORSE_H
#define ASCIITOMORSE_H
#include "morse.h"

class AsciiToMorse
{
  public:
  // Constructor
  AsciiToMorse();
  
  // setOutputLine()
  // Arguments:
  //   line - Pin number to use as the output line.
  // Configures which digital pin to use as our output line.
  void setOutputLine( const int line );
  
  //
  // Arguments:
  //   character - ASCII character to convert to Morse code.
  // Notify the ASCII to Morse class that an ASCII character has been received.
  void addChar( const char character );
  
  // timestamp()
  // Arguments:
  //   now - Time in milliseconds since startup.
  // Notify the ASCII to Morse class of passage of time. This function should be called every time the loop()
  // function is executed, with a call to millis() passed in.
  void timestamp( const unsigned long & now );
  
  private:
  // State machine:
  // NOTE: Doesn't show handling of ASCII SPACE when converting char to Morse code. A SPACE is represented
  // by a LOW output for a long duration. But adding this to the state machine diagram would have been too
  // wordy for ASCII art. State transitions are unaffected.
  //                                             
  //                                             add char: queue char    
  //                                                   +----+
  //          add char: convert char to Morse code,    |    V
  // +------+ raise output, set event timestamp.     +--------+
  // | IDLE |--------------------------------------->| KEYING | <-------------------+
  // +------+                                  +---->|        |                     |
  //    ^                                      |     +--------+                     |
  //    |                                      |         |                          |
  //    |                                      |         | timestamp: drop output,  | timestamp[ key in queue ]:
  //    |                                      |         | set event timestamp      | raise line, set event timestamp
  //    |                                      |         V                          |
  //    |          timestamp[ char in queue ]: |    +-----------+                   |
  //    |          convert char to Morse code, |    | KEY_SPACE |-------------------+
  //    |   raise output, set event timestamp  |    +-----------+
  //    |                                      |      ^    |   |
  //    |                                      |      +----+   | timestamp[ key queue empty ]:
  //    |                                      |    add char:  | set event timestamp
  //    |                                      |   queue char  |
  //    |                                      |               V
  //    |                                      |   +--------------+
  //    |                                      +---| LETTER_SPACE |
  //    +------------------------------------------|              |
  //     timestamp[ char queue empty ]: do nothing +--------------+
  //                                                     ^    | add char: queue char
  //                                                     +----+
  enum State { IDLE, KEYING, KEY_SPACE, LETTER_SPACE };
  static const unsigned int queueSize = 128;
  
  State state;
  unsigned long             eventTimestamp;
  Morse::MorseCodeElement   codeword[ Morse::SEQUENCE_LENGTH ];
  unsigned int              codewordReadPoint;
  char                      queue[ queueSize ];
  unsigned int              queueInsertPoint;
  unsigned int              queueExtractPoint;
  int                       outputLine;
  
  // addCharIdle()
  // Arguments:
  //   character - ASCII character to convert to Morse code.
  // Process the received ASCII character in the IDLE state.
  void addCharIdle( const char character );
  
  // addCharKeying()
  // Arguments:
  //   character - ASCII character to convert to Morse code.
  // Process the received ASCII character in the KEYING, KEY_SPACE, or LETTER_SPACE state.
  void addCharKeying( const char character );
  
  // timestampKeying()
  // Arguments:
  //   now - Time in milliseconds since startup.
  // Process a timestamp in the KEYING state.
  void timestampKeying( const unsigned long & now );

  // timestampKeySpace()
  // Arguments:
  //   now - Time in milliseconds since startup.
  // Process a timestamp in the KEY_SPACE state.
  void timestampKeySpace( const unsigned long & now );

  // timestampLetterSpace()
  // Arguments:
  //   now - Time in milliseconds since startup.
  // Process a timestamp in the LETTER_SPACE state.
  void timestampLetterSpace( const unsigned long & now );

  //
  // Tools and Helpers
  //
  
  // initializeCodeword()
  // Sets values of all codeword array elements to SPACE.
  void initializeCodeword();
  
  // outputKey()
  // Arguments:
  //   keyDuration - Time to set output line high.
  // Raises the output line, and sets the event timestamp to the appropriate point in the future.
  void outputKey( const unsigned long keyDuration );
  
  // processCharacter()
  // Arguments:
  //   character - the ASCII character to convert to Morse code.
  // Converts character to its Morse code. Starts outputting the Morse code.
  void processCharacter( const char character );
  
  // processSpace()
  // Handles the special case of an ASCII SPACE.
  void processSpace();
  
  // printMorse()
  // Prints the Morse codword.
  void printMorse();
};

#endif
