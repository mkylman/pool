#include "colors.h"
#include "point.h"
#include "pool.h"

void setup() {
  tft.begin( tft.readID() );
  tft.fillScreen( DGREEN );

  loadPockets();
  loadBalls();
  drawBalls();
  drawPockets(0);
}

void(* resetFunc)(void) = 0;

void loop() {
  static unsigned long ms = millis();
  for (int p = 0; p < 2; p++) {
    
    putText( p ? "Player 2" : "Player 1" );
    
    bool turn = true;
    while ( turn ) {
      bool shot_made = false;
      bool shot_taken = false;
      Ball *ball = ball_list[0];

      Point point = getPoint();
      drawPockets(p);
      while ( !point.touched ) { point = getPoint(); }
      if ( !(ball->number) && !(ball->power) ) {
        if (ball->sunk) placeCue( ball );
        shot_taken = shootCue( ball );
        drawPockets(p);
      }
      
      do {
        if (millis() - ms >= 1000 / 24) { // "fps"
          ms = millis();
    
          // DRAW, MOVE BALLS, COLLISION CHECKS
  
          for (int i = 0; i < 4; i++) {
            ball = ball_list[i] != NULL ? ball_list[i] : NULL;
            while ( ball != NULL ) {
              Ball *nball = ball->next != NULL ? ball->next : NULL;
              ball->power = ball->power > 0 ? ((ball->power * 5) - 1) / 5 : 0;
            
              // MOVE BALLS
              moveBall(ball);
              
              // EDGE COLLISION
              edgeCollision(ball);
      
              // POCKET COLLISION
              pocketCollision(ball);
      
              // BALL COLLISION
              ballCollision(ball);

              if ( ball->sunk ) {
                if (ball->number == 0) {
                  ball->old_pos = ball->pos;
                  ball->vel.x = 0;
                  ball->vel.y = 0;
                  ball->power = 0;
                  turn = false;
                  if ( (player[p].solid && ball_list[2] == NULL) || (player[p].stripe && ball_list[3] == NULL) ) {
                    putText( p ? "Player 2 loses!" : "Player 1 loses!" );
                    resetFunc();
                  }
                } else {
                  if ( player[p].choice ) {
                    shot_made = true;
                    player[0].choice = false;
                    player[1].choice = false;
                    if ( ball->number < 8 && ball->number ) {
                      player[p].solid = true;
                      player[p].stripe = false;
                      player[p ? 0 : 1].solid = false;
                      player[p ? 0 : 1].stripe = true;
                    } else if ( ball->number > 8 ) {
                      player[p].solid = false;
                      player[p].stripe = true;
                      player[p ? 0 : 1].solid = true;
                      player[p ? 0 : 1].stripe = false;
                    }
                  }
                  
                  if ( (ball->number > 8 && player[p].solid) || (ball->number < 8 && player[p].stripe) ) {
                    if (!shot_made) turn = false;
                  } else {
                    if (turn) shot_made = true;
                  }
                  
                  if ( ball->number == 8 ) {
                    if ( ( player[p].solid && ball_list[2] == NULL )
                      || ( player[p].stripe && ball_list[3] == NULL ) ){
                      putText( p ? "Player 2 wins!" : "Player 1 wins!" );
                      resetFunc();
                    } else {
                      putText( p ? "Player 2 loses!" : "Player 1 loses!" );
                      resetFunc();
                    }
                  }
                    
                  if (ball->next != NULL) {
                    ball->next->prev = (ball->prev == NULL) ? NULL : ball->prev;
                  }
                  if (ball->prev != NULL) {
                    ball->prev->next = (ball->next == NULL) ? NULL : ball->next;
                  } else {
                    ball_list[i] = ball->next != NULL ? ball->next : NULL;
                  }
    
                  free(ball);
                  ball = NULL;
                }
              }
              
              if ( ball != NULL ) {
                if (ball->old_pos.x != ball->pos.x || ball->old_pos.y != ball->pos.y) {
                  drawBall( ball );
                }
              }
              
              ball = nball;
            }
          }
        }
      } while ( balls_moving() );
        
      turn = shot_made;
    }
  }
}