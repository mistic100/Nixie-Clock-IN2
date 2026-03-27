# Nixie Clock IN-2

Check out my other projects:
- [Nixie Clock IN14](https://github.com/mistic100/Nixie-Clock-IN14)
- [Nixie Clock IN12](https://github.com/mistic100/Nixie-Clock-IN12)
- [IV-27 VFD Clock](https://github.com/mistic100/IV-27-Clock)

This is a Nixie Clock using small [IN-2 tubes](https://www.tube-tester.com/sites/nixie/data/I/IN-2/in-2.htm) and a 16x9 LED matrix. It was mostly inspired by this 3D model [80s Alarm Clock by Tom Seddon](https://sketchfab.com/3d-models/80s-alarm-clock-36aaecc458bc4e3ea907cdb9d538c9f1).

### Key features

- 3 buttons and 1 rotary encoder
- Zigbee connectivity
  - receive date-time
  - receive weather
  - send room temperature and humidity
  - can be toggled on/off
  - 3 generic buttons send click, double click, long click events
- small LED matrix with multiple screens
  - scrolling date
  - temperature
  - falling sand animation
  - Game of Life animation
  - fire animation
  - Matrix animation
  - rain animation
- possibility to use a DS3231 RTC module (routed on the PCB but not available in current firmware)
  

### BOM

- ESP32-C6 SuperMini
- Adafruit Charlieplexed 16x9 LED matrix + IS31FL3731 driver
- 4x IN-2 Nixie tubes
- 2x INS-1 Nixie indicators
- Microchip HV5222 serial-to-parallel converter
- Adafruit 4 channels TTL level-shifter
- 4x APA106 5mm adressable LEDs
- BME280 ambient sensor
- EC11 rotary encoder
- 3x 6x6x4.3mm push buttons
- MPSA42 high voltage NPN transistor
- USB-C connector
- resistors, capacitors, diodes...
