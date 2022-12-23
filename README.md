# OpenThermostat

OpenThermostat is a simple, open-source thermostat for your home. It is designed to be easy to build, easy to use, and easy to modify. It is based on the ESP8266 microcontroller, and uses an OLED or text LCD display to show the current temperature and setpoint. It also has a rotary encoder to adjust the setpoint.

This project does not use Arduino IDE. Instead, it uses PlatformIO, which is a cross-platform IDE for embedded development. It is available for Windows, Mac, and Linux, and is available as a plugin for VSCode. It is also available as a standalone application.

:warning: **This project is still in development. It is not yet ready for use.** :warning:

## Features
 - [x] Temperature sensing
 - [x] WiFi connectivity with captive portal to configure
 - [x] Web interface
 - [x] Low power consumption
  > :warning: **ESP-01 need a bridge to use deep-sleep for better energy saving but you can use light sleep if you don't want to modify your module**
  Here are instructions to enable deep sleep on ESP-01:
  https://www.instructables.com/Enable-DeepSleep-on-an-ESP8266-01/
 - [x] OTA updates
  > :warning: **OTA need at least 2Mb of flash so you need to solder a bigger flash module**
 - [ ] Relay control to turn on/off heating/cooling
 - [ ] Hub to connect to other devices
 - [ ] HomeKit support
 - [ ] Alexa support
 - [ ] Google Assistant support
 - [ ] MQTT support