#pragma once
#ifdef _MSC_VER
#include "../Shared/BuildTools/ArduinoIntellisense.h"
#endif

#include <Arduino.h>

// All commands and results are two character bytes combined into an unsigned. 
// MAKE_COMMAND flips the byte order so that the commands can be compared with
// text received from the communications channel. 
#define MAKE_COMMAND(x) ((unsigned)(((x&0xff)<<8)|(x>>8)))


struct CMessageHeader
{
  enum EType { TYPE_Command = '!', TYPE_Reply = '@' } __attribute__((__packed__));
  enum EConstants { MAX_DATA_LENGTH = 10};

  EType Type;
  uint8_t Channel;
  uint16_t Command;
  char Data[MAX_DATA_LENGTH]; // Null terminated string. 

} __attribute__((__packed__));