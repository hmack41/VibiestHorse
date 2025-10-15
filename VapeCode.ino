#include <SPI.h>
#include <TFT_eSPI.h>
#include "vibiesthorsetext.h"
#include "horseimage.h"
#include "horseimagerun.h"
#include "grass.h"
#include "heart.h"

// TFT and Sprite objects
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft); // Used for the horse animation
TFT_eSprite text_sprite = TFT_eSprite(&tft); // Used for the text

// Image and animation frame definitions
const int HORSE_SHEET_WIDTH = 512;
const int HORSE_SHEET_HEIGHT = 36;
const int HORSE_FRAME_WIDTH = 64; // Each frame of the horse is 64 pixels wide
const int HORSE_FRAME_WIDTH_MINI = 36;
const int SCALE = 3;
const int ANIM_FRAME_COUNT = 8; // Total animation frames
const int SCALED_SPRITE_WIDTH = HORSE_FRAME_WIDTH_MINI * SCALE;
const int SCALED_SPRITE_HEIGHT = (HORSE_SHEET_HEIGHT) * SCALE;
const int HORSE_PRINT_HEIGHT = 180;

// text definitions
const int vhwid = 39;
const int vhhei = 15;
const int vhsca = 5;
const int scaledvhwid = vhwid*vhsca;
const int scaledvhhei = vhhei*vhsca;

// Grass definitions
const int gwid  = 240;
const int ghei  = 110;


int anim_frame = 0;
int frame = 0;


void setup(){

  tft.init();
  tft.setRotation(270);
  ///////////// CREATE THE SKY ////////////////////
 int half_height = tft.height() / 2;
  uint16_t light_blue_top = tft.color565(67, 184, 239);
  uint16_t white_bottom = TFT_WHITE;

  // Fill the bottom half with a solid white color
  tft.fillRect(0, half_height, tft.width(), tft.height() - half_height, white_bottom);
  
  // Draw the gradient in the top half, starting with white at the bottom and transitioning to blue at the top
  tft.fillRectVGradient(0, 0, tft.width(), half_height, light_blue_top, white_bottom);
  //////////////////////////////////

  // tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  tft.setTextSize(2); 
  tft.setCursor(10, 10);
  tft.setSwapBytes(true);  
  tft.pushImage(0,210,gwid,ghei,epd_grass);
  sprite.createSprite(SCALED_SPRITE_WIDTH, SCALED_SPRITE_HEIGHT);
  sprite.setSwapBytes(true);
  text_sprite.createSprite(scaledvhwid,scaledvhhei);
  drawImageScaled(&text_sprite, epd_bitmap_vibiesthorsetext, 0, 0, vhwid, vhhei, vhsca, 0, vhwid);
  //text_sprite.pushSprite(25, 10,TFT_WHITE);
  text_sprite.pushSprite(25, 10);

}

void loop(){
  // run_animation();
  // text_sprite.pushSprite(10, 10,TFT_WHITE);
    walk_animation();
  frame++;
  delay(150);
}

void run_animation(){
}


void walk_animation(){
  anim_frame = frame%8;
  // int anim_frame = (ANIM_FRAME_COUNT - 1) - (frame % ANIM_FRAME_COUNT);
  int image_x_offset = ((anim_frame * HORSE_FRAME_WIDTH));
  image_x_offset += 16;
  sprite.fillSprite(TFT_WHITE);
  // sprite.pushImage(-1*(anim_frame*64),0,512,36,epd_bitmap_Light_Blue_Horse);
  drawImageScaled(&sprite, epd_bitmap_Light_Blue_Horse, 0, 0, HORSE_SHEET_WIDTH, HORSE_SHEET_HEIGHT, SCALE, image_x_offset, HORSE_FRAME_WIDTH_MINI);
  sprite.pushSprite(75, HORSE_PRINT_HEIGHT);
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