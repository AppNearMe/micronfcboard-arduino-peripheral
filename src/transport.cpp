/*
 MicroNFCBoard Arduino API
 
 Copyright (c) 2014-2015 AppNearMe Ltd
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "transport.h"
#include <SPI.h>


//MSB first
#define WRITE_UINT32( addr, val ) do{ *(((uint8_t*)(addr)) + 0) = ((val) >> 24 ) & 0xFF; \
                                           *(((uint8_t*)(addr)) + 1) = ((val) >> 16 ) & 0xFF; \
                                           *(((uint8_t*)(addr)) + 2) = ((val) >> 8 ) & 0xFF; \
                                           *(((uint8_t*)(addr)) + 3) = ((val) >> 0 ) & 0xFF; } while(0)
#define WRITE_UINT16( addr, val ) do{ *(((uint8_t*)(addr)) + 0) = ((val) >> 8 ) & 0xFF; \
                                           *(((uint8_t*)(addr)) + 1) = ((val) >> 0 ) & 0xFF; } while(0)

//MSB first
#define READ_UINT32( addr, val ) do{ val = (*(((uint8_t*)(addr)) + 0) << 24 ) \
                                              | (*(((uint8_t*)(addr)) + 1) << 16 ) \
                                              | (*(((uint8_t*)(addr)) + 2) << 8 ) \
                                              | (*(((uint8_t*)(addr)) + 3) << 0 ); } while(0)
#define READ_UINT16( addr, val ) do{ val = (*(((uint8_t*)(addr)) + 0) << 8 ) \
                                              | (*(((uint8_t*)(addr)) + 1) << 0 ); } while(0)
#define INT_PIN 9
#define CS_PIN 10

Transport::Transport()
{
    pinMode(INT_PIN, INPUT);
    pinMode(CS_PIN, OUTPUT);
    SPI.begin();
}

void Transport::init()
{
  SPI.setDataMode(SPI_MODE1);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  digitalWrite(CS_PIN, 1);

  for(int i = 0; i < 64; i++)
  {
    digitalWrite(CS_PIN, 0);
    SPI.transfer(0);
    digitalWrite(CS_PIN, 1);
  }
}

void Transport::reset()
{
  uint8_t out[] = {0};
  command(Transport::RESET, out, sizeof(out), NULL, 0);
}

bool Transport::statusChanged()
{
  return (digitalRead(INT_PIN) != 0);
}

uint32_t Transport::status()
{
  uint8_t in[4];
  command(Transport::GET_STATUS, NULL, 0, in, sizeof(in));

  uint32_t status;
  READ_UINT32(&in[0], status);
  return status;
}

void Transport::nfcPoll(bool enable)
{
  uint8_t out[] = {enable?1:0};
  command(Transport::NFC_POLL, out, sizeof(out), NULL, 0);
}

void Transport::nfcOperation(bool readOp, bool writeOp)
{
  uint8_t out[1];
  if(readOp)
  {
    out[0] = 1;
  }
  else if(writeOp)
  {
    out[0] = 2;
  }
  else
  {
    out[0] = 0;
  }
  command(Transport::NFC_OPERATION, out, sizeof(out), NULL, 0);
}

void Transport::nfcGetInfoIsoA(uint8_t* atqa, uint8_t* sak, uint8_t* uid, size_t* pUidLength)
{
  uint8_t in[2 + 1 + 1 + 10];
  command(Transport::NFC_GET_INFO, NULL, 0, in, sizeof(in));
  memcpy(atqa, &in[0], 2);
  *sak = in[2];
  *pUidLength = in[3];
  memcpy(uid, &in[4], *pUidLength);
}

void Transport::nfcGetMessageInfo(size_t* pRecordCount)
{
  uint8_t in[2];
  command(Transport::NFC_GET_MESSAGE_INFO, NULL, 0, in, sizeof(in));
  READ_UINT16(&in[0], *pRecordCount);
}

void Transport::nfcSetMessageInfo(size_t recordCount)
{
  uint8_t out[2];
  WRITE_UINT16(&out[0], recordCount);
  command(Transport::NFC_SET_MESSAGE_INFO, out, sizeof(out), NULL, 0);
}

void Transport::nfcGetRecordInfo(size_t recordNumber, uint16_t* pType, uint16_t* info, size_t infoCount)
{
  uint8_t out[2];
  uint8_t in[2+2*infoCount];
  WRITE_UINT16(&out[0], recordNumber);
  command(Transport::NFC_GET_RECORD_INFO, out, sizeof(out), in, sizeof(in));
  READ_UINT16(&in[0], *pType);
  for(int i = 0; i < infoCount; i++)
  {
    READ_UINT16(&in[2+2*i], info[i]);
  }
}

void Transport::nfcSetRecordInfo(size_t recordNumber, uint16_t type, uint16_t* info, size_t infoCount)
{
  uint8_t out[2+2+2*infoCount];
  WRITE_UINT16(&out[0], recordNumber);
  WRITE_UINT16(&out[2], type);
  for(int i = 0; i < infoCount; i++)
  {
    WRITE_UINT16(&out[2+2+2*i], info[i]);
  }
  command(Transport::NFC_SET_RECORD_INFO, out, sizeof(out), NULL, 0);
}

void Transport::nfcGetRecordData(size_t recordNumber, size_t item, size_t offset, uint8_t* data, size_t length)
{
  uint8_t out[7];
  WRITE_UINT16(&out[0], recordNumber);
  out[2] = item;
  WRITE_UINT16(&out[3], offset);
  WRITE_UINT16(&out[5], length);
  command(Transport::NFC_GET_RECORD_DATA, out, sizeof(out), data, length);
}

void Transport::nfcSetRecordData(size_t recordNumber, size_t item, size_t offset, uint8_t* data, size_t length)
{
  uint8_t out[7+length];
  WRITE_UINT16(&out[0], recordNumber);
  out[2] = item;
  WRITE_UINT16(&out[3], offset);
  WRITE_UINT16(&out[5], length);
  memcpy(&out[7], data, length);
  command(Transport::NFC_SET_RECORD_DATA, out, sizeof(out), NULL, 0);
}

void Transport::nfcPrepareMessage(bool lock, bool generate)
{
  uint8_t out[1];
  if(lock)
  {
    out[0] = 1;
  }
  else if(generate)
  {
    out[0] = 2;
  }
  else
  {
    out[0] = 0;
  }
  command(Transport::NFC_POLL, out, sizeof(out), NULL, 0);
}

void Transport::nfcDecodePrefix(uint8_t prefix, char* data, size_t* pDataLength)
{
  uint8_t out[] = { prefix };
  uint8_t in[2 + 36]; //max prefix length is 36
  command(Transport::NFC_DECODE_PREFIX, out, sizeof(out), in, sizeof(in));
  size_t length;
  READ_UINT16(&in[0], length);
  if(length < *pDataLength)
  {
    *pDataLength = length;
  }
  memcpy(data, &in[2], *pDataLength);
}

void Transport::nfcEncodePrefix(uint8_t* pPrefix, char* data, size_t dataLength)
{
  uint8_t out[2 + dataLength];
  uint8_t in[1];

  WRITE_UINT16(&out[0], dataLength);
  memcpy(data, &out[2], dataLength);

  command(Transport::NFC_ENCODE_PREFIX, out, sizeof(out), in, sizeof(in));

  *pPrefix = in[0];
}

void Transport::leds(bool led1, bool led2)
{
  uint8_t out[] = {led1?1:0, led2?1:0};
  command(Transport::LEDS, out, sizeof(out), NULL, 0);
}

Transport::CommandError Transport::command(Transport::CommandCode command, uint8_t* outBuf, size_t outLength, uint8_t* inBuf, size_t inLength)
{
  digitalWrite(CS_PIN, 0);
  SPI.transfer((uint8_t)((outLength + 1) & 0xFF));
  digitalWrite(CS_PIN, 1);
    
  digitalWrite(CS_PIN, 0);
  SPI.transfer((uint8_t)(command & 0xFF));
  digitalWrite(CS_PIN, 1);
  for(int i = 0; i < outLength; i++)
  {
    digitalWrite(CS_PIN, 0);
    SPI.transfer(outBuf[i]);
    digitalWrite(CS_PIN, 1);
  }

  size_t length = 0;
  while(length == 0)
  {
    digitalWrite(CS_PIN, 0);
    length = SPI.transfer(0);
    digitalWrite(CS_PIN, 1);
  }

  digitalWrite(CS_PIN, 0);
  Transport::CommandCode retCmd = (Transport::CommandCode)SPI.transfer(0);
  length--;
  digitalWrite(CS_PIN, 1);

  digitalWrite(CS_PIN, 0);
  Transport::CommandError ret = (Transport::CommandError)SPI.transfer(0);
  length--;
  digitalWrite(CS_PIN, 1);

  for(int i = 0; i < length; i++)
  {
    digitalWrite(CS_PIN, 0);
    if(i < inLength)
    {
      inBuf[i] = SPI.transfer(0);
    }
    else
    {
      SPI.transfer(0);
    }
    digitalWrite(CS_PIN, 1);
  }

  return ret;
}
