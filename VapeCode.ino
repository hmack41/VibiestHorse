#include <SPI.h>
#include <TFT_eSPI.h>
#include "title-text.h"
#include "animal.h"
#include "grass.h"
#include "heart.h"
#include "cig.h"
#define VAPE_PIN PA6
#define LED_PIN PC13

// TFT and Sprite objects
TFT_eSPI tft              = TFT_eSPI();
TFT_eSprite sprite        = TFT_eSprite(&tft); 
TFT_eSprite runsprite     = TFT_eSprite(&tft);
TFT_eSprite text_sprite   = TFT_eSprite(&tft);  // Used for the text
TFT_eSprite heart1        = TFT_eSprite(&tft);

// Hearts
const int HEART_SHEET_WIDTH = 45;
const int HEART_SHEET_HEIGHT = 13;
const int HEART_FRAME_WIDTH = 15;
const int HEART_SCALE = 2;
const int SCALED_HEART_WIDTH = HEART_FRAME_WIDTH * HEART_SCALE;
const int SCALED_HEART_HEIGHT = HEART_SHEET_HEIGHT * HEART_SCALE;

// Image and animation frame definitions
const int HORSE_SHEET_WIDTH = 512;
const int HORSE_RUN_SHEET_WIDTH = 512;
const int HORSE_SHEET_HEIGHT = 36;
const int HORSE_FRAME_WIDTH = 64;  // Each frame of the horse is 64 pixels wide
const int HORSE_FRAME_WIDTH_MINI = 36;
const int HORSE_FRAME_WIDTH_RUN_MINI = 50;
const int SCALE = 3;
const int ANIM_FRAME_COUNT = 8;  // Total animation frames
const int SCALED_SPRITE_WIDTH = HORSE_FRAME_WIDTH_MINI * SCALE;
const int SCALED_SPRITE_WIDTH_RUN = HORSE_FRAME_WIDTH_RUN_MINI * SCALE;
const int SCALED_SPRITE_HEIGHT = (HORSE_SHEET_HEIGHT)*SCALE;
const int HORSE_PRINT_HEIGHT = 120;

const int x1 = 45;
const int x2 = 85;
const int x3 = 125;
const int x4 = 165;
const int s1 = 0;
const int s2 = 15;
const int s3 = 30;
const int yall = 70;

// text definitions
const int vhwid = 39;
const int vhhei = 23;
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
int score = 0;
int anim_state;
int grnd_speed = 0;

// Heart parameters
int h4_x = 0;
int h3_x = 0;
int h2_x = 0;
int h1_x = 0;

// Vape measurement parameters
float current_voltage = 10;
const float VREF = 3.3;     // ADC reference voltage
const int ADC_MAX = 4095;   // 12-bit ADC
const float DIVIDER = 2.0;  // Adjust for your resistor divider ratio
float THRESHOLD = .975;     // Trigger when battery < threshold
char buffer[20];
uint16_t sky_color = tft.color565(67, 184, 239);

