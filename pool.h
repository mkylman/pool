#define BORDER 20

#include "player.h"
#include "pocket.h"
#include "ball.h"

void putText(const char *s) {
  int len = strlen(s) * 6;
  tft.fillScreen( BLACK );
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor((WIDTH - len) / 2, HEIGHT / 2);
  tft.print(s);
  delay(1000);
  tft.fillScreen( DGREEN );
  drawBalls();
}

void putText2(const char *s, unsigned int ypos) {
  int len = strlen(s) * 6;
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor((WIDTH - len) / 2, ypos);
  tft.print(s);
}

uint8_t balls_moving(void){
  uint8_t moving = 0;
  for (int i = 0; i < 4; i++) {
    Ball *ball = ball_list[i];
    while ( ball ) {
      if (ball->power) moving++;
      ball = ball->next;
    }
  }
  return moving;
}
