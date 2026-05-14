#ifndef LEDMATRIX_FONT_H
#define LEDMATRIX_FONT_H

#define LEDMATRIX_CHAR_X_LEN 8u
#define LEDMATRIX_CHAR_Y_LEN 8u

#define LEDMATRIX_SMALL_CHAR_X_LEN 3u
#define LEDMATRIX_SMALL_CHAR_Y_LEN 5u

enum ledmatrix_icons {
  LEDMATRIX_ICON_EMPTY = 10,
  LEDMATRIX_ICON_COLON,
  LEDMATRIX_ICON_DOT,
  LEDMATRIX_ICON_CELSIUS,
  LEDMATRIX_ICON_SNOW = 14,
  LEDMATRIX_ICON_SLEET,
  LEDMATRIX_ICON_MIST,
  LEDMATRIX_ICON_THUNDERSTORM,
  LEDMATRIX_ICON_HEAVY_RAIN,
  LEDMATRIX_ICON_LIGHT_RAIN,
  LEDMATRIX_ICON_SHOWERS,
  LEDMATRIX_ICON_HEAVY_CLOUD,
  LEDMATRIX_ICON_LIGHT_CLOUD,
  LEDMATRIX_ICON_CLEAR,
  LEDMATRIX_ICON_CLEAR_NIGHT,
  LEDMATRIX_ICON_AM,
  LEDMATRIX_ICON_PM,
  LEDMATRIX_ICON_WIFI,
  LEDMATRIX_ICON_UP,
  LEDMATRIX_ICON_WEATHER,
  LEDMATRIX_ICON_DATE,
  LEDMATRIX_ICON_BACK,
  LEDMATRIX_ICON_SNAKE,
  LEDMATRIX_ICON_INFO,
  LEDMATRIX_ICON_BRIGHTNESS,
  LEDMATRIX_ICON_ALARM,
  LEDMATRIX_ICON_TETRIS,
  LEDMATRIX_ICON_SOUND,
  LEDMATRIX_ICON_YES,
  LEDMATRIX_ICON_NO,
  LEDMATRIX_ICON_DASH,
  LEDMATRIX_ICON_CLOCK,
  LEDMATRIX_ICON_LEFT_ARROW,
  LEDMATRIX_ICON_RIGHT_ARROW,
};

enum ledmatrix_small_icons {
  LEDMATRIX_SMALL_ICON_COLON = 10,
  LEDMATRIX_SMALL_ICON_CELSIUS,
  LEDMATRIX_SMALL_ICON_DASH,
};

