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

 

// Initalize TFT
TFT_eSPI tft = TFT_eSPI();

// Initalize Sprites
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
const int WALK_SHEET_WIDTH = 512;
const int RUN_SHEET_WIDTH = 384;
const int SHEET_HEIGHT = 36;
const int FRAME_WIDTH = 64;  // Each frame of the horse is 64 pixels wide
const int FRAME_WIDTH_MINI = 36;
const int FRAME_WIDTH_RUN_MINI = 50;
const int ANIM_FRAME_SCALE = 3;
const int ANIM_FRAME_COUNT = 8;  // Total animation frames
const int SCALED_SPRITE_WIDTH = FRAME_WIDTH_MINI * ANIM_FRAME_SCALE;
const int SCALED_SPRITE_WIDTH_RUN = FRAME_WIDTH_RUN_MINI * ANIM_FRAME_SCALE;
const int SCALED_SPRITE_HEIGHT = (SHEET_HEIGHT)*ANIM_FRAME_SCALE;
const int ANIM_PRINT_HEIGHT = 180;

const int HEART_1_X_LOC = 45;
const int HEART_2_X_LOC = 85;
const int HEART_3_X_LOC = 125;
const int HEART_4_X_LOC = 165;
const int HEART_ALL_Y_LOC = 100;


// text definitions
const int TITLE_TEXT_WID = 39;
const int TITLE_TEXT_HEI = 15;
const int TITLE_TEXT_SCALE = 5;
const int TITLE_TEXT_WDITH_SCALED = TITLE_TEXT_WID * TITLE_TEXT_SCALE;
const int TITLE_TEXT_HEIGHT_SCALED = TITLE_TEXT_HEI * TITLE_TEXT_SCALE;
 
// Grass definitions
const int GRASS_WID = 240;
const int GRASS_HEI = 110;

 
int anim_frame = 0;
int frame = 0;
int health = 5;
bool death_anim_flag = 0;
int pet_state = 3; // 1=dead, 2=ok, 3=excited


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
  tft.setSwapBytes(true);
  init_background();
  init_pet_sprite();
  init_hearts();
  init_threshold();
}

 

void loop() {

  current_voltage = readBatteryVoltage();

  check_battery_voltage();

  print_current_voltage();

  check_current_state();

  play_animation();

  draw_hearts(health);

  decrement_health();

  frame++;

  delay(150);

}

 

float readBatteryVoltage() {

  int adcValue = analogRead(VAPE_PIN);

  float vMeasured = (adcValue * VREF) / ADC_MAX;

  return vMeasured * DIVIDER;  // actual battery voltage

}

 

void check_battery_voltage() {

  if (current_voltage < THRESHOLD) {

    hit_detected();

    }

  }

 

void hit_detected() {

    health = health + 5;

    if (health > 100) {

      health = 100;

    }

}

 

void decrement_health() {

  if (health > 0 && (frame % 20 == 0)) {

    health = health - 1;

  }

}

 

void check_current_state() {

  if (health > 50){

    pet_state = 3; // excited

  } else if (health <= 50 && health > 0){

    pet_state = 2; // ok

  } else if (health == 0){

    pet_state = 1; // dead

  } else if (health == 1){

    death_anim_flag = 1;

  }

 

 

}




void play_animation() {

  if (pet_state == 3) {

    run_animation();

  } else if (pet_state == 2) {

    walk_animation();

  } else if (pet_state == 1) {

    death_animation();

  }

}

 

void walk_animation() {

  anim_frame = frame % 8;

  int image_x_offset = ((anim_frame * FRAME_WIDTH));

  image_x_offset += 16;

  sprite.fillSprite(TFT_WHITE);

  scale_and_draw_sprite(&sprite, epd_bitmap_Light_Blue_Horse, WALK_SHEET_WIDTH, SHEET_HEIGHT, ANIM_FRAME_SCALE, image_x_offset, FRAME_WIDTH_MINI);

  sprite.pushSprite(75, ANIM_PRINT_HEIGHT);

}

 

void erase_sprites() {

  runsprite.fillSprite(TFT_WHITE);

  sprite.fillSprite(TFT_WHITE);

  runsprite.pushSprite(45, ANIM_PRINT_HEIGHT);

  sprite.pushSprite(75, ANIM_PRINT_HEIGHT);

}

 

void run_animation() {

  anim_frame = frame % 6;

  int image_x_offset = ((anim_frame * FRAME_WIDTH));

  image_x_offset += 10;

  runsprite.fillSprite(TFT_WHITE);

  scale_and_draw_sprite(&runsprite, epd_bitmap_Light_Blue_Horse_Run, RUN_SHEET_WIDTH, SHEET_HEIGHT, ANIM_FRAME_SCALE, image_x_offset, FRAME_WIDTH_RUN_MINI);

  runsprite.pushSprite(45, ANIM_PRINT_HEIGHT);

}

 

