# ESP32 Precision Temperature Sensor
*This is a repo for my own testing. The actual one will not be in this stupid name.*

## Hardware
![PXL_20220722_023616_jrymk](https://user-images.githubusercontent.com/39593345/180464413-bfded315-408f-4266-831b-97addb62acad.jpg)

The board is a Firebeetle 2 from DFRobot using the newer ESP-WROOM-32E microcontroller. I used this one because it has a built-in lipo charger/boost circuit so it would be ideal for mobile projects, although I did not fit a battery in this project for now.

The display is a 2.9 inch Waveshare 2 color e-paper display. 3 colors ones refreshes for 15 seconds and does not support partial update, while this black and white one can do it and allows me to update it every second.

#### Sensors
* SHT31 temperature and humidity sensor
it isn't like really "precision" or anything. it's really just called that because it is better than the stupid cheap DHT sensors.
* Senseair S8 LP
this is a infrared CO2 sensor. I bought it for a [portable CO2 monitor doodad project](https://github.com/jrymk/portable-CO2-monitor-doodad) but I kinda got distracted and made this instead.

There is also a 3cm DC fan that runs constantly on 5V because due to some reason (RF heating or from the ground?) the sensor is reading significantly higher temperature. I found out that pointing a fan to it fixes it. So the current design just has a fan blowing through the CO2 sensor and the temperature/humidity sensor.

Also there is 3 soft silicone buttons, and a SD card slot. No LEDs though, probably will add one in the future. One problem is that I don't really have enough pins, either the Firebeetle 2 board didn't break it out, I don't know I haven't look into it. I did already broken out the 27 on board button pin because I won't need it at all.

![IMG_20220722_002310_055](https://user-images.githubusercontent.com/39593345/180467587-9c9554e2-1f45-4c8f-a571-d43a411909c1.jpg)
