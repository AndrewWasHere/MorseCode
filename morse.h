/*
  morselookup.h
  
  Interface for converting Morse code to ASCII or vice versa. The Morse code lookup table is based
  on the one presented on Wikipedia, http://en.wikipedia.org/wiki/Morse_code.
  
  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode

  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#ifndef MORSE_H
#define MORSE_H

class Morse
{
  public:
  //
  // Types
  //
  
  // Morse code is made up of DOTs (short duration) and DASHes (long duration). SPACE is a placeholder
  // to indicate the end of a Morse encode since it's a variable length code.
  enum MorseCodeElement { SPACE, DOT, DASH };
  
  //
  // Constants
  //
  
  // Maximum number of elements in a Morse code sequence.
  static const unsigned int SEQUENCE_LENGTH = 5;
  
  // Duration of signals in milliseconds.
  static const unsigned long DOT_DURATION = 100;
  static const unsigned long DASH_DURATION = 3 * DOT_DURATION;
  static const unsigned long KEY_SPACE_DURATION = DOT_DURATION;
  static const unsigned long LETTER_SPACE_DURATION = 5 * DOT_DURATION; // 3 * DOT_DURATION
  static const unsigned long WORD_SPACE_DURATION = 9 * DOT_DURATION; // 7 * DOT_DURATION
  
  //
  // Interface functions
  //
  
  // asciiToMorse()
  // Arguments:
  //   character - ASCII character to encode into Morse code.
  //   sequence - pointer to the storage for the converted morse code.
  //
  // Converts an ASCII character into a Morse code sequence of DOTs and DASHes.
  // If the character cannot be encoded, the Morse code sequence will be all SPACEs.
  static bool asciiToMorse( const char character, MorseCodeElement * const sequence );
  
  // morseToAscii()
  // Arguments:
  //   sequence - pointer to the Morse code sequence to convert to ASCII.
  // Returns:
  //   The ASCII character equivalent of the Morse code. If the Morse code cannot be
  //   converted, the function returns '?'.
  static const char morseToAscii( const MorseCodeElement * const sequence );
};

#endif
