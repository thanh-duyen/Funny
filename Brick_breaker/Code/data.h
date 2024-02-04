#include <map>
#include "esp_sleep.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include "RTClib.h"
#include "FS.h"
#include "SD.h"
#include <vector>
#include <EEPROM.h>
#include <JPEGDecoder.h>

#define LED_LCD_PIN 27
#define POWER_EN_PIN 16
#define BUZZER_PIN 17
#define VOLT_EN_PIN 15
#define VOLT_PIN 14
#define CE_PIN 12
#define CSN_PIN 13
#define IRQ_PIN 35
#define BUTTON_PIN 0

#define RADIUS_CIRCLE 5
#define BLOCK_SIZE 20
uint8_t brick_data[32][48];
struct sBalls{
  uint16_t x1[6], y1[6], x2[6], y2[6];
  uint16_t old_x[6], old_y[6];
  int16_t new_x[6], new_y[6];
  uint16_t start_x, start_y;
  float i[6];
  bool running;
  uint16_t angle[6];
  uint16_t color[6];
  uint32_t time_run[6];
  uint32_t delay;
  uint16_t step;
};
sBalls ball;