void setup() {
  digitalWrite(LED_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  SPI.begin();
  tft.init();
  pinMode(PC13, OUTPUT);
  tft.setRotation(270);
  ///////////// CREATE THE SKY ////////////////////
  int half_height = tft.height() / 2;

  uint16_t white_bottom = TFT_WHITE;
  tft.fillRect(0, half_height, tft.width(), tft.height() - half_height, white_bottom);
  tft.fillRectVGradient(0, 0, tft.width(), half_height, sky_color, sky_color);
  //////////////////////////////////////////////////


  tft.setSwapBytes(true);
  tft.pushImage(0, 210, gwid, ghei, epd_grass);
  text_sprite.createSprite(scaledvhwid, scaledvhhei);
  drawImageScaled(&text_sprite, myBitmaptitle_text, 0, 0, vhwid, vhhei, vhsca, 0, vhwid);
  text_sprite.pushSprite(25, 10);
  text_sprite.deleteSprite();
  sprite.createSprite(tft.width(), 250);
  sprite.setSwapBytes(true);
  runsprite.createSprite(SCALED_SPRITE_WIDTH_RUN, SCALED_SPRITE_HEIGHT);
  runsprite.setSwapBytes(true);
  // heart1.createSprite(SCALED_HEART_WIDTH, SCALED_HEART_HEIGHT);
  // drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  // heart1.pushSprite(x1, yall, TFT_WHITE);
  // drawImageScaled(&heart1, epd_1bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  // heart1.pushSprite(x2, yall, TFT_WHITE);
  // drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  // heart1.pushSprite(x3, yall, TFT_WHITE);
  // drawImageScaled(&heart1, epd_bitmap_heart, 0, 0, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, 0, HEART_SHEET_WIDTH);
  // heart1.pushSprite(x4, yall, TFT_WHITE);
  /////////////// Initalize threshold /////////////
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  THRESHOLD = readBatteryVoltage() + .1;
  dtostrf(THRESHOLD, 5, 4, buffer);  // 5 is the total width, 2 is precision
  // tft.print(buffer);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  current_voltage = readBatteryVoltage();
  check_battery_voltage();
  set_state();
  draw_sprite();
  digitalWrite(LED_PIN, LOW);   // turn the LED off by making the voltage LOW
  decrement_health();
  frame++;
}

void draw_sky(){
  sprite.setTextColor(sky_color,TFT_WHITE);
  sprite.setTextSize(3);
  sprite.setCursor(175,22);
  sprite.print(score);
}

void draw_sprite(){
  sprite.fillSprite(TFT_WHITE);
  sprite.fillRectVGradient(0, -50,tft.width(),tft.height()/2,sky_color,TFT_WHITE);
  drawImageScaled(&sprite, myBitmaptitle_text, 25, -60, vhwid, vhhei, vhsca, 0, vhwid);
  draw_sky();
  draw_heart();
  ground_animation();
  switch(anim_state){
    case 2:
      run_animation();
      break;
    case 1:
      walk_animation();
      break;
    case 0:
      death_animation();
      break;
  }
  // drawImageScaled(&sprite, myBitmapcig, 30, 180, 30, 30, 2, 0, 30);
  sprite.pushSprite(0,70);
}

void ground_animation(){
  drawImageScaled(&sprite, epd_grass, ((frame*grnd_speed)%240)     ,140,gwid,ghei,1,0,gwid);
  drawImageScaled(&sprite, epd_grass, ((frame*grnd_speed)%240)-240 ,140,gwid,ghei,1,0,gwid);
}

void write_score(){
  tft.setCursor(200, 10);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(1);
  dtostrf(current_voltage, 5, 4, buffer);
  // tft.print(buffer);
  tft.setTextColor(sky_color,TFT_WHITE);
  tft.setTextSize(4);
  tft.setCursor(175,92);
  tft.print(score);
}

void check_battery_voltage() {
  if (current_voltage > THRESHOLD) {
    digitalWrite(LED_PIN, LOW);  // LED ON (active low)
    health = health + 2;
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
  if (health > 0 && (frame % 15 == 0)) {
    health = health - 2;
  } else if (health == 1) {
    health = health - 1;
    death_flag = 1;
  }
}

void set_state() {
    if (health == 2) {
    death_flag = 1;
  }
  if (health >= 50 && health > 2) {
    anim_state = 2;
    grnd_speed = 6;
  } else if (health < 50 && health > 1) {
    anim_state = 1;
    grnd_speed = 4;
  } else if (health < 2) {
    anim_state = 0;
    grnd_speed = 0;
  }
}
void play_animation() {
  if (health == 2) {
    death_flag = 1;
  }
  if (health >= 50 && health > 2) {
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
  if(frame% 10 == 1){
    score++;
  }
  int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
  image_x_offset += 16;
  drawImageScaled(&sprite, myBitmapanimal_1, 120-(HORSE_FRAME_WIDTH/2), HORSE_PRINT_HEIGHT, HORSE_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_MINI);
  // sprite.pushSprite(0, 170);
}

void erase_sprites() {
  runsprite.fillSprite(TFT_WHITE);
  sprite.fillSprite(TFT_WHITE);
  runsprite.pushSprite(45, HORSE_PRINT_HEIGHT);
  sprite.pushSprite(75, HORSE_PRINT_HEIGHT);
}

void run_animation() {
  if(frame% 10 == 1){
    score++;
  }
  anim_frame = frame % 6;
  int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
  image_x_offset += 10;

  drawImageScaled(&sprite, myBitmapanimal_2, 45, HORSE_PRINT_HEIGHT, HORSE_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_MINI);
  // sprite.pushSprite(0, 170);
}

void death_animation() {
  score = 0;
  if(death_flag != 0){
    int anim_frame = 0;
  }
  int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
  image_x_offset += 16;
  drawImageScaled(&sprite, myBitmapanimal_0, 45, HORSE_PRINT_HEIGHT, HORSE_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_MINI);
  if(anim_frame<7){
    anim_frame++;
  }
  delay(200);
  death_flag = 0;
}

void draw_heart() {
  // Heart 1
  if (health == 0) {
    h1_x = 30;
  }
  if (health > 0 && health < 12.5) {
    h1_x = 15;
  }
  if (health > 12.5) {
    h1_x = 0;
  }
  // Heart 2
  if (health <12.5) {
    h2_x = 30;
  }
  if (health > 24 && health < 37.5) {
    h2_x = 15;
  }
  if (health > 50) {
    h2_x = 0;
  }
  // Heart 3
  if (health < 50) {
    h3_x = 30;
  }
  if (health > 49 && health < 62.5) {
    h3_x = 15;
  }
  if (health > 75) {
    h3_x = 0;
  }
  // Heart 4
  if (health < 75) {
    h4_x = 30;
  }
  if (health > 74 && health < 87.5) {
    h4_x = 15;
  }
  if (health > 87.5) {
    h4_x = 0;
  }
  drawImageScaled(&sprite, epd_bitmap_heart, x1, yall, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, h1_x, HEART_FRAME_WIDTH);
  drawImageScaled(&sprite, epd_bitmap_heart, x2, yall, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, h2_x, HEART_FRAME_WIDTH);
  drawImageScaled(&sprite, epd_bitmap_heart, x3, yall, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, h3_x, HEART_FRAME_WIDTH);
  drawImageScaled(&sprite, epd_bitmap_heart, x4, yall, HEART_SHEET_WIDTH, HEART_SHEET_HEIGHT, HEART_SCALE, h4_x, HEART_FRAME_WIDTH);
}

void drawImageScaled(TFT_eSprite *targetSprite, const uint16_t *data, int x, int y, int width, int height, int scale, int image_x_offset, int frame_width) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < frame_width; i++) {
      uint16_t color = data[j * width + (i + image_x_offset)];
      if(color!=0x0000){
      targetSprite->fillRect(x + i * scale, y + j * scale, scale, scale, color);
      }
    }
  }
}