void death_animation() {

  if (death_anim_flag == 1) {

    for (int i = 0; i <= 7; i++) {

      anim_frame = i;

      int image_x_offset = ((anim_frame * FRAME_WIDTH));

      image_x_offset += 16;

      sprite.fillSprite(TFT_WHITE);

      scale_and_draw_sprite(&sprite, epd_bitmap_Light_Blue_Horse_Die, WALK_SHEET_WIDTH, SHEET_HEIGHT, ANIM_FRAME_SCALE, image_x_offset, FRAME_WIDTH_MINI);

      sprite.pushSprite(75, ANIM_PRINT_HEIGHT);

      delay(200);

      if(i==7){

        death_anim_flag = 0;

      }

    }

  }

}

 

void draw_hearts(int health) {

  // Heart 1

  if (health == 0) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_1_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 0 && health < 12.5) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_1_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 12.5) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_1_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  // Heart 2

  if (health < 25) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_2_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 24 && health < 37.5) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_2_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 50) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_2_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  // Heart 3

  if (health < 50) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_3_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 49 && health < 62.5) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_3_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 75) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_3_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  // Heart 4

  if (health < 75) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 30, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_4_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 74 && health < 87.5) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 15, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_4_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

  if (health > 87.5) {

    scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

    heart1.pushSprite(HEART_4_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  }

}

 

void scale_and_draw_sprite(TFT_eSprite *targetSprite, const uint16_t *data, int width, int height, int scale, int image_x_offset, int frame_width) {

  for (int j = 0; j < height; j++) {

    for (int i = 0; i < frame_width; i++) {

      uint16_t color = data[j * width + (i + image_x_offset)];

      // Draw a filled rectangle of the size specified by the 'scale' factor

      targetSprite->fillRect(i * scale,  j * scale, scale, scale, color);

    }

  }

}

 

void init_background() {

  // Draw gradient sky

  int half_height = tft.height() / 2;

  uint16_t light_blue_top = tft.color565(67, 184, 239);

  uint16_t white_bottom = TFT_WHITE;

  tft.fillRect(0, half_height, tft.width(), tft.height() - half_height, white_bottom);

  tft.fillRectVGradient(0, 0, tft.width(), half_height, light_blue_top, white_bottom);

  // Draw grass

  tft.pushImage(0, 210, GRASS_WID, GRASS_HEI, epd_grass);

  // Draw text

  text_sprite.createSprite(TITLE_TEXT_WDITH_SCALED, TITLE_TEXT_HEIGHT_SCALED);

  scale_and_draw_sprite(&text_sprite, epd_bitmap_vibiesthorsetext, TITLE_TEXT_WID, TITLE_TEXT_HEIGHT_SCALED, TITLE_TEXT_SCALE, 0, TITLE_TEXT_WID);

  text_sprite.pushSprite(25, 10);

  text_sprite.deleteSprite();

}

 

void init_hearts() {

  heart1.createSprite(SCALED_HEART_WIDTH, SCALED_HEART_HEIGHT);

  scale_and_draw_sprite(&heart1, epd_bitmap_heart,  HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

  heart1.pushSprite(HEART_1_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

  heart1.pushSprite(HEART_2_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

  heart1.pushSprite(HEART_3_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

  scale_and_draw_sprite(&heart1, epd_bitmap_heart, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);

  heart1.pushSprite(HEART_4_X_LOC, HEART_ALL_Y_LOC, TFT_WHITE);

 

}

 

void init_pet_sprite() {

    sprite.createSprite(SCALED_SPRITE_WIDTH, SCALED_SPRITE_HEIGHT);

    sprite.setSwapBytes(true);

    runsprite.createSprite(SCALED_SPRITE_WIDTH_RUN, SCALED_SPRITE_HEIGHT);

    runsprite.setSwapBytes(true);

}

 

void init_threshold() {

    tft.setTextColor(TFT_BLACK, TFT_WHITE);

    tft.setTextSize(1);

    tft.setCursor(10, 10);

    THRESHOLD = readBatteryVoltage() - .01;

    dtostrf(THRESHOLD, 5, 4, buffer);  // 5 is the total width, 2 is precision

    tft.print(buffer);

}

 

void print_current_voltage() {

  tft.setCursor(200, 10);

  dtostrf(current_voltage, 5, 4, buffer);

  tft.print(buffer);

}