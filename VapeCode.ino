#include <SPI.h>
#include <TFT_eSPI.h>
#include "vibiesthorsetext.h"
#include "horseimage.h"
#include "horseimagerun.h"
#include "grass.h"
#include "heart.h"
#include "horseimagedying.h"
#define VAPE_PIN PA6
#define LED_PIN PC13

// TFT and Sprite objects
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);  // Used for the horse animation
TFT_eSprite runsprite = TFT_eSprite(&tft);
TFT_eSprite text_sprite = TFT_eSprite(&tft);  // Used for the text
TFT_eSprite heart1 = TFT_eSprite(&tft);


// Hearts
const int HEART_SHEET_WIDTH = 45;
const int HEART_SHEET_HEIGHT = 13;
const int HEART_FRAME_WIDTH = 15;
const int HEART_SCALE = 2;
const int SCALED_HEART_WIDTH = HEART_FRAME_WIDTH * HEART_SCALE;
const int SCALED_HEART_HEIGHT = HEART_SHEET_HEIGHT * HEART_SCALE;

// Image and animation frame definitions
const int HORSE_SHEET_WIDTH = 512;
const int HORSE_RUN_SHEET_WIDTH = 384;
const int HORSE_SHEET_HEIGHT = 36;
const int HORSE_FRAME_WIDTH = 64;  // Each frame of the horse is 64 pixels wide
const int HORSE_FRAME_WIDTH_MINI = 36;
const int HORSE_FRAME_WIDTH_RUN_MINI = 50;
const int SCALE = 3;
const int ANIM_FRAME_COUNT = 8;  // Total animation frames
const int SCALED_SPRITE_WIDTH = HORSE_FRAME_WIDTH_MINI * SCALE;
const int SCALED_SPRITE_WIDTH_RUN = HORSE_FRAME_WIDTH_RUN_MINI * SCALE;
const int SCALED_SPRITE_HEIGHT = (HORSE_SHEET_HEIGHT)*SCALE;
const int HORSE_PRINT_HEIGHT = 180;

const int x1 = 45;
const int x2 = 85;
const int x3 = 125;
const int x4 = 165;
const int s1 = 0;
const int s2 = 15;
const int s3 = 30;
const int yall = 100;


// text definitions
const int vhwid = 39;
const int vhhei = 15;
const int vhsca = 5;
const int scaledvhwid = vhwid * vhsca;
const int scaledvhhei = vhhei * vhsca;

// Grass definitions
const int gwid = 240;
const int ghei = 110;

int anim_frame = 0;
int frame = 0;
int health = 5;
int death_flag = 0;

// Vape measurement parameters
float current_voltage = 10;
const float VREF = 3.3;     // ADC reference voltage
const int ADC_MAX = 4095;   // 12-bit ADC
const float DIVIDER = 2.0;  // Adjust for your resistor divider ratio
float THRESHOLD = .975;     // Trigger when battery < threshold
char buffer[20];

void setup() {

  tft.init();
  tft.setRotation(270);
  ///////////// CREATE THE SKY ////////////////////
  int half_height = tft.height() / 2;
  uint16_t light_blue_top = tft.color565(67, 184, 239);
  uint16_t white_bottom = TFT_WHITE;
  tft.fillRect(0, half_height, tft.width(), tft.height() - half_height, white_bottom);
  tft.fillRectVGradient(0, 0, tft.width(), half_height, light_blue_top, white_bottom);
  //////////////////////////////////


  tft.setSwapBytes(true);
  tft.pushImage(0, 210, gwid, ghei, epd_grass);
  text_sprite.createSprite(scaledvhwid, scaledvhhei);
  drawImageScaled(&text_sprite, epd_bitmap_vibiesthorsetext, 0, 0, vhwid, vhhei, vhsca, 0, vhwid);
  text_sprite.pushSprite(25, 10);
  text_sprite.deleteSprite();
  sprite.createSprite(SCALED_SPRITE_WIDTH, SCALED_SPRITE_HEIGHT);
  sprite.setSwapBytes(true);
  runsprite.createSprite(SCALED_SPRITE_WIDTH_RUN, SCALED_SPRITE_HEIGHT);
  runsprite.setSwapBytes(true);
  heart1.createSprite(SCALED_HEART_WIDTH, SCALED_HEART_HEIGHT);
  drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  heart1.pushSprite(x1, yall, TFT_WHITE);
  drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  heart1.pushSprite(x2, yall, TFT_WHITE);
  drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  heart1.pushSprite(x3, yall, TFT_WHITE);
  drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  heart1.pushSprite(x4, yall, TFT_WHITE);
  /////////////// Initalize threshold /////////////
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  THRESHOLD = readBatteryVoltage() - .01;
  dtostrf(THRESHOLD, 5, 4, buffer);  // 5 is the total width, 2 is precision
  tft.print(buffer);
}

