#pragma once

#ifdef _MSC_VER
#include "../Shared/BuildTools/ArduinoIntellisense.h"
#endif

#include <Arduino.h>
#include "Message.h"

class CSerialHandler
{
  HardwareSerial *m_pPort;
  uint8_t m_RawReceiveBuffer[sizeof(CMessageHeader)];

  enum EConstants 
  { 
    StartOfMessage = '!', // Character to signal start of message. 
    EndOfMessage = '\n',   // Character to signal end of message.
    CharacterTimeout = 10, // Timeout for character. 
    BaudRate = 9600,       // Baud rate for serial communications. 
  };

  bool m_bOverflow, m_bReceivingMessage, m_bInvalidData;
  int m_nNextChar;

public:
  CSerialHandler(HardwareSerial *pPort);
  void Setup();

  CMessageHeader *ProcessData();

  void Send(const char *pchMessage);
  void ReplyOK(CMessageHeader *pMessage);
  void ReplyError(const char *pchDescription);
protected:
  void ReplyOverflowError();
  void ReplyInvalidMessageError();
};

