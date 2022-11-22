The WT32-SC01 module is an ESP32 dev board which has a capacitive touch screen mounted on the same PCB.
I have been looking for a capacitive touch screen dev board for a while and now I finally found one, so I might as well document everything here
in case someone else finds the same module.

<p align="middle">
  <img src="Images/WT32-SC01-BACK.jpeg" width="60%" title="Front side of the module"/>
  <img src="Images/WT32-SC01-Front.jpeg" width="24%" title="Front side of the module"/>
</p>

The documentation on this module is quite bad, but luckily there is already a couple of projects, so we don't have to reverse engineer the entire thing.

### Parts
Okay so on paper this display has a lot of good parts. I will try to get the most important components on this list:
* ESP32-WROVER-B | Main MCU
* CP2104 (SIL2104) | USB <-> UART Bridge
* FT6336U | Touch panel controller
* ST77xx -based display driver (might be ST7796S?)
* 320x480 display
* 2 pcs of 2x20 connectors with the frickin 2.0mm pitch, glhf getting anything plugged into that. Luckily both of them are SMD connectors with quite long leads so soldering wires to them shouldn't be that hard.

Key notes:
* This module DOES NOT HAVE any sort of battery management circuitry, so you have to implement it by yourself if you want to make this battery powered.
* There is a [PDF of this module](http://myosuploads3.banggood.com/products/20220111/20220111020446WT32-SC01DataSheetV3.3.pdf), but there is not a lot in there. At least the pinouts of the connectors are there.

### Pinout
This list might be incomplete, but I'll add more pins along the way
| Arduino IDE pin   | WT32-SC01 Display |
| ------------- | ------------- |
| 12 | TFT_MISO |
| 13  | TFT_MOSI |
| 14 | TFT_SCLK |
| 15  | TFT_CS |
| 21  | TFT_DC |
| 22 | TFT_RST |
| 23  | TFT_BACKLIGHT |

| Arduino IDE pin   | WT32-SC01 Touch controller |
| ------------- | ------------- |
| 19 | I2C_SCL |
| 18  | I2C_SDA |

NOTE: To be able to use the I2C-interface on these pins you must use
```Wire.begin(18,19);``` to be able to get it working. Most of the libraries seem to be using only ```Wire.begin();``` which didn't work for me, and I'd guess that the default I2C pins might be different.
For example if you want to use Adafruit_FT6206 you have to change the ```Wire.begin();``` in ```Adafruit_FT6206.cpp``` on line 57 to ```Wire.begin(18,19);```

### Display driver
To get the display working, I used the Adafruits [Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library). This library has support for the ST7735 as well as ST7789, which very similar to the ST7796S driver which we might have onboard. They are basically the same driver, but the colors are inversed and the color order is different.

For now I'll list all of the modifications needed for the ```Adafruit_ST7789.cpp``` that is included on the Adafruit-ST7735-Library:
Most of the modifications includes a resolution change or configuration change based on the [datasheet of the ST7796S](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf).
1. ROW 35 -> ``` 	Adafruit_ST7789::Adafruit_ST7789(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst) : Adafruit_ST77xx(320, 480, spiClass, cs, dc, rst) {} ```
* display width and height fix
2. ROW 61 -> ``` 0b01001010, ```
* MADCTL, Memory Data Access Control, Datasheet page 183
* Fix mirrored image & color order
3. ROW 66 -> ``` 320 >> 8, ```
* CASET, Column Address Set MSB, Datasheet page 170
4. ROW 67 -> ``` 320&0xFF, ```
* CASET, Column Address Set LSB, Datasheet page 170
5. ROW 71 -> ``` 480>>8, ```
* RASET, Row Address Set MSB, Datasheet page 172
6. ROW 72 -> ``` 480&0xFF, ```
* RASET, Row Address Set LSB, Datasheet page 172
7. ROW 73 -> ``` ST77XX_INVOFF, ST_CMD_DELAY, ```
* Display inversion OFF, Datasheet page 165
8. ROW 74 -> ``` 0b00100000, ```
* INVOFF CMD parameters, specified on page 165
9. ROW 121 -> ``` _rowstart = _rowstart2 = (int)((480 - height) / 2); ```
* Fix row start position
10. ROW 122 -> ``` _colstart = _colstart2 = (int)((320 - width) / 2); ```
* Fix column start position
11. ROW 129 -> ``` //setRotation(0) ```
* Disable setRotation, because it doesn't have the proper bits set.
