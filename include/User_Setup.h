#ifndef USER_SETUP_H
#define USER_SETUP_H

#define USER_SETUP_LOADED

// Try GC9A01 driver - maybe this was working before
#define GC9A01_DRIVER

// Display size - CORRECTED for 1.14" TFT (240x135)
#define TFT_WIDTH  240
#define TFT_HEIGHT 135

// SPI pins
#define TFT_MOSI 35
#define TFT_SCLK 36
#define TFT_MISO -1
#define TFT_CS   42
#define TFT_DC   40
#define TFT_RST  41

// Backlight control
#define TFT_BL 45
#define TFT_BACKLITE 45  // Alias for main.cpp compatibility
#define TFT_BACKLIGHT_ON HIGH

// I2C Power (board pin 7) - REQUIRED for TFT to work
#define TFT_I2C_POWER 7  // Power supply pin for I2C and TFT

// SPI frequency - try different speeds
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 10000000

// SPI mode for ST7789 (needs mode 0)
#define TFT_SPI_MODE SPI_MODE0

#endif // USER_SETUP_H
