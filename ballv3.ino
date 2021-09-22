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
  
  if (millis() - ms >= 1000 / 60) { // "fps"
    ms = millis();

    // DRAW, MOVE BALLS, COLLISION CHECKS
    Ball *ball;
    for (int i = 0; i < 4; i++) {
      ball = ball_list[i];
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
          removeBall( ball );
        }
        
        if ( ball != NULL ) {
          if (ball->old_pos.x != ball->pos.x || ball->old_pos.y != ball->pos.y) {
            drawBall( ball );
          }
        }

        if ( !(ball->number) && !(ball->power) ) {
          shootCue( ball );
        }
        
        ball = nball;
      }
    }
  }
}
