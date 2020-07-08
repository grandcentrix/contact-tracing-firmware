# contact-tracing-firmware
Firmware for the ESP32 (Olimex ESP32-POE-ISO) based project hardware

# Development Setup

- Install the current upstream Arduino IDE at the 1.8 level or later. The current version is at the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *esp32* platform (and don't forget to select OLIMEX ESP32-PoE-ISO from Tools > Board menu after installation).
- Go to Tools > Manage Libraries and install the following:
  - ArduinoHttpClient (tested with 0.4.0)
  - ESP32 AnalogWrite (tested with 0.1.0)
- Make sure to test if the code compiles by clicking Verify before making any changes.

# Creating OTA binaries
- Make sure to increase the VERSION constant by one.
- Use Sketch > Export compiled binary.