const byte ledmatrix_number_font[][8] = {
  { // 0
    0b01110000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b01110000
  },
  { // 1
    0b00100000,
    0b01100000,
    0b00100000,
    0b00100000,
    0b00100000,
    0b00100000,
    0b00100000,
    0b01110000
  }, { // 2
    0b01110000,
    0b10001000,
    0b00001000,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b11111000
  }, { // 3
    0b01110000,
    0b10001000,
    0b00001000,
    0b00110000,
    0b00001000,
    0b00001000,
    0b10001000,
    0b01110000
  }, { // 4
    0b00001000,
    0b00011000,
    0b00101000,
    0b01001000,
    0b10001000,
    0b11111000,
    0b00001000,
    0b00001000
  }, { // 5
    0b11111000,
    0b10000000,
    0b10000000,
    0b11110000,
    0b00001000,
    0b00001000,
    0b10001000,
    0b01110000
  }, { // 6
    0b01110000,
    0b10001000,
    0b10000000,
    0b11110000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b01110000
  }, { // 7
    0b11111000,
    0b00001000,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b01000000,
    0b01000000
  }, { // 8
    0b01110000,
    0b10001000,
    0b10001000,
    0b01110000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b01110000
  }, { // 9
    0b01110000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b01111000,
    0b00001000,
    0b10001000,
    0b01110000
  }, { // empty
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { // colon
    0b00000000,
    0b00000000,
    0b00000000,
    0b01000000,
    0b00000000,
    0b00000000,
    0b01000000,
    0b00000000
  }, { // dot
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01000000
  }, { // celsius
    0b11100000,
    0b10100000,
    0b11100000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { // snow (14)
    0b01111110,
    0b10000001,
    0b10000001,
    0b01111110,
    0b00000000,
    0b01010101,
    0b00100010,
    0b01010101
  }, { // sleet
    0b01111110,
    0b10000001,
    0b10000001,
    0b01111110,
    0b00000000,
    0b01010101,
    0b00100101,
    0b01010101
  }, { // mist
    0b01111110,
    0b10000001,
    0b10000001,
    0b01111110,
    0b00000000,
    0b01110111,
    0b00000000,
    0b01110111
  }, { // thunderstorms
    0b01111110,
    0b10000001,
    0b10000001,
    0b01111110,
    0b00000100,
    0b01010010,
    0b01010100,
    0b01010010
  }, { // heavy rain
    0b01111110,
    0b10000001,
    0b10000001,
    0b01111110,
    0b00000000,
    0b01010100,
    0b01010100,
    0b01010100
  }, { // light rain
    0b01111110,
    0b10000001,
    0b10000001,
    0b01111110,
    0b00000000,
    0b00101000,
    0b00000000,
    0b00101000
  }, { // showers
    0b01110000,
    0b10001001,
    0b10001010,
    0b01110010,
    0b00000001,
    0b01010000,
    0b00000000,
    0b01010000
  }, { // heavy cloud
    0b01111000,
    0b10000100,
    0b10000110,
    0b10000101,
    0b01111001,
    0b00001110,
    0b00000000,
    0b00000000
  }, { // light cloud
    0b01110000,
    0b10001001,
    0b10001010,
    0b01110010,
    0b00000001,
    0b00000000,
    0b00000000,
    0b00000000
  }, { // sunny
    0b00000000,
    0b00010000,
    0b01000100,
    0b00111000,
    0b10101010,
    0b00111000,
    0b01000100,
    0b00010000
  }, { // Clear night
    0b00000000,
    0b01111000,
    0b11100000,
    0b11000000,
    0b11000000,
    0b11100000,
    0b01111000,
    0b00000000
  }, { // AM
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11100000,
    0b10100000,
    0b11100000,
    0b10100000
  }, { // PM
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11100000,
    0b10100000,
    0b11100000,
    0b10000000
  }, { // WiFi icon
    0b00000000,
    0b01111110,
    0b10000001,
    0b00111100,
    0b01000010,
    0b00011000,
    0b00000000,
    0b00011000
  }, { // Up icon
    0b00010000,
    0b00111000,
    0b01111100,
    0b11111110,
    0b00111000,
    0b00111000,
    0b00111000,
    0b00111000
  }, { // Weather icon
    0b00000000,
    0b01100000,
    0b01100110,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00000110,
    0b00000000
  }, { // Date icon
    0b01110110,
    0b00000000,
    0b01110010,
    0b00010110,
    0b00110010,
    0b00010010,
    0b01110010,
    0b00000000
  }, { // Back icon
    0b00000000,
    0b00000100,
    0b00111110,
    0b01000100,
    0b01000000,
    0b01000000,
    0b00111100,
    0b00000000
  }, { // Snake icon
    0b00000000,
    0b01110010,
    0b01010000,
    0b01010010,
    0b01010010,
    0b01010010,
    0b01011110,
    0b00000000
  }, { // Info icon
    0b00000000,
    0b00011000,
    0b00000000,
    0b00111000,
    0b00011000,
    0b00011000,
    0b00111100,
    0b00000000
  }, { // Brightness icon
    0b00000000,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01111110,
    0b00011000,
    0b00111100,
    0b00000000
  }, { // Alarm icon
    0b00000000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b01000010,
    0b01111110,
    0b00011000,
    0b00000000
  }, { // Tetris icon
    0b00000000,
    0b00000000,
    0b01111110,
    0b01111110,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00000000
  }, { // Sound icon
    0b00000000,
    0b00010000,
    0b00011000,
    0b00010100,
    0b00010100,
    0b00110000,
    0b00110000,
    0b00000000
  }, { // Yes icon
    0b00000000,
    0b00000010,
    0b00000110,
    0b00001100,
    0b01011000,
    0b01110000,
    0b00100000,
    0b00000000
  }, { // No icon
    0b00000000,
    0b01100110,
    0b00111100,
    0b00011000,
    0b00011000,
    0b00111100,
    0b01100110,
    0b00000000
  }, { // Dash icon
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { // Menu clock icon
    0b00000000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00010000,
    0b00001000,
    0b00000100,
    0b00000000
  }, { // Left arrow icon
    0b00000000,
    0b00010000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b01100000,
    0b00110000,
    0b00010000
  }, { // Right arrow icon
    0b00000000,
    0b00001000,
    0b00001100,
    0b00000110,
    0b00000011,
    0b00000110,
    0b00001100,
    0b00001000
  }
};

const int ledmatrix_number_font_len = sizeof(ledmatrix_number_font) / 8;

const byte ledmatrix_small_number_font[][5] = {
  { // 0
    0b00000111,
    0b00000101,
    0b00000101,
    0b00000101,
    0b00000111
  }, { // 1
    0b00000001,
    0b00000011,
    0b00000001,
    0b00000001,
    0b00000001
  }, { // 2
    0b00000111,
    0b00000001,
    0b00000111,
    0b00000100,
    0b00000111
  }, { // 3
    0b00000111,
    0b00000001,
    0b00000011,
    0b00000001,
    0b00000111
  }, { // 4
    0b00000101,
    0b00000101,
    0b00000111,
    0b00000001,
    0b00000001
  }, { // 5
    0b00000111,
    0b00000100,
    0b00000111,
    0b00000001,
    0b00000111
  }, { // 6
    0b00000111,
    0b00000100,
    0b00000111,
    0b00000101,
    0b00000111
  }, { // 7
    0b00000111,
    0b00000001,
    0b00000001,
    0b00000001,
    0b00000001
  }, { // 8
    0b00000111,
    0b00000101,
    0b00000111,
    0b00000101,
    0b00000111
  }, { // 9
    0b00000111,
    0b00000101,
    0b00000111,
    0b00000001,
    0b00000111
  }, { // colon
    0b00000000,
    0b00000010,
    0b00000000,
    0b00000010,
    0b00000000
  }, { // celsius
    0b00000010,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { // dash
    0b00000000,
    0b00000000,
    0b00000111,
    0b00000000,
    0b00000000
  }
};

const int ledmatrix_small_number_font_len = sizeof(ledmatrix_small_number_font) / 5;

/*
   Well, well - the size of the first dimension of this array requires some explanation.
   This may be a complier or linker bug.
   If I leave the size blank, it will mess up the webserver HTML template and cause an exception, which is not related at all.
   The only relation is that they are linked after each other in irom.text:

   .irom.text.ledmatrix_font.cpp.344.0
                0x0000000040252668      0x3d5 /tmp/arduino-sketch-46FE38A118D66C7BE720FD8E76F0FED5/sketch/src/ledmatrix_font/ledmatrix_font.cpp.o
   .irom.text.webserver.cpp.8.2
                0x0000000040252a3d      0x7d4 /tmp/arduino-sketch-46FE38A118D66C7BE720FD8E76F0FED5/sketch/src/webserver/webserver.cpp.o

   The exception I get is:

   Exception 3: LoadStoreError: Processor internal physical address or data error during load or store
   PC: 0x4000bf64
   EXCVADDR: 0x4025291d
   0x40209ccc: Webserver::task() at /tmp/arduino_build_407489/sketch/src/webserver/webserver.cpp line 205

   If I give it the exact size (eg. 109) it will still be messed up, however if I decrease the size by one (by deleting an initializer) to 108, it will work as blank.

   So, in order to work this around, I've given it a much larger size, than it really is. (it only takes up flash, which we have plenty of)
   Be careful when adding new elements, always make sure that the array is bigger than it's contents!

 */

const PROGMEM uint8_t ledmatrix_letter_font[150][9] = {
  { // uppercase letters
    2,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11111100,
    0b11001100,
    0b11001100,
    0b11001100
  }, {
    6,
    0b00000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11111000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11111000
  }, {
    6,
    0b00000000,
    0b11111100,
    0b11000000,
    0b11000000,
    0b11111000,
    0b11000000,
    0b11000000,
    0b11111100
  }, {
    6,
    0b00000000,
    0b11111100,
    0b11000000,
    0b11000000,
    0b11111000,
    0b11000000,
    0b11000000,
    0b11000000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11000000,
    0b11000000,
    0b11011100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11111100,
    0b11001100,
    0b11001100,
    0b11001100
  }, {
    4,
    0b00000000,
    0b11110000,
    0b01100000,
    0b01100000,
    0b01100000,
    0b01100000,
    0b01100000,
    0b11110000
  }, {
    6,
    0b00000000,
    0b00111100,
    0b00011000,
    0b00011000,
    0b00011000,
    0b11011000,
    0b11011000,
    0b01110000
  }, {
    6,
    0b00000000,
    0b11001100,
    0b11011000,
    0b11110000,
    0b11100000,
    0b11110000,
    0b11011000,
    0b11001100
  }, {
    6,
    0b00000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11111100
  }, {
    7,
    0b00000000,
    0b11000110,
    0b11101110,
    0b11111110,
    0b11010110,
    0b11000110,
    0b11000110,
    0b11000110
  }, {
    7,
    0b00000000,
    0b11000110,
    0b11100110,
    0b11110110,
    0b11011110,
    0b11001110,
    0b11000110,
    0b11000110
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11111000,
    0b11000000,
    0b11000000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11011100,
    0b01111000,
    0b00001100
  }, {
    6,
    0b00000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11111000,
    0b11110000,
    0b11011000,
    0b11001100
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11000000,
    0b01111000,
    0b00001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b11111100,
    0b10110100,
    0b00110000,
    0b00110000,
    0b00110000,
    0b00110000,
    0b00110000
  }, {
    6,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111100
  }, {
    6,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000,
    0b00110000
  }, {
    7,
    0b00000000,
    0b11000110,
    0b11000110,
    0b11000110,
    0b11010110,
    0b11111110,
    0b11101110,
    0b11000110
  }, {
    7,
    0b00000000,
    0b11000110,
    0b11000110,
    0b01101100,
    0b00111000,
    0b01101100,
    0b11000110,
    0b11000110
  }, {
    6,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000,
    0b00110000,
    0b00110000,
    0b00110000
  }, {
    6,
    0b00000000,
    0b11111100,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b11111100
  },
/* Lowercase letters*/
  {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111000,
    0b00001100,
    0b01111100,
    0b11001100,
    0b01111100
  }, {
    6,
    0b00000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11111000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11000000,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b00001100,
    0b00001100,
    0b00001100,
    0b01111100,
    0b11001100,
    0b11001100,
    0b01111100
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11111100,
    0b11000000,
    0b01111000
  }, {
    6,
    0b00000000,
    0b00111000,
    0b01101100,
    0b01100000,
    0b01100000,
    0b11111000,
    0b01100000,
    0b01100000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b01111100,
    0b11001100,
    0b11001100,
    0b01111100,
    0b00001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11001100
  }, {
    4,
    0b00000000,
    0b00000000,
    0b01100000,
    0b00000000,
    0b01100000,
    0b01100000,
    0b01100000,
    0b11110000
  }, {
    5,
    0b00000000,
    0b00011000,
    0b00000000,
    0b00011000,
    0b00011000,
    0b11011000,
    0b11011000,
    0b01110000
  }, {
    6,
    0b00000000,
    0b11000000,
    0b11000000,
    0b11001100,
    0b11011000,
    0b11110000,
    0b11011000,
    0b11001100
  }, {
    2,
    0b00000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000
  }, {
    7,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11000110,
    0b11101110,
    0b11111110,
    0b11010110,
    0b11010110
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111000,
    0b11111100,
    0b11001100,
    0b11001100,
    0b11001100
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11111000,
    0b11000000,
    0b11000000
  }, {
    5,
    0b00000000,
    0b00000000,
    0b01111000,
    0b11011000,
    0b11011000,
    0b01111000,
    0b00011000,
    0b00011000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b11000000,
    0b11000000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01111100,
    0b10000000,
    0b01111000,
    0b00000100,
    0b11111000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00110000,
    0b00110000,
    0b11111100,
    0b00110000,
    0b00110000,
    0b00110000
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111100
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11001100,
    0b11001100,
    0b01111000,
    0b00110000
  }, {
    7,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11000110,
    0b11010110,
    0b11010110,
    0b11010110,
    0b01111100
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11001100,
    0b01111000,
    0b00110000,
    0b01111000,
    0b11001100
  }, {
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11001100,
    0b11001100,
    0b01111100,
    0b00001100,
    0b01111000
  }, {
    4,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11110000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b11110000
  },
/* Numbers start here*/
  {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11011100,
    0b11101100,
    0b11001100,
    0b11001100,
    0b01111000
  },
  {
    6,
    0b00000000,
    0b00110000,
    0b00110000,
    0b01110000,
    0b00110000,
    0b00110000,
    0b00110000,
    0b11111100
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b00001100,
    0b00011000,
    0b01100000,
    0b11000000,
    0b11111100
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b00001100,
    0b00111000,
    0b00001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b00011000,
    0b00111000,
    0b01011000,
    0b10011000,
    0b11111100,
    0b00011000,
    0b00011000
  }, {
    6,
    0b00000000,
    0b11111100,
    0b11000000,
    0b11111000,
    0b00001100,
    0b00001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11000000,
    0b11111000,
    0b11001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b11111100,
    0b11001100,
    0b00011000,
    0b00011000,
    0b00110000,
    0b00110000,
    0b00110000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b01111000,
    0b11001100,
    0b11001100,
    0b01111000
  }, {
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b01111100,
    0b00001100,
    0b11001100,
    0b01111000
  },
/* Symbols start here */
  { /* !  ASCII 33*/
    4,
    0b00000000,
    0b01100000,
    0b11110000,
    0b11110000,
    0b01100000,
    0b01100000,
    0b00000000,
    0b01100000
  }, { /* " */
    5,
    0b00000000,
    0b11011000,
    0b11011000,
    0b01010000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { /* # */
    7,
    0b00000000,
    0b01101100,
    0b01101100,
    0b11111110,
    0b01101100,
    0b11111110,
    0b01101100,
    0b01101100
  }, { /* $ */
    5,
    0b00000000,
    0b00100000,
    0b01111000,
    0b10000000,
    0b01110000,
    0b00001000,
    0b11110000,
    0b00100000
  }, { /* % */
    6,
    0b00000000,
    0b11000000,
    0b11001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b11001100,
    0b00001100
  }, { /* & */
    7,
    0b00000000,
    0b01111000,
    0b11001100,
    0b01111000,
    0b01010000,
    0b11001010,
    0b11001100,
    0b01111110
  }, { /* '  */
    3,
    0b00000000,
    0b11000000,
    0b11000000,
    0b01100000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { /* ( */
    4,
    0b00000000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b01100000,
    0b00110000
  }, { /* ) */
    4,
    0b00000000,
    0b11000000,
    0b01100000,
    0b00110000,
    0b00110000,
    0b00110000,
    0b01100000,
    0b11000000
  }, { /* * */
    7,
    0b00000000,
    0b00000000,
    0b01101100,
    0b00111000,
    0b11111110,
    0b00111000,
    0b01101100,
    0b00000000
  }, { /* + */
    5,
    0b00000000,
    0b00000000,
    0b00100000,
    0b00100000,
    0b11111000,
    0b00100000,
    0b00100000,
    0b00000000
  }, { /* , */
    3,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01100000,
    0b01100000,
    0b01100000,
    0b11000000
  }, { /* - */
    4,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11110000,
    0b00000000,
    0b00000000,
    0b00000000
  }, { /* . */
    2,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11000000,
    0b11000000
  }, { /* /  ASCII 47 */
    6,
    0b00000000,
    0b00000000,
    0b00001100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b00000000
  }, { /* :  ASCII 58 */
    2,
    0b00000000,
    0b00000000,
    0b11000000,
    0b11000000,
    0b00000000,
    0b11000000,
    0b11000000,
    0b00000000
  }, { /* ; */
    3,
    0b00000000,
    0b00000000,
    0b01100000,
    0b01100000,
    0b00000000,
    0b01100000,
    0b01100000,
    0b11000000
  }, { /* < */
    5,
    0b00000000,
    0b00011000,
    0b00110000,
    0b01100000,
    0b11000000,
    0b01100000,
    0b00110000,
    0b00011000
  }, { /* = */
    4,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11110000,
    0b00000000,
    0b11110000,
    0b00000000,
    0b00000000
  }, { /* > */
    5,
    0b00000000,
    0b11000000,
    0b01100000,
    0b00110000,
    0b00011000,
    0b00110000,
    0b01100000,
    0b11000000
  }, { /* ? */
    6,
    0b00000000,
    0b01111000,
    0b11001100,
    0b00001100,
    0b00111000,
    0b00110000,
    0b00000000,
    0b00110000
  }, { /* @  ASCII 64*/
    6,
    0b00000000,
    0b01110000,
    0b10001000,
    0b10111000,
    0b10110000,
    0b10000100,
    0b01111000,
    0b00000000
  }, { /* [  ASCII 91 */
    4,
    0b00000000,
    0b11110000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11110000
  }, { /* \ */
    6,
    0b00000000,
    0b00000000,
    0b11000000,
    0b01100000,
    0b00110000,
    0b00011000,
    0b00001100,
    0b00000000
  }, { /* ] */
    4,
    0b00000000,
    0b11110000,
    0b00110000,
    0b00110000,
    0b00110000,
    0b00110000,
    0b00110000,
    0b11110000
  }, { /* ^ */
    7,
    0b00000000,
    0b00010000,
    0b00101000,
    0b01000100,
    0b10000010,
    0b00000000,
    0b00000000,
    0b00000000
  }, { /* _ */
    4,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b11110000
  }, { /* ` <3  ASCII 96 */
    7,
    0b00000000,
    0b01000100,
    0b11101110,
    0b11111110,
    0b11111110,
    0b01111100,
    0b00111000,
    0b00010000
  }, { /* {  ASCII 123 */
    5,
    0b00000000,
    0b00111000,
    0b01100000,
    0b01100000,
    0b11000000,
    0b01100000,
    0b01100000,
    0b00111000
  }, { /* | */
    2,
    0b00000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000,
    0b11000000
  }, { /* } */
    5,
    0b00000000,
    0b11100000,
    0b00110000,
    0b00110000,
    0b00011000,
    0b00110000,
    0b00110000,
    0b11100000
  }, { /* ~  ASCII 126 */
    6,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01110100,
    0b11011000,
    0b00000000,
    0b00000000,
    0b00000000
  },

  { /* á (133) */
    6,
    0b00001000,
    0b00010000,
    0b00000000,
    0b01111000,
    0b00001100,
    0b01111100,
    0b11001100,
    0b01111100
  }, { /* é */
    6,
    0b00001000,
    0b00010000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11111100,
    0b11000000,
    0b01111000
  }, { /* í */
    4,
    0b00000000,
    0b00100000,
    0b01000000,
    0b00000000,
    0b01100000,
    0b01100000,
    0b01100000,
    0b11110000
  }, { /* ó */
    6,
    0b00001000,
    0b00010000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000
  }, { /* ö */
    6,
    0b00000000,
    0b00101000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000
  }, { /* ő */
    6,
    0b00100100,
    0b01001000,
    0b00000000,
    0b01111000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111000
  }, { /* ú */
    6,
    0b00001000,
    0b00010000,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111100
  }, { /* ü */
    6,
    0b00000000,
    0b00101000,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111100
  }, { /* ű */
    6,
    0b00010100,
    0b00101000,
    0b00000000,
    0b11001100,
    0b11001100,
    0b11001100,
    0b11001100,
    0b01111100
  }, {
    8, /* Face with tears of joy */
    0b00000000,
    0b01100110,
    0b01100110,
    0b10000001,
    0b00000000,
    0b01111110,
    0b01000010,
    0b00111100
  }, { /* Crying face */
    8,
    0b00000000,
    0b01100110,
    0b01100110,
    0b10000000,
    0b10000000,
    0b00111100,
    0b01000010,
    0b00000000
  }, { /* Smiling face with open mouth */
    8,
    0b00000000,
    0b01100110,
    0b01100110,
    0b00000000,
    0b00000000,
    0b01111110,
    0b01000010,
    0b00111100
  }, { /* B button */
    8,
    0b11111111,
    0b10000011,
    0b10011001,
    0b10011001,
    0b10000011,
    0b10011001,
    0b10011001,
    0b10000011
  }, { // degree
    3,
    0b11100000,
    0b10100000,
    0b11100000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
  }
};

const int ledmatrix_letter_font_size = sizeof(ledmatrix_letter_font) / 9;

const uint8_t* ledmatrix_ascii_table[128];

void ledmatrix_ascii_table_init()
{
  /* Fill all the places with space */
  for (uint8_t i = 0; i < 128; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[0];
  }

  /* Fill the capital letters (26) */
  #define ASCII_CAPTIAL_LETTERS_START_INDEX 65u
  #define ASCII_CAPTIAL_LETTERS_END_INDEX 91u
  uint8_t ledmatrix_character_offset_cnt = 1u;
  for (uint8_t i = ASCII_CAPTIAL_LETTERS_START_INDEX; i < ASCII_CAPTIAL_LETTERS_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }

  /* Fill the lower case letters (26) */
  #define ASCII_LOWERCASE_LETTERS_START_INDEX 97u
  #define ASCII_LOWERCASE_LETTERS_END_INDEX 123u
  ledmatrix_character_offset_cnt = 27u;
  for (uint8_t i = ASCII_LOWERCASE_LETTERS_START_INDEX; i < ASCII_LOWERCASE_LETTERS_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }

  /* Fill the numbers (10) */
  #define ASCII_NUMBERS_START_INDEX 48u
  #define ASCII_NUMBERS_END_INDEX 58u
  ledmatrix_character_offset_cnt = 53u;
  for (uint8_t i = ASCII_NUMBERS_START_INDEX; i < ASCII_NUMBERS_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }

  /* Fill symbol group 1 (15) */
  #define ASCII_SYMBOL_GROUP_1_START_INDEX 33u
  #define ASCII_SYMBOL_GROUP_1_END_INDEX 48u
  ledmatrix_character_offset_cnt = 63u;
  for (uint8_t i = ASCII_SYMBOL_GROUP_1_START_INDEX; i < ASCII_SYMBOL_GROUP_1_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }

  /* Fill symbol group 2 (7) */
  #define ASCII_SYMBOL_GROUP_2_START_INDEX 58u
  #define ASCII_SYMBOL_GROUP_2_END_INDEX 65u
  ledmatrix_character_offset_cnt = 78u;
  for (uint8_t i = ASCII_SYMBOL_GROUP_2_START_INDEX; i < ASCII_SYMBOL_GROUP_2_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }

  /* Fill symbol group 3 (6) */
  #define ASCII_SYMBOL_GROUP_3_START_INDEX 91u
  #define ASCII_SYMBOL_GROUP_3_END_INDEX 97u
  ledmatrix_character_offset_cnt = 85u;
  for (uint8_t i = ASCII_SYMBOL_GROUP_3_START_INDEX; i < ASCII_SYMBOL_GROUP_3_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }

  /* Fill symbol group 4 (4) */
  #define ASCII_SYMBOL_GROUP_4_START_INDEX 123u
  #define ASCII_SYMBOL_GROUP_4_END_INDEX 127u
  ledmatrix_character_offset_cnt = 91u;
  for (uint8_t i = ASCII_SYMBOL_GROUP_4_START_INDEX; i < ASCII_SYMBOL_GROUP_4_END_INDEX; i++) {
    ledmatrix_ascii_table[i] = ledmatrix_letter_font[ledmatrix_character_offset_cnt];
    ledmatrix_character_offset_cnt++;
  }
}

#define UTF8_LUT_SIZE 14u
/* UTF-8 Look Up Table */
/* UTF-8 code | Local array index */
static const uint32_t utf8_lut[UTF8_LUT_SIZE][2] = {
  { 0xc3a1, 95u },                               /* á */
  { 0xc3a9, 96u },                               /* é */
  { 0xc3ad, 97u },                               /* í */
  { 0xc3b3, 98u },                               /* ó */
  { 0xc3b6, 99u },                               /* ö */
  { 0xc591, 100u },                              /* ő */
  { 0xc3ba, 101u },                              /* ú */
  { 0xc3bc, 102u },                              /* ü */
  { 0xc5b1, 103u },                              /* ű */
  { 0xf09f9882, 104u },                          /* Face with tears of joy */
  { 0xf09f98a2, 105u },                          /* Crying face */
  { 0xf09f9883, 106u },                          /* Smiling face with open mouth */
  { 0xf09f85b1, 107u },                          /* B Button */
  { 0xc2b0, 108u },                              /* Degree sign */
};

/* This function converts an UTF-8 string to an array of pointers to the character data which should be drawn */
void convert_utf8_string_to_character_data(const uint8_t* output_data_array[], String text_in)
{
  /* Make our String fit the output */
  if (text_in.length() >= 256) {
    text_in.remove(255);
  }

  uint16_t output_position = 0u;

  for (uint16_t i = 0u; text_in[i] != '\0'; i++) {
    /* If the character is ASCII, fetch it from the ASCII table */
    if (text_in[i] < 127) {
      output_data_array[output_position] = ledmatrix_ascii_table[(uint8_t)text_in[i]];
      output_position++;
      continue;
    }

    /* If the character is UTF-8, it needs further handling */

    uint8_t current_char = (uint8_t)text_in[i];

    /* Determine the byte length of the UTF-8 character */
    /* Check how many bits are 1 before we encounter a 0 */
    uint8_t utf8_length = 0u;
    uint8_t bit_offset = 7;
    while (current_char & (1 << bit_offset)) {
      if (bit_offset == 0) {
        break;
      }
      utf8_length++;
      bit_offset--;
    }

    /* Check that length doesn't exceed 4 bytes */
    /* If the length exceeds 4 bytes then something went really wrong, we just leave the string as it is and return */
    if (utf8_length > 4u) {
      break;
    }

    /* Check if the requested length doesn't go over the end of our string */
    if (i + utf8_length > text_in.length()) {
      break;
    }

    // Serial.printf("UTF-8 len: %d\n", utf8_length);

    /* Combine the calculated number of bytes together to get our UTF-8 character */
    uint32_t utf8_char = 0u;
    uint8_t combine_shift = (utf8_length * 8) - 8;
    for (uint8_t j = i; j < i + utf8_length; j++) {
      utf8_char |= ((uint8_t)text_in[j] << combine_shift);
      combine_shift -= 8;
    }

    // Serial.printf("UTF-8 char: 0x%x\n", utf8_char);

    /* Search the Look Up Table for a matching character */
    bool character_match_found = false;
    for (uint8_t k = 0u; k < UTF8_LUT_SIZE; k++) {
      if (utf8_lut[k][0] == utf8_char) {
        uint8_t character_data_index = utf8_lut[k][1];
        output_data_array[output_position] = ledmatrix_letter_font[character_data_index];
        character_match_found = true;
        break;
      }
    }

    /* If we don't have the requested character, print a space */
    if (character_match_found == false) {
      output_data_array[output_position] = ledmatrix_ascii_table[32];
    }

    output_position++;

    /* Step the iterator forward to the end of the current UTF-8 character */
    i += utf8_length - 1;
  }

  /* Terminate the output data with NULL */
  output_data_array[output_position] = NULL;
}

#endif