void loop() {
  current_voltage = readBatteryVoltage();
  check_battery_voltage();
  tft.setCursor(200, 10);
  dtostrf(current_voltage, 5, 4, buffer);
  tft.print(buffer);
  decrement_health();
  play_animation();
  frame++;
  draw_heart(health);
  delay(150);
}

void check_battery_voltage() {
  if (current_voltage < THRESHOLD) {
    digitalWrite(LED_PIN, LOW);  // LED ON (active low)
    health = health + 5;
    if (health > 100) {
      health = 100;
    }
  } else {
    digitalWrite(LED_PIN, HIGH);  // LED OFF
  }
}

float readBatteryVoltage() {
  int adcValue = analogRead(VAPE_PIN);
  float vMeasured = (adcValue * VREF) / ADC_MAX;
  return vMeasured * DIVIDER;  // actual battery voltage
}

void decrement_health() {
  if (health > 0 && (frame % 20 == 0)) {
    health = health - 1;
  } else if (health == 1) {
    health = health - 1;
    death_flag = 1;
  }
}


void play_animation() {
  if (health == 2) {
    death_flag = 1;
  }
  if (health > 50 && health > 2) {
    run_animation();
  } else if (health < 50 && health > 1) {
    walk_animation();
  } else if (health < 2) {
    death_animation();
  }
  if (health == 50) {
    erase_sprites();
  }
}

void walk_animation() {
  anim_frame = frame % 8;
  // int anim_frame = (ANIM_FRAME_COUNT - 1) - (frame % ANIM_FRAME_COUNT);
  int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
  image_x_offset += 16;
  sprite.fillSprite(TFT_WHITE);
  drawImageScaled(&sprite, epd_bitmap_Light_Blue_Horse, 0, 0, HORSE_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_MINI);
  sprite.pushSprite(75, HORSE_PRINT_HEIGHT);
}

void erase_sprites() {
  runsprite.fillSprite(TFT_WHITE);
  sprite.fillSprite(TFT_WHITE);
  runsprite.pushSprite(45, HORSE_PRINT_HEIGHT);
  sprite.pushSprite(75, HORSE_PRINT_HEIGHT);
}

void run_animation() {
  anim_frame = frame % 6;
  int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
  image_x_offset += 10;
  runsprite.fillSprite(TFT_WHITE);
  drawImageScaled(&runsprite, epd_bitmap_Light_Blue_Horse_Run, 0, 0, HORSE_RUN_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_RUN_MINI);
  runsprite.pushSprite(45, HORSE_PRINT_HEIGHT);
}

void death_animation() {
  while (death_flag != 0) {
    for (int i = 0; i <= 7; i++) {
      anim_frame = i;
      int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
      image_x_offset += 16;
      sprite.fillSprite(TFT_WHITE);
      drawImageScaled(&sprite, epd_bitmap_Light_Blue_Horse_Die, 0, 0, HORSE_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_MINI);
      sprite.pushSprite(75, HORSE_PRINT_HEIGHT);
      delay(200);
    }
    death_flag = 0;
  }
}

void draw_heart(int health) {
  // Heart 1
  if (health == 0) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);
    heart1.pushSprite(x1, yall, TFT_WHITE);
  }
  if (health > 0 && health < 12.5) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);
    heart1.pushSprite(x1, yall, TFT_WHITE);
  }
  if (health > 12.5) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
    heart1.pushSprite(x1, yall, TFT_WHITE);
  }
  // Heart 2
  if (health < 25) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);
    heart1.pushSprite(x2, yall, TFT_WHITE);
  }
  if (health > 24 && health < 37.5) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);
    heart1.pushSprite(x2, yall, TFT_WHITE);
  }
  if (health > 50) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
    heart1.pushSprite(x2, yall, TFT_WHITE);
  }
  // Heart 3
  if (health < 50) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);
    heart1.pushSprite(x3, yall, TFT_WHITE);
  }
  if (health > 49 && health < 62.5) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);
    heart1.pushSprite(x3, yall, TFT_WHITE);
  }
  if (health > 75) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
    heart1.pushSprite(x3, yall, TFT_WHITE);
  }
  // Heart 4
  if (health < 75) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);
    heart1.pushSprite(x4, yall, TFT_WHITE);
  }
  if (health > 74 && health < 87.5) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);
    heart1.pushSprite(x4, yall, TFT_WHITE);
  }
  if (health > 87.5) {
    drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
    heart1.pushSprite(x4, yall, TFT_WHITE);
  }
}



void drawImageScaled(TFT_eSprite *targetSprite, const uint16_t *data, int x, int y, int width, int height, int scale, int image_x_offset, int frame_width) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < frame_width; i++) {
      uint16_t color = data[j * width + (i + image_x_offset)];

      // Draw a filled rectangle of the size specified by the 'scale' factor
      targetSprite->fillRect(x + i * scale, y + j * scale, scale, scale, color);
    }
  }
}