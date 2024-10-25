# A WIP implementation of the KC868-A16 board in C/C++ using esp-idf

WIP

## Features

- None

## Components

**EthernetManager**: Ethernet stuff
**I2CManager**: I2C related
**PC8574**: PC8584 multiplexer conf
**UartManager**: Uart stuff
**WiFiManager**: Manages Wi-Fi connectivity.
**TODO: Web Server**: TODO

## Building and Flashing

This project uses the ESP-IDF framework. To build and flash the project:

1. Set up the ESP-IDF environment.
2. Navigate to the project directory.
3. Run `idf.py build` to build the project.
4. Run `idf.py -p (PORT) flash` to flash the ESP32, replacing (PORT) with your device's port.



## TODO

* I/O stuff
* Interlock functionality

## License

This project is licensed under the GNU General Public License v3.0 License. See the LICENSE file for details.
