/*
MicroNFCBoard mbed API

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

#include <micronfcboard.h>
#include <SPI.h>

MicroNFCBoard nfc;

void setup() {
  Serial.begin(9600);
  nfc.init();
  bool b = true;
  for(int i = 0; i < 10; i++)
  {
      nfc.setLeds(b, !b);
      delay(200);
      b = !b;
  }
}

void loop() {
  Serial.println(F("Poll"));
  nfc.startPolling();

  while( nfc.polling() );

  if( nfc.type2() )
  {
     Serial.println(F("Connected to type 2 tag"));
  }
  else if( nfc.p2p() )
  {
        Serial.println(F("Connected in P2P mode"));
  }

  bool ndefRead = false;
  bool ndefReadingStarted = false;
  while( nfc.connected() )
  {
      if( !ndefReadingStarted && nfc.ndefReadable() )
      {
        Serial.println(F("Reading tag"));
        ndefReadingStarted = true;
        nfc.ndefRead();
      }
      if( !ndefRead && nfc.ndefPresent() )
      {
          Serial.println(F("Got message"));
          char buf[512];
          if( nfc.readNdefUri(buf, sizeof(buf)) )
          {
            Serial.print(F("Got URI: "));
            Serial.println(buf);
          }
          if( nfc.readNdefText(buf, sizeof(buf)) )
          {
            Serial.print(F("Got Text: "));
            Serial.println(buf);
          }
          ndefRead = true;
      }
  }

  Serial.println(F("Disconnected"));
}
