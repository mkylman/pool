typedef struct Ball {
  Ball *prev;
  Ball *next;
  uint8_t  number;
  uint16_t color;
  Vector   pos;
  Vector   old_pos;
  Vector   vel;   // direction
  uint16_t power; // acceleration/speed in cm/s - 0 to 1000
  uint8_t  radius;
  bool     sunk;
} Ball;

Ball *ball_list[4]; // 0 cue 1 eight 2 solid 3 stripe

Ball *makeBall( Ball *, uint16_t, Vector, uint8_t );

float getDist( Vector, Vector );
void loadBalls(void);
void drawBall(Ball *);
void drawBalls(void);
void launchBall(Ball *, Vector, Vector, uint16_t);
void moveBall(Ball *);
void edgeCollision(Ball *);
void pocketCollision(Ball *);
void ballCollision(Ball *);
void placeCue(Ball *);
void aimCue( Vector *, Ball *);
uint16_t shotPower(void);
bool shootCue(Ball *);

Ball *makeBall( Ball *pball, uint16_t color, Vector pos, uint8_t num ) {
  Ball *ball = (Ball *)malloc(sizeof(Ball));
  ball->prev = pball;
  ball->next = NULL;
  ball->number = num;
  ball->color = color;
  ball->radius = 8;
  ball->pos.x = pos.x;
  ball->pos.y = pos.y;
  ball->vel.x = ball->vel.y = 0;
  ball->old_pos.x = ball->old_pos.y = 0;
  ball->sunk = false;
  ball->power = 0;

  return ball;
}

void loadBalls(void) {
  uint16_t colors[7] = { YELLOW, BLUE, RED, VIOLET, ORANGE, GREEN, MAGENTA };

  for (int i = 0; i < 4; i++) {
    ball_list[i] = (Ball *)malloc(sizeof(Ball));
  }

  ball_list[0] = makeBall( NULL, WHITE, { WIDTH / 2, HEIGHT - (HEIGHT / 6) }, 0 );
  uint8_t radius = ball_list[0]->radius;
  
  ball_list[1] = makeBall( NULL, BLACK, { WIDTH / 2, HEIGHT / 6 + radius * 4 }, 8 );

  
  Vector sol_pos[7] = {
    { WIDTH /2,               HEIGHT / 6 + radius * 8 },
    { WIDTH / 2 - radius,     HEIGHT / 6 + radius * 6 },
    { WIDTH / 2 - radius * 3, HEIGHT / 6 + radius * 2 },
    { WIDTH / 2 + radius,     HEIGHT / 6 + radius * 2 },
    { WIDTH / 2 + radius * 4, HEIGHT / 6 },
    { WIDTH / 2 - radius * 4, HEIGHT / 6 },
    { WIDTH / 2,              HEIGHT / 6 }
  };

  Vector str_pos[7] = {
    { WIDTH / 2 + radius,     HEIGHT / 6 + radius * 6 },
    { WIDTH / 2 - radius * 2, HEIGHT / 6 + radius * 4 },
    { WIDTH / 2 + radius * 2, HEIGHT / 6 + radius * 4 },
    { WIDTH / 2 + radius * 3, HEIGHT / 6 + radius * 2 },
    { WIDTH / 2 - radius,     HEIGHT / 6 + radius * 2 },
    { WIDTH / 2 + radius * 2, HEIGHT / 6 },
    { WIDTH / 2 - radius * 2, HEIGHT / 6 }
  };
  
  if (ball_list[2] != NULL && ball_list[3] != NULL) {
    Ball *strp = NULL, *solp = NULL;
    for (int i = 0; i < 7; i++) {
      Ball *strb = makeBall( strp, colors[i], str_pos[i], i+9 );
      Ball *solb = makeBall( solp, colors[i], sol_pos[i], i+1 );
      if (strp != NULL) strp->next = strb;
      else {
        strp->prev = NULL;
        ball_list[3] = strb;
      }
      if (solp != NULL) solp->next = solb;
      else {
        strp->prev = NULL;
        ball_list[2] = solb;
      }
      strp = strb;
      solp = solb;
    }
  }
}

void drawBall(Ball *ball) {
  if (ball->sunk != true) {
    tft.fillCircle( ball->old_pos.x, ball->old_pos.y,
                    ball->radius, DGREEN );
    if (ball->number > 8) { // "stripe" some balls
      tft.fillCircle( ball->pos.x, ball->pos.y,
                    ball->radius, WHITE );
      tft.fillCircle( ball->pos.x, ball->pos.y,
                    ball->radius/2, ball->color );
    } else {
      tft.fillCircle( ball->pos.x, ball->pos.y,
                      ball->radius, ball->color );
    }
  }
}

