/*
  morsetoascii.h
  
  Class to convert morse code to ASCII
    
  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode

  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#include <WProgram.h>
#include "morsetoascii.h"

MorseToAscii::MorseToAscii() :
  state( IDLE ),
  keypressTimestamp( 0 ),
  keyInIdx( 0 )
{
  initializeCodeword();
}

void MorseToAscii::initializeCodeword()
{
  // Initialize the codeword buffer.
  for ( unsigned int idx = 0; idx < sizeof( codeword ); ++idx )
  {
    codeword[ idx ] = Morse::SPACE;
  }
  
  keyInIdx = 0;
}

void MorseToAscii::keypress( const Morse::MorseCodeElement key )
{
  switch ( state )
  {
    case IDLE:
      keypressIdle( key );
      break;
    case ENCODING:
      keypressEncoding( key );
      break;
    case EOW_CHECK:
      keypressEOWCheck( key );
      break;
    default:
      // This should never happen. Exceptions aren't supported, so do something more mundane.
      Serial.println( "\n\n( MTA::keypress() ) ERROR: Unknown state." );
      break;
  }
}

void MorseToAscii::keypressIdle( const Morse::MorseCodeElement key )
{
  // Store key in codeword buffer. Timestamp the key.
  keypressCommon( key );
  
  // Change state to ENCODING.
  state = ENCODING;
}

void MorseToAscii::keypressEncoding( const Morse::MorseCodeElement key )
{
  // Store key in codeword buffer. Timestamp the key.
  keypressCommon( key );
  
  // Remain in ENCODING state.
}

void MorseToAscii::keypressEOWCheck( const Morse::MorseCodeElement key )
{
  // Store key in codeword buffer. Timestamp the key.
  keypressCommon( key );
  
  // Change state to ENCODING
  state = ENCODING;
}

void MorseToAscii::keypressCommon( const Morse::MorseCodeElement key )
{
  // Store key in codeword buffer.
  if ( keyInIdx < Morse::SEQUENCE_LENGTH )
  {
    codeword[ keyInIdx++ ] = key;
  }
  else if ( keyInIdx == Morse::SEQUENCE_LENGTH )
  {
    // The codeword length is longer than any valid codeword. Therefore, it's garbage.
    // Abandon the codeword, but don't accept a new codeword until the sender regains
    // its senses.
    initializeCodeword();
    ++keyInIdx;
  }
  // else do nothing. We still consider it garbage, so we ignore it until it goes away.
  
  // Timestamp the key. Yes, even if it's garbage.
  keypressTimestamp = millis();
}

void MorseToAscii::timestamp( const unsigned long & now )
{
  switch ( state )
  {
    case IDLE:
      // Ignore timestamps in the IDLE state.
      return;
    case ENCODING:
      timestampEncoding( now );
      break;
    case EOW_CHECK:
      timestampEOWCheck( now );
      break;
    default:
      // This should never happen. Exceptions aren't supported, so do something more mundane.
      Serial.println( "\n\n( MTA::timestamp() ) ERROR: unknown state." );
      break;
  }
}

void MorseToAscii::timestampEncoding( const unsigned long & now )
{
  if ( now - keypressTimestamp > Morse::LETTER_SPACE_DURATION )
  {
    // Convert Morse codeword to ASCII character, and write to serial port.
    Serial.print( Morse::morseToAscii( codeword ) );
    
    // Reset codeword.
    initializeCodeword();
    
    // Change state to EOW_CHECK.
    state = EOW_CHECK;
  }
}

void MorseToAscii::timestampEOWCheck( const unsigned long & now )
{
  if ( now - keypressTimestamp > Morse::WORD_SPACE_DURATION )
  {
    // Write an ASCII SPACE to the serial port.
    Serial.print( ' ' );
    
    // Change state to IDLE.
    state = IDLE;
  }
}
