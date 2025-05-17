// Define the driver for the display
#define ILI9341_DRIVER // Generic driver for common displays
//#define ILI9341_2_DRIVER // Alternative driver, commented out for stability

// Set the resolution for the display
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// Define the backlight pin and its state
#define TFT_BL   21
#define TFT_BACKLIGHT_ON HIGH

// Define the pins for the ESP32 setup
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

// Define the touch screen chip select pin
#define TOUCH_CS 33

// Set the SPI frequencies
#define SPI_FREQUENCY  40000000 // Reduced from 55 MHz to 40 MHz for stability
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// Use HSPI port for the ESP32
#define USE_HSPI_PORT

// Enable smooth fonts
#define SMOOTH_FONT

// Enable necessary fonts
#define LOAD_GLCD   // Font 1
#define LOAD_FONT2  // Font 2
#define LOAD_FONT4  // Font 4
#define LOAD_FONT6  // Font 6
#define LOAD_FONT7  // Font 7
#define LOAD_FONT8  // Font 8
#define LOAD_GFXFF  // FreeFonts