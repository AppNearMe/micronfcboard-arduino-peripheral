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

#ifndef SRC_TRANSPORT_H_
#define SRC_TRANSPORT_H_

#include "Arduino.h"

class Transport
{
public:
  Transport();

  void init();

  void reset();

  bool statusChanged();

  uint32_t status();

  void nfcPoll(bool enable);

  void nfcOperation(bool readOp, bool writeOp);

  void nfcGetInfoIsoA(uint8_t* atqa, uint8_t* sak, uint8_t* uid, size_t* pUidLength);

  void nfcGetMessageInfo(size_t* pRecordCount);

  void nfcSetMessageInfo(size_t recordCount);

  void nfcGetRecordInfo(size_t recordNumber, uint16_t* pType, uint16_t* info, size_t infoCount);

  void nfcSetRecordInfo(size_t recordNumber, uint16_t type, uint16_t* info, size_t infoCount);

  void nfcGetRecordData(size_t recordNumber, size_t item, size_t offset, uint8_t* data, size_t length);

  void nfcSetRecordData(size_t recordNumber, size_t item, size_t offset, uint8_t* data, size_t length);

  void nfcPrepareMessage(bool lock, bool generate);

  void nfcDecodePrefix(uint8_t prefix, char* data, size_t* pDataLength);

  void nfcEncodePrefix(uint8_t* pPrefix, char* data, size_t dataLength);

  void leds(bool led1, bool led2);

protected:
  enum CommandError
  {
    OK                  = 0,
    ERR_LENGTH          = 1,
    ERR_EMPTY           = 2,
    ERR_UNKNOWN_COMMAND = 3,
    ERR_TIMEOUT         = 4,
    ERR_PARAMS          = 5,
    ERR_STATUS          = 6,
  };

  enum CommandCode
  {
    GET_STATUS = 0,
    INFO = 1,
    RESET = 2,
    LEDS = 3,
    NFC_POLL = 4,
    NFC_OPERATION = 5,
    NFC_GET_INFO = 6,
    NFC_GET_MESSAGE_INFO = 7,
    NFC_GET_RECORD_INFO = 8,
    NFC_GET_RECORD_DATA = 9,
    NFC_SET_MESSAGE_INFO = 10,
    NFC_SET_RECORD_INFO = 11,
    NFC_SET_RECORD_DATA = 12,
    NFC_PREPARE_MESSAGE = 13,
    NFC_DECODE_PREFIX = 14,
    NFC_ENCODE_PREFIX = 15
  };

  CommandError command(CommandCode command, uint8_t* outBuf, size_t outLength, uint8_t* inBuf, size_t inLength);
};



#endif /* SRC_TRANSPORT_H_ */