void drawBalls(void) {
  for (int i = 0; i < 4; i++) {
    Ball *ball = ball_list[i];
    while ( ball != NULL ) {
      drawBall( ball );
      ball = ball->next;
    }
  }
}

void launchBall(Ball *ball, Vector pos1, Vector pos2, uint16_t power) {
  double a, b, c;
  
  a = pos2.x - pos1.x;
  b = pos2.y - pos1.y;
  c = sqrt( (a*a) + (b*b) );
  c = c ? c : 0.1;       // values of vel.x and vel.y will be between -1 and 1 after we divide them by the distance
  ball->vel.x = (a / c); // normalizing the pair, such that sqrt(vel.x ^ 2 + vel.y ^ 2) = 1
  ball->vel.y = (b / c); // here we are calculating the angle at which we will move
                         // these values times power will yield our pixels per 33.3ms (1000/30)
                         // values will be between -1 and 1
  
  ball->power = power;
}

void moveBall(Ball *ball){
  ball->old_pos.x = ball->pos.x;
  ball->old_pos.y = ball->pos.y;                   // 1m/s = 180px/s -> 33ms tick, 6px/tick | 17ms tick, 3px/tick
  ball->pos.x += ball->vel.x * (ball->power / 60); // divide by 60, tick is 1000/120ms
  ball->pos.y += ball->vel.y * (ball->power / 60); // and power is cm/s...
  
}

void edgeCollision(Ball *ball) {
  if (ball->pos.x <= BORDER + ball->radius) {
    ball->pos.x += ball->radius;
    ball->vel.x *= -1;
  }
  if (ball->pos.x >= WIDTH - BORDER - ball->radius) {
    ball->pos.x -= ball->radius;
    ball->vel.x *= -1;
  }
  if (ball->pos.y <= BORDER + ball->radius) {
    ball->pos.y += ball->radius;
    ball->vel.y *= -1;
  }
  if (ball->pos.y >= HEIGHT - BORDER - ball->radius) {
    ball->pos.y -= ball->radius;
    ball->vel.y *= -1;
  }
}

float getDist( Vector pos1, Vector pos2 ) {
  double a = pos1.x - pos2.x; 
  double b = pos1.y - pos2.y;
  float  c = sqrt( (a*a) + (b*b) );
  
  return (c ? c : 0.1);
}

void pocketCollision(Ball *ball) {
  for (int i = 0; i < 6; i++) {
    if ( getDist( ball->pos, pocket[i].pos ) <= pocket[i].radius + ball->radius ) {
      ball->sunk = true;
      tft.fillCircle(ball->old_pos.x, ball->old_pos.y, ball->radius, DGREEN);
    }
  }
}

void ballCollision(Ball *ball) {
  for (int j = 0; j < 4; j++) {
    Ball *ball2 = ball_list[j];
    while ( ball2 != NULL ){
      if ( ball->number != ball2->number && ball2->sunk != true ) {
        if ( getDist( ball2->pos, ball->pos ) <= ball->radius * 2 ) {
          launchBall( ball2, ball->pos, ball2->pos,
                      ball->power ? ball->power : ball2->power );
          
          ball->vel.x -= ball2->vel.x;
          ball->vel.y -= ball2->vel.y;
        }
      }
      ball2 = ball2->next;
    }
  }
}


/*
 * placeCue( Ball *);
 * checks if we're within the table boundary and not on another ball. If these
 * conditions are met, the cue ball is positioned wherever we touch.
 */
void placeCue(Ball *ball) {
  Point p = getPoint();
  while ( !p.touched ) p = getPoint();

  if ( p.touched ) {
    if ( p.y < HEIGHT - BORDER && p.y > BORDER && p.x < WIDTH - BORDER && p.x > BORDER ) {
      bool valid = true;
      for (int i = 1; i < 4; i++) {
        Ball *check = ball_list[i];
        while (check != NULL && valid) {
          if ( getDist( { p.x, p.y }, check->pos ) <= check->radius * 2 ) {
            valid = false;
          }
          check = check->next;
        }
      }
      if (valid) { ball->pos = { p.x, p.y }; ball->sunk = false; drawBall( ball ); delay(300); }
    }
  }
}


