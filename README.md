# Web Accessable Temperature gague /w OLED readout

Master: [![Build Status](https://travis-ci.org/Findarato/Feather-WiFi-OLED-Temp-Monitor.svg?branch=master)](https://travis-ci.org/Findarato/Feather-WiFi-OLED-Temp-Monitor) [![codecov](https://codecov.io/gh/Findarato/Feather-WiFi-OLED-Temp-Monitor/branch/master/graph/badge.svg)](https://codecov.io/gh/Findarato/Feather-WiFi-OLED-Temp-Monitor) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/650fb27b1893410a838bc3575aedde91)](https://www.codacy.com/app/Findarato/Feather-WiFi-OLED-Temp-Monitor?utm_source=github.com&utm_medium=referral&utm_content=Findarato/Feather-WiFi-OLED-Temp-Monitor&utm_campaign=Badge_Grade)

### Branches
PlatformIO: [![Build Status](https://travis-ci.org/Findarato/Feather-WiFi-OLED-Temp-Monitor.svg?branch=PlatformIO)](https://travis-ci.org/Findarato/Feather-WiFi-OLED-Temp-Monitor) 

No-OLED: [![Build Status](https://travis-ci.org/Findarato/Feather-WiFi-OLED-Temp-Monitor.svg?branch=master)](https://travis-ci.org/Findarato/Feather-WiFi-OLED-Temp-Monitor) 


## Now with Toggle switches for OLED version

A :radio_button: Shows IP<br>
B :radio_button: Shows Temperature

There are a few libraries you will need, and they are listed below. Ensure they are in your Arduino libraries folder and you should be able to compile the file.

If your into using the terminal

```bash
platformio ci --project-conf=platformio.ini ./src
```
Mainly setup for PlatformIO building.  Could still be used with arduino, but libraries would have to manually be installed

Libraries Needed to be installed

:octopus: [AdaFruitGFX][432f0407]

:octopus: [ArduinoJSON][92f91ab3]

:octopus: [AdaFruit SSD1306][e13d6d0d]

:octopus: [AdaFruit DHT Sensor Library][b4a05a48]

:octopus: [Adafruit Unified Sensor Driver][b47100f1]

[432f0407]: https://github.com/adafruit/Adafruit-GFX-Library "Github"
[92f91ab3]: https://github.com/bblanchon/ArduinoJson "Github"
[b47100f1]: https://github.com/adafruit/Adafruit_Sensor "Github"
[b4a05a48]: https://github.com/adafruit/DHT-sensor-library "Github"
[e13d6d0d]: https://github.com/adafruit/Adafruit_SSD1306 "Github"
