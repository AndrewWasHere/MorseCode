/*
  asciitomorse.h
  
  Class to convert ASCII to Morse code
    
  Written by Andrew Lin, April 2011
  https://github.com/AndrewWasHere/MorseCode

  This code is released under the Creative Commons Attribution 3.0 license
  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
*/
#include <WProgram.h>
#include "asciitomorse.h"
#include "trace.h"

AsciiToMorse::AsciiToMorse() :
  state( IDLE ),
  eventTimestamp( 0 ),
  codewordReadPoint( 0 ),
  queueInsertPoint( 0 ),
  queueExtractPoint( 0 ),
  outputLine( 13 )
{
  initializeCodeword();
  
  for ( unsigned int idx = 0; idx < queueSize; ++idx )
  {
    queue[ idx ] = 0;
  }
}

void AsciiToMorse::addChar( const char character )
{
  switch ( state )
  {
    case IDLE:
      addCharIdle( character );
      break;
    case KEYING:
    case KEY_SPACE:
    case LETTER_SPACE:
      addCharKeying( character );
      break;
    default:
      Serial.println( "\n\n( ATM::addChar() ) ERROR: Unknown state." );
      break;
  }
}

void AsciiToMorse::addCharIdle( const char character )
{
  // If a character gets added while idle, start processing it immediately.
  if ( character == ' ' )
  {
    // SPACE is a special case.
    processSpace();
  }
  else
  {
    processCharacter( character );
  }
}

void AsciiToMorse::addCharKeying( const char character )
{
  // If a character gets added while keying, put it on the queue for later processing.
  queue[ queueInsertPoint ] = character;
  if ( ++queueInsertPoint >= sizeof( queue ) )
  {
    queueInsertPoint = 0;
  }  
}

void AsciiToMorse::timestamp( const unsigned long & now )
{
  if ( now >= eventTimestamp )
  {
    switch( state )
    {
      case IDLE:
        // Ignore.
        break;
      case KEYING:
        // Key duration is finished.
        timestampKeying( now );
        break;
      case KEY_SPACE:
        timestampKeySpace( now );
        break;
      case LETTER_SPACE:
        timestampLetterSpace( now );
        break;
      default:
        Serial.println( "\n\nERROR ( ATM::timestamp() ): Unknown state." );
        break;
    }
  }
}

void AsciiToMorse::timestampKeying( const unsigned long & now )
{
  #if TRACE
  Serial.println( "( ATM::timestampKeying() ) outputLine -> LOW." );
  #endif
  digitalWrite( outputLine, LOW );
  eventTimestamp = now + Morse::KEY_SPACE_DURATION;
  
  #if TRACE_STATE
  Serial.println( "( ATM::timestampKeying() ) state -> KEY_SPACE" );
  #endif
  state = KEY_SPACE;
}

void AsciiToMorse::timestampKeySpace( const unsigned long & now )
{
  if ( ( codewordReadPoint >= Morse::SEQUENCE_LENGTH ) || ( codeword[ codewordReadPoint ] == Morse::SPACE ) )
  {
    // Codeword is done.
    eventTimestamp = now + Morse::LETTER_SPACE_DURATION - Morse::KEY_SPACE_DURATION;
    
    #if TRACE_STATE
    Serial.println( "( ATM::timestampKeySpace() ) state -> LETTER_SPACE" );
    #endif
    state = LETTER_SPACE;
  }
  else
  {
    // Codeword is not done.
    switch ( codeword[ codewordReadPoint++ ] )
    {
    case ( Morse::DOT ):
      #if TRACE_ATM_OUTPUT
      Serial.println( "( ATM::timestampKeySpace() ) DOT" );
      #endif
      outputKey( Morse::DOT_DURATION );
      #if TRACE_STATE
      Serial.println( "( ATM::timestampKeySpace() ) state -> KEYING" );
      #endif
      state = KEYING;
      break;
    case ( Morse::DASH ):
      #if TRACE_ATM_OUTPUT
      Serial.println( "( ATM::timestampKeySpace() ) DASH" );
      #endif
      outputKey( Morse::DASH_DURATION );
      #if TRACE_STATE
      Serial.println( "( ATM::timestampKeySpace() ) state -> KEYING" );
      #endif
      state = KEYING;
      break;
    default:
      Serial.println( "\n\n( ATM::timestampKeySpace() ) ERROR: Unknown Morse key type in codeword." );
      break;
    }
  }
}

