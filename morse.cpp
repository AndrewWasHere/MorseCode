/*
  morselookup.cpp
  
  Interface for converting Morse code to ASCII or vice versa. The Morse code lookup table is based
  on the one presented on Wikipedia, http://en.wikipedia.org/wiki/Morse_code.
  
  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode

  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#include "morse.h"

// A Morse code lookup table element.
struct MorseCodeMap
{
  char                    character;
  Morse::MorseCodeElement sequence[ Morse::SEQUENCE_LENGTH ];
};

// Lookup table map between ASCII characters and Morse code.
static const MorseCodeMap morseLookup[] = 
{
  { 'A', { Morse::DOT,   Morse::DASH,  Morse::SPACE, Morse::SPACE, Morse::SPACE } },
  { 'B', { Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::SPACE } },
  { 'C', { Morse::DASH,  Morse::DOT,   Morse::DASH,  Morse::DOT,   Morse::SPACE } },
  { 'D', { Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::SPACE, Morse::SPACE } },
  { 'E', { Morse::DOT,   Morse::SPACE, Morse::SPACE, Morse::SPACE, Morse::SPACE } },
  { 'F', { Morse::DOT,   Morse::DOT,   Morse::DASH,  Morse::DOT,   Morse::SPACE } },
  { 'G', { Morse::DASH,  Morse::DASH,  Morse::DOT,   Morse::SPACE, Morse::SPACE } },
  { 'H', { Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::SPACE } },
  { 'I', { Morse::DOT,   Morse::DOT,   Morse::SPACE, Morse::SPACE, Morse::SPACE } },
  { 'J', { Morse::DOT,   Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::SPACE } },
  { 'K', { Morse::DASH,  Morse::DOT,   Morse::DASH,  Morse::SPACE, Morse::SPACE } },
  { 'L', { Morse::DOT,   Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::SPACE } },
  { 'M', { Morse::DASH,  Morse::DASH,  Morse::SPACE, Morse::SPACE, Morse::SPACE } },
  { 'N', { Morse::DASH,  Morse::DOT,   Morse::SPACE, Morse::SPACE, Morse::SPACE } },
  { 'O', { Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::SPACE, Morse::SPACE } },
  { 'P', { Morse::DOT,   Morse::DASH,  Morse::DASH,  Morse::DOT,   Morse::SPACE } },
  { 'Q', { Morse::DASH,  Morse::DASH,  Morse::DOT,   Morse::DASH,  Morse::SPACE } },
  { 'R', { Morse::DOT,   Morse::DASH,  Morse::DOT,   Morse::SPACE, Morse::SPACE } },
  { 'S', { Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::SPACE, Morse::SPACE } },
  { 'T', { Morse::DASH,  Morse::SPACE, Morse::SPACE, Morse::SPACE, Morse::SPACE } },
  { 'U', { Morse::DOT,   Morse::DOT,   Morse::DASH,  Morse::SPACE, Morse::SPACE } },
  { 'V', { Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DASH,  Morse::SPACE } },
  { 'W', { Morse::DOT,   Morse::DASH,  Morse::DASH,  Morse::SPACE, Morse::SPACE } },
  { 'X', { Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::DASH,  Morse::SPACE } },
  { 'Y', { Morse::DASH,  Morse::DOT,   Morse::DASH,  Morse::DASH,  Morse::SPACE } },
  { 'Z', { Morse::DASH,  Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::SPACE } },
  { '0', { Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::DASH  } },
  { '1', { Morse::DOT,   Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::DASH  } },
  { '2', { Morse::DOT,   Morse::DOT,   Morse::DASH,  Morse::DASH,  Morse::DASH  } },
  { '3', { Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DASH,  Morse::DASH  } },
  { '4', { Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DASH  } },
  { '5', { Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DOT   } },
  { '6', { Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::DOT,   Morse::DOT   } },
  { '7', { Morse::DASH,  Morse::DASH,  Morse::DOT,   Morse::DOT,   Morse::DOT   } },
  { '8', { Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::DOT,   Morse::DOT   } },
  { '9', { Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::DASH,  Morse::DOT   } }
};


bool Morse::asciiToMorse( const char character, MorseCodeElement * const sequence )
{
  unsigned int idx;
  
  if ( character >= 'a' && character <= 'z' )
  {
    // Character is in the range ['a'..'z']
    idx = static_cast< unsigned int >( character - 'a' );
  }
  else if ( character >= 'A' && character <= 'Z' )
  {
    // Character is in the range ['A'..'Z']
    idx = static_cast< unsigned int >( character - 'A' );
  }
  else if ( character >= '0' && character <= '9' )
  {
    // Character is in the range ['0'..'9']
    idx = static_cast< unsigned int >( character - '0' + ('Z' - 'A' ) );
  }
  else
  {
    // Character is out of range. No mapping of the ASCII character, the returned sequence is all Morse::SPACEs.
    MorseCodeElement * element = sequence;
    for ( unsigned int idx = 0; idx < Morse::SEQUENCE_LENGTH; ++idx )
    {
      *element++ = Morse::SPACE;
    }
    
    return false;
  }
  
  // The character has been found. Copy the Morse sequence to the output buffer.
  MorseCodeElement * element = sequence;
  const MorseCodeElement * sourceElement = morseLookup[ idx ].sequence;

  for ( unsigned int lp = 0; lp < Morse::SEQUENCE_LENGTH; ++lp )
  {
    *element++ = *sourceElement++;
  }
  
  return true;
}


const char Morse::morseToAscii( const MorseCodeElement * const sequence )
{
  // Step through the lookup table.
  for ( unsigned int idx = 0; idx < sizeof( morseLookup ); ++idx )
  {
    // Reset pointers and flags.
    const MorseCodeElement * element = sequence;
    const MorseCodeElement * tableElement = morseLookup[ idx ].sequence;
    bool elementsEqual = true;
    
    // Compare sequences
    for ( unsigned int lp = 0; lp < Morse::SEQUENCE_LENGTH; ++lp )
    {
      if ( *element++ != *tableElement++ )
      {
        elementsEqual = false;
        break;
      }
    }
    
    if ( elementsEqual )
    {
      // Morse sequence found.
      return morseLookup[ idx ].character;
    }
  }
  
  // Morse sequence not found.
  return '?';
}

