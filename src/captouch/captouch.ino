#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789 (NEEDS TO BE MODIFIED)
#include <Adafruit_FT6206.h> // Touch driver library (NEEDS TO BE MODIFIED)
#include <SPI.h>

/*
    This is a pretty scuffed drawing test program for the WT32-SC01 module.
    I couldn't get the sensitivity of the touch screen quite right, so the drawing
    is done with lines that connect between touch points (if the distance is small enough between the points)
*/

// Touch library
Adafruit_FT6206 ts = Adafruit_FT6206();

#define TFT_MISO    12
#define TFT_MOSI    13
#define TFT_SCLK    14
#define TFT_CS      15
#define TFT_DC      21
#define TFT_RST     22 // Or set to -1 and connect to Arduino RESET pin
#define TFT_BL      23

// HARDWARE SPI
SPIClass *hspi = new SPIClass(HSPI);
Adafruit_ST7789 tft = Adafruit_ST7789(hspi, TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
    Serial.begin(115200);

    Serial.println("INIT");

    // Initialize the touchscreen
    // begin(uint8_t thresh)
    if (!ts.begin(18)) {
        Serial.println("Unable to start touchscreen.");
    }else{
        Serial.println("Touchscreen started.");
    }

    // Set backlight as output
    pinMode(TFT_BL, OUTPUT);

    // Attach PWM channel 0 to the backlight pin from the internal PWM controller
    ledcAttachPin(TFT_BL, 0);

    // Channel 0, 1000 Hz, 8-bit resolution (0-255)
    ledcSetup(0, 1000, 8);

    // Set channel 0 duty cycle to 20 (~8% brightness)
    ledcWrite(0, 20);

    // Initialize display with 320x480 resolution
    // SPI-mode is set to 3
    tft.init(320, 480, 3);
    tft.fillScreen(ST77XX_BLACK);

    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_RED);
    tft.setTextWrap(true);
    tft.print("DOES THIS WORK");

    tft.setCursor(0, 10);
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextWrap(true);
    tft.print("DOES THIS WORK");

    tft.setCursor(0, 20);
    tft.setTextColor(ST77XX_RED);
    tft.setTextWrap(true);
    tft.print("DOES THIS WORK");

    tft.setCursor(0, 30);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.print("DOES THIS WORK");

    tft.drawRect(280, 0, 320, 30, ST77XX_WHITE);
    tft.drawRect(280, 440, 320, 480, ST77XX_WHITE);
}

uint16_t lastX = 0;
uint16_t lastY = 0;

void loop() {

    // If a touch is registered
    if (ts.touched()) {

        // Get only the first touch, however the module should support a second one
        TS_Point p = ts.getPoint(0);

        // Sometimes the display gives either 0,0 or 4096,4096 coordinates and they are usually not actual touches
        if (p.x == 0 and p.y == 0) {
            return;
        }

        // If the touch was 4 px away
        if (abs(lastX - p.x) > 4 || abs(lastY - p.y) > 4) {

            // And less than 40 px away, draw a line
            if (abs(lastX - p.x) < 40 || abs(lastY - p.y) < 40) {
                tft.drawLine(p.x, p.y, lastX, lastY, ST77XX_WHITE);
            }

            // Upper button
            if (p.x >= 280 && p.x <= 320 && p.y >= 0 && p.y <= 30) {
                tft.fillScreen(ST77XX_BLACK);
                tft.drawRect(280, 0, 320, 30, ST77XX_WHITE);
                tft.drawRect(280, 440, 320, 480, ST77XX_WHITE);
            }

            // Lower button
            if (p.x >= 280 && p.x <= 440 && p.y >= 320 && p.y <= 480) {
                tft.fillScreen(ST77XX_BLACK);
                tft.drawRect(280, 0, 320, 30, ST77XX_WHITE);
                tft.drawRect(280, 440, 320, 480, ST77XX_WHITE);

                tft.setCursor(0, 20);
                tft.setTextColor(ST77XX_RED);
                tft.setTextWrap(true);
                tft.print("CLEARED");
            }
        }

        // Save last touch coordinates
        lastX = p.x;
        lastY = p.y;
    }
}

void testdrawtext(char *text, uint16_t color) {
    tft.setCursor(0, 0);
    tft.setTextColor(color);
    tft.setTextWrap(true);
    tft.print(text);
}