/*
 * aimCue( Vector *, Vector *, Ball * );
 * This function could probably be broken down into smaller functions
 * 
 * ghost is used for the ghost ball position and adjusted when snapping, which translates to our "finer" adjusted final aim
 * 
 * Waits until we touch the screen. When we do a line from the cue ball to where we are touching,
 * where a ghost ball is also drawn.
 * Iterate through all the balls and perform collision check with the ghost ball
 * If ghost collides with a ball we want to snap to its perimeter.
 * Initial contact point is stored in x1, y1
 * Ghost snapped position is calculated based on current touch point's distance from the initial contact point and the slope
 * of the line between them. Those get used to adjust ghost ball position(ghost) relative to the target ball and snapped to perimeter
 * I could only get position to work in semi-circles, so the ghost ball y position flips depending on
 * touch point position relative to initial contact point, to get the full circle
 */
void aimCue( Vector *ghost, Vector *target, Vector *est_line, Ball *ball ) {
  Point p = getPoint();
  int x1, y1;

  // Wait until we touch the screen
  while ( !p.touched ) { p = getPoint(); }

  if ( p.touched ) {
    
    while ( p.touched ) {
      x1 = p.x;
      y1 = p.y;
      
      ghost->x = x1;
      ghost->y = y1;
      target->x = est_line->x = 0;
      target->y = est_line->y = 0;

      // Snap aim to side rails and draw estimate
      bool hcoll = false, vcoll = false;
      if (x1 <= BORDER + ball->radius) {
        ghost->x = BORDER + ball->radius;
        hcoll = true;
      }
      
      if (x1 >= WIDTH - BORDER - ball->radius) {
        ghost->x = WIDTH - BORDER - ball->radius;
        hcoll = true;
      }
      
      if (y1 <= BORDER + ball->radius) {
        ghost->y = BORDER + ball->radius;
        vcoll = true;
      }
      if (y1 >= HEIGHT - BORDER - ball->radius) {
        ghost->y = HEIGHT - BORDER - ball->radius;
        vcoll = true;
      }

      if (hcoll) {
        double a = ball->pos.x - ghost->x;
        double b = ball->pos.y - ghost->y;
        float c = sqrt( (a*a) + (b*b) );
        c = c ? c : 0.1;
        target->x = ghost->x;
        target->y = ghost->y;
        est_line->x = target->x + (a / c) * 300;
        est_line->y = target->y - (b / c) * 300;
      }

      if (vcoll) {
        double a = ball->pos.x - ghost->x;
        double b = ball->pos.y - ghost->y;
        float c = sqrt( (a*a) + (b*b) );
        c = c ? c : 0.1;
        target->x = ghost->x;
        target->y = ghost->y;
        est_line->x = target->x - (a / c) * 300;
        est_line->y = target->y + (b / c) * 300;
      }

      for (int i = 1; i < 4; i++) { // check if ghost ball collided
        Ball *ball2 = ball_list[i];
        while (ball2 != NULL) {
          // GHOST BALL AIM SNAP/ASSIST
          // If when aiming we get within range of a ball, this becomes the target ball
          // ghost ball will snap to target and "fine" aiming can be done 
          if ( getDist( ball2->pos, { p.x, p.y } ) <= ball2->radius * 2 ) {
            
            putText2( "   Fine Aiming Mode   ", HEIGHT - BORDER / 2, RED );

            // While we're touching screen and we stay close enough to the target(4 ball distance) aim will stay snapped
            while( p.touched && getDist( ball2->pos, { p.x, p.y } ) <= ball2->radius * 8 ) {
              double a = p.x - x1;                          // Determine ghost ball movement in relation to pointer and target
              double b = y1 < p.y ? (p.y - y1) : (y1 - p.y);// This line ensures ghost moves with pointer, instead of opposite to
              float c = sqrt( (a*a) + (b*b) );
              c = c ? c : 0.1;

              // determines ghost ball location
              double a2 = (a / c) * (ball->radius - 1) * 2; // ghost x, snapped to target
              double b2 = (b / c) * (ball->radius - 1) * 2; // ghost y, snapped to target

              ghost->x = ball2->pos.x;
              ghost->y = ball2->pos.y;
              
              ghost->x += a2;
              ghost->y += (p.y < ball2->pos.y) ? -1 * b2 : b2; // if touch point is above ball, place ghost in top half

              target->x = (ball2->pos.x - ghost->x);
              target->y = (ball2->pos.y - ghost->y);
              float c3 = sqrt( (target->x*target->x) + (target->y*target->y) );
              c3 = c3 ? c3 : 0.1;

              est_line->x = ball2->pos.x + (target->x / c3) * 200;
              est_line->y = ball2->pos.y + (target->y / c3) * 200;

              target->x = ball2->pos.x;
              target->y = ball2->pos.y;
              
              tft.drawLine( ball->pos.x, ball->pos.y, ghost->x, ghost->y, BLACK );    // line from cue to ghost ball
              tft.drawLine( target->x, target->y, est_line->x, est_line->y, BLACK ); // estimate line?
              tft.drawCircle( ghost->x, ghost->y, ball->radius, WHITE );  // ghost ball snapped to target ball
              tft.drawCircle( ghost->x, ghost->y, ball->radius, DGREEN );
              tft.drawLine( ball->pos.x, ball->pos.y, ghost->x, ghost->y, DGREEN );
              tft.drawLine( target->x, target->y, est_line->x, est_line->y, DGREEN ); // estimate line

              p = getPoint();
            }
          }
          ball2 = ball2->next;
        }
      }

      // draw ghost ball and estimate line while aiming
      tft.drawLine( ball->pos.x, ball->pos.y, ghost->x, ghost->y, BLACK );
      tft.drawLine( ball->pos.x, ball->pos.y, ghost->x, ghost->y, DGREEN );
      tft.drawCircle( ghost->x, ghost->y, ball->radius, WHITE ); // ghost ball
      tft.drawCircle( ghost->x, ghost->y, ball->radius, DGREEN );
      tft.drawLine( target->x, target->y, est_line->x, est_line->y, BLACK ); // estimate line
      tft.drawLine( target->x, target->y, est_line->x, est_line->y, DGREEN ); // estimate line
      
      p = getPoint();
    }
    drawBalls();
    drawPockets();
    
    tft.drawLine( ball->pos.x, ball->pos.y, ghost->x, ghost->y, BLACK );  // cue to target line
    tft.drawCircle( ghost->x, ghost->y, ball->radius, WHITE ); // ghost ball
    tft.drawLine( target->x, target->y, est_line->x, est_line->y, BLACK ); // estimate line
  }

  putText2( "Press & hold for shot power", HEIGHT - BORDER / 2, RED );
  
  delay(1000);
}

