#include "SerialHandler.h"


CSerialHandler::CSerialHandler(HardwareSerial *pPort) 
{
  m_bOverflow = false;
  m_nNextChar = 0;
  m_pPort = pPort;
}

void CSerialHandler::Setup()
{
  m_pPort->begin(BaudRate);
}

/* Process serial data as it comes in, filling the buffer. Return complete, decoded
** messages. If no message available, returns null. 
** Looks for messages like: !1SOT1234\n */
CMessageHeader *CSerialHandler::ProcessData()
{
  // Track the time stamp of the last character received. 
  // if it takes too long to receive the next character, the 
  // buffer is reset. 
  static uint32_t uLastReceiveTime;
  uint32_t uNow;

  char chReceived;

  uNow = millis();
  chReceived = m_pPort->read();

  if (chReceived != -1)
  {
    // Record time that the this character is received. 
    uLastReceiveTime = uNow;

    if (chReceived == StartOfMessage)
    {
      m_nNextChar = 0;
      m_RawReceiveBuffer[m_nNextChar++] = chReceived;
      m_bReceivingMessage = true;
      m_bInvalidData = false;
      m_bOverflow = false;
    }

    if (m_bReceivingMessage && chReceived != StartOfMessage)
      if (chReceived == EndOfMessage)
      {
        m_RawReceiveBuffer[m_nNextChar] = '\0';
        m_bReceivingMessage = false;

        if (m_bOverflow)
          ReplyOverflowError();
        if (m_bInvalidData)
          ReplyInvalidMessageError();
        else
        {
          CMessageHeader *pMessage;
          pMessage = (CMessageHeader*)m_RawReceiveBuffer;
          if (pMessage->Type != CMessageHeader::TYPE_Command)
          {
            ReplyError("Unk type");
            return NULL;
          } else if (m_nNextChar < (sizeof(CMessageHeader) - CMessageHeader::MAX_DATA_LENGTH))
          {
            ReplyError("Msg too small");
            return NULL;
          }

          return pMessage;
        }
      }
      else
      {
        if (!m_bOverflow && chReceived != '\r' && chReceived != '\n')
        {
          m_RawReceiveBuffer[m_nNextChar++] = chReceived;
          if (m_nNextChar >= sizeof(m_RawReceiveBuffer) - 1)
            m_bOverflow = true;
        }
      }
  } else if (m_bReceivingMessage && uNow - uLastReceiveTime > CharacterTimeout)
  {
    m_bReceivingMessage = false; // we timed out. 
  }

  return NULL;
}

void CSerialHandler::Send(const char *pchMessage)
{
  m_pPort->print(pchMessage);
}

void CSerialHandler::ReplyOK(CMessageHeader *pMessage)
{
  m_pPort->println("@OK");
}

void CSerialHandler::ReplyError(const char *pchDescription)
{
  m_pPort->print("@ERR: ");
  m_pPort->println(pchDescription);
}

void CSerialHandler::ReplyOverflowError()
{
  ReplyError("overflow");
}

void CSerialHandler::ReplyInvalidMessageError()
{
  ReplyError("invalid message");
}