void AsciiToMorse::timestampLetterSpace( const unsigned long & now )
{
  if ( queueExtractPoint == queueInsertPoint )
  {
    // Character queue is empty.
    #if TRACE_STATE
    Serial.println( "( ATM::timestampLetterSpace() ) state -> IDLE" );
    #endif
    state = IDLE;
  }
  else
  {
    // Convert next character to Morse code.
    if ( queue[ queueExtractPoint ] == ' ' )
    {
      // SPACE is a special case.
      processSpace();
    }
    else
    {
      // Convert character to Morse code. Start spitting it out.
      processCharacter( queue[ queueExtractPoint ] );
    }
    
    // Circularly increment the extract point.
    if ( ++queueExtractPoint > sizeof( queue ) )
    {
      queueExtractPoint = 0;
    }
  }
}

void AsciiToMorse::initializeCodeword()
{
  for ( unsigned int idx = 0; idx < Morse::SEQUENCE_LENGTH; ++idx )
  {
    codeword[ idx ] = Morse::SPACE;
  }
}

void AsciiToMorse::setOutputLine( const int line )
{
  outputLine = line;
}

void AsciiToMorse::outputKey( const unsigned long keyDuration )
{
  #if TRACE
  Serial.println( "( ATM::outputKey() ) outputLine -> HIGH." );
  #endif
  // Raise the output line.
  digitalWrite( outputLine, HIGH );
  // Set the timestamp for when to handle the next event.
  eventTimestamp = millis() + keyDuration;
}

void AsciiToMorse::processCharacter( const char character )
{
  // Convert to Morse code.
  if ( !Morse::asciiToMorse( character, codeword ) )
  {
    #if TRACE
    Serial.print( "( ATM::processCharacter() ) character '" );
    Serial.print( character );
    Serial.println( "' cannot be represented in Morse code. Skipping." );
    #endif
    
    return;
  }
  
  codewordReadPoint = 0;
  
  #if TRACE
  Serial.print( "( ATM::processCharacter() ) character: " );
  Serial.println( character );
  printMorse();
  #endif
  
  // Process the first key of the codeword.
  switch ( codeword[ codewordReadPoint++ ] )
  {
    case ( Morse::DOT ):
      #if TRACE_ATM_OUTPUT
      Serial.println( "( ATM::processCharacter() ) DOT" );
      #endif
      outputKey( Morse::DOT_DURATION );
      #if TRACE_STATE
      Serial.println( "( ATM::processCharacter() ) state -> KEYING" );
      #endif
      state = KEYING;
      break;
    case ( Morse::DASH ):
      #if TRACE_ATM_OUTPUT
      Serial.println( "( ATM::processCharacter() ) DASH" );
      #endif
      outputKey( Morse::DASH_DURATION );
      #if TRACE_STATE
      Serial.println( "( ATM::processCharacter() ) state -> KEYING" );
      #endif
      state = KEYING;
      break;
    case ( Morse::SPACE ):
      // Character did not map to a Morse codeword.
      #if TRACE_ATM_OUTPUT
      Serial.println( "( ATM::processCharacter() ) SPACE" );
      #endif
      break;
    default:
      Serial.print( "\n\n( ATM::processCharacter() ) ERROR: Unknown Morse key type in codeword: " );
      Serial.println( *codeword, HEX );
      break;
  }
}

void AsciiToMorse::processSpace()
{
  #if TRACE_ATM_OUTPUT
  Serial.println( "( ATM::processSpace() ) WORD SPACE" );
  #endif
  #if TRACE
  Serial.println( "( ATM::processSpace() ) outputLine -> LOW." );
  #endif
  // SPACE is a word separation, which in Morse code is a LOW output for a long duration.
  initializeCodeword();
  codewordReadPoint = 0;
  
  digitalWrite( outputLine, LOW );
  eventTimestamp = millis() + Morse::WORD_SPACE_DURATION - Morse::KEY_SPACE_DURATION - Morse::LETTER_SPACE_DURATION;
  
  #if TRACE_STATE
  Serial.println( "( ATM::processSpace() ) state -> KEYING" );
  #endif
  state = KEYING;
}

void AsciiToMorse::printMorse()
{
  Serial.print( "Morse codeword: " );
  for ( unsigned int idx = 0; idx < Morse::SEQUENCE_LENGTH; ++idx )
  {
    switch ( codeword[ idx ] )
    {
      case Morse::DASH:
        Serial.print( "DASH ");
        break;
      case Morse::DOT:
        Serial.print( "DOT " );
        break;
      case Morse::SPACE:
        Serial.print( "SPACE " );
        break;
      default:
        Serial.print( "??? " );
        break;
    }
  }
  Serial.println();
}