uint16_t shotPower(void) {
  Point p = getPoint();
  uint16_t power = 200;
  const int pwr_max = 1400;
  const int pwr_min = 200;
  Vector pt1 = { 0, 0 };
  float distance;
  //while ( !p.touched ) { p = getPoint(); }
  if ( p.touched ) {
    pt1.y = p.y;
    pt1.x = p.x;
  }
  while ( p.touched ) {

    Vector pt2 = { p.x, p.y };
    distance = getDist( pt1, pt2 );

    power = map( distance, 0, HEIGHT - BORDER * 2, pwr_min, pwr_max ); 
    
    uint16_t color = WHITE;
    if (power >= 200 && power <= 400) {
      color = WHITE;
    } else if (power > 400 && power <= 600) {
      color = BLUE;
    } else if (power > 600 && power <= 800) {
      color = YELLOW;
    } else if (power > 800 && power <= 1000) {
      color = ORANGE;
    } else if (power > 1000) {
      color = MAGENTA;
    }
  
    tft.fillRect( 0, 0, 10, 100, RED );  
    tft.drawRect( 0, 0, 10, 100, WHITE );
    tft.fillRect( 0, 0, 10, map(power, pwr_min, pwr_max, 0, 100), color);

    p = getPoint();
  }
  
  tft.fillRect( 0, 0, 10, 100, RED );

  return power;
}

bool shootCue(Ball *ball) {

  Vector p1 = { ball->pos.x, ball->pos.y }; // cue ball position
  Vector ghost = { 0, 0 }; // used for ghost ball position
  Vector target = { 0, 0 }; // used for target ball position, startpoint of estimate line
  Vector est_line = { 0, 0 }; // used for endpoint of estimate line

  uint16_t power = 0;

  aimCue( &ghost, &target, &est_line, ball );

  Point p = getPoint();
    
  while ( !p.touched ) { p = getPoint(); }

  // RE-AIM if we touch near the ghost
  while ( true ) {
    if ( p.touched ) {
      if ( getDist( { p.x, p.y }, ghost ) <= ball->radius * 4 ) {
        tft.drawLine( p1.x, p1.y, ghost.x, ghost.y, DGREEN );     // cue to target line
        tft.drawCircle( ghost.x, ghost.y, ball->radius, DGREEN ); // ghost ball
        tft.drawLine( target.x, target.y, est_line.x, est_line.y, DGREEN ); // estimate line
        aimCue( &ghost, &target, &est_line, ball );
      } else { power = shotPower(); break; }
    }
    p = getPoint();
  }

  tft.drawLine( p1.x, p1.y, ghost.x, ghost.y, DGREEN );     // cue to target line
  tft.drawCircle( ghost.x, ghost.y, ball->radius, DGREEN ); // ghost ball
  tft.drawLine( target.x, target.y, est_line.x, est_line.y, DGREEN ); // estimate line

  launchBall( ball, p1, ghost, power );
  return true;
}
