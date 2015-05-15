# MicroNFCBoard: Arduino API
Arduino API for MicroNFCBoard

![MicroNFCBoard + Arduino][micronfcboard-arduino]

# Features
## NFC technologies
* SNEP client (send NDEF messages via P2P)
* SNEP server (receive NDEF messages via P2P)
* Tag writer type 2 (encode tags with NDEF messages)
* Tag reader type 2 (decode NDEF messages from tags)
* Tag emulator type 4
* ... any combination

## NDEF records
* URI record
* Text record
* Smart poster record
* MIME record

# License
This code is licensed under the Apache 2.0 License:
http://www.apache.org/licenses/LICENSE-2.0

# Getting started

## Flashing the firmware

Plug in a USB cable into your MicroNFCBoard. Press the ```Bootloader``` button (right) and hold it while inserting the USB cable in your computer. The board should appear as an USB mass storage device with the label ```CRP DISABLD```. If you are upgrading the firmware, you can also hold the ```Bootloader``` button while pressing the ```Reset``` button and then releasing it.

Download the current firmware (version 1.5): http://dev.appnearme.com/static/micronfcboard/fw/firmware-MICRONFCBOARD-d4f6aecb7316.bin

### Windows
Open the ```CRP DISABLD``` drive, erase the ```firmware.bin``` file and drag and drop the firmware file into the disk. Once done, press the ```Reset``` button (left) for a second and release it.

On Windows, you will need to use this file for proper installation of the serial driver:
http://dev.appnearme.com/static/micronfcboard/drivers/micronfcboard_serial.inf

### Mac OS X
Using the terminal, erase the ```firmware.bin``` file from the ```CRP DISABLD``` drive (usually ```/Volumes/CRP\ DISABLD```) and copy the firmware file to the disk. Once done, press the ```Reset``` button (left) for a second and release it.

### Linux
Flashing the firmware using the above methods might not work. If so, use the following command:
```shell
umount /dev/sdX
dd if=firmware-MICRONFCBOARD-*.bin of=/dev/sdX seek=4
```
Replace ```/dev/sdX``` with the correct drive path. Make sure to select the correct drive to avoid losing data!

## Wiring it up

The MicroNFCBoard acts as a SPI slave. The MicroNFCBoard's USB cable can be unplugged now.

| Arduino pin 	| MicroNFCBoard pin 	|
|-------------	|-------------------	|
| 5V           	| VIN               	|
| GND         	| GND               	|

| Arduino pin 	| MicroNFCBoard pin 	|
|-------------	|-------------------	|
| D13         	| SCK               	|
| D12         	| MISO              	|
| D11         	| MOSI              	|
| D10         	| CS               	  |
| D9          	| IRQ               	|

## Running the examples
We have put together a few Arduino projects in the ```examples/``` directory.

### Basic
* The ```blink.ino``` example will blink the board's LEDs a few times.

### Tags
* The ```tag_reader.ino``` example will poll for tags and display a tag's UID, decoding a NDEF message if available.
* The ```tag_writer.ino``` example will poll for tags and display a tag's UID, and then write a NDEF message to the tag.

### P2P
* The ```p2p_server.ino``` example will poll for peers and wait for the peer to push a NDEF message (works with a NFC  phone/tablet or another MicroNFCBoard).
* The ```p2p_client.ino``` example will poll for peers and push a NDEF message to the peer (works with a NFC  phone/tablet or another MicroNFCBoard).

### Tag emulation
* The ```tag_emulator.ino``` example will encode a NDEF message, poll for initiators and decode a new NDEF message if written to the emulated tag.

### Working with multiple NFC technologies
All the examples above can be combined as you wish. It should be noted though that most NFC phones will favour handling P2P over reading a tag if you are emulating a tag and using P2P.

#### Typical combinations
* Tag reader/writer + P2P
* Tag reader/writer + Tag emulation
* Tag reader/writer + P2P (for phones) + Tag emulation (for "legacy" readers)
 
# Issues, bugs and feature requests
Please use GitHub's issues platform: https://github.com/AppNearMe/micronfcboard-arduino-peripheral/issues

# Work in progress
* AppNearMe mode
* Type 1, 3 and 4 reader/writer

[micronfcboard-arduino]: http://appnearme.github.io/micronfcboard/doc/img/micronfcboard-arduino.png

