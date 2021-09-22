#include "colors.h"
#include "point.h"
#include "pocket.h"
#include "ball.h"

void setup() {
  tft.begin( tft.readID() );
  tft.fillScreen( DGREEN );

  loadPockets();
  loadBalls();
  drawBalls();
  drawPockets();
}

void loop() {
  static unsigned long ms = millis();
  
  if (millis() - ms >= 1000 / 15) {
    ms = millis();

    // DRAW, MOVE BALLS, COLLISION CHECKS

    for (int i = 0; i < 4; i++) {
      Ball *ball = ball_list[i];
      while ( ball != NULL ){
        Ball *nball = ball->next != NULL ? ball->next : NULL;
        ball->power = ball->power > 0 ? ((ball->power * 4) - 1) / 4 : 0;
        
        if (ball->sunk != true){
          // MOVE BALLS
          moveBall(ball);
          
          // EDGE COLLISION
          edgeCollision(ball);

          // POCKET COLLISION
          pocketCollision(ball);

          // BALL COLLISION
          ballCollision(ball);

        } else {
          if (ball->next != NULL) {
            ball->next->prev = ball->prev == NULL ? NULL : ball->prev;
          }
          if (ball->prev != NULL) {
            ball->prev->next = ball->next == NULL ? NULL : ball->next;
          }
          ball = NULL;
        }
        
        if ( ball != NULL ) {
          if (ball->old_pos.x != ball->pos.x || ball->old_pos.y != ball->pos.y) {
            drawBall( ball );
          }
        }
        ball = nball;
      }
    }

    // LAUNCH BALL
    if (ball_list[0]->power == 0) {
      shootCue( ball_list[0] );
    }
  }
}
