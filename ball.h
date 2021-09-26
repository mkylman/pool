typedef struct Ball {
  Ball *prev;
  Ball *next;
  uint8_t  number;
  uint16_t color;
  Vector   pos;
  Vector   old_pos;
  Vector   vel;
  short    power;
  uint8_t  radius;
  bool     sunk;
} Ball;

Ball *ball_list[4]; // 0 cue 1 eight 2 solid 3 stripe

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

void launchBall(Ball *ball, Vector pos1, Vector pos2, short power) {
  double a, b, c;
  
  a = pos2.x - pos1.x;
  b = pos2.y - pos1.y;
  c = sqrt( (a*a) + (b*b) );
  c = c ? c : 0.1;

  ball->vel.x = (a / c);
  ball->vel.y = (b / c);
  
  ball->power = power;
}

void moveBall(Ball *ball){
  ball->old_pos.x = ball->pos.x;
  ball->old_pos.y = ball->pos.y;
  ball->pos.x += ball->vel.x * (ball->power / 2);
  ball->pos.y += ball->vel.y * (ball->power / 2);
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
      if ( ball != ball2 && ball2->sunk != true ) {
        if ( getDist( ball2->pos, ball->pos ) <= 1 + ball->radius * 2 ) {
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

void aimCue( Vector *p1, Vector *p2, Ball *ball ) {
  Point p = getPoint();
  int x1, y1;
  Vector *p3 = (Vector *)malloc(sizeof(Vector)); // used for target ball pos
  
  while ( !p.touched ) { p = getPoint(); }

  if ( p.touched ) {
    
    while ( p.touched ) {
      x1 = p.x;
      y1 = p.y;
      p2->x = x1;
      p2->y = y1;
      
      tft.drawLine( p1->x, p1->y, p2->x, p2->y, BLACK );
      tft.drawLine( p1->x, p1->y, p2->x, p2->y, DGREEN );
      tft.drawCircle( p2->x, p2->y, ball->radius, WHITE ); // ghost ball
      tft.drawCircle( p2->x, p2->y, ball->radius, DGREEN );

      for (int i = 1; i < 4; i++) { // check if ghost ball "collided", draw estimated trajectory
        Ball *ball2 = ball_list[i];
        while (ball2 != NULL) {
          p3->x = ball2->pos.x;
          p3->y = ball2->pos.y;

          // GHOST BALL AIM SNAP/ASSIST
          // If when aiming we get within range of a ball, this becomes the target ball
          // ghost ball will snap to target and "fine" aiming can be done 
          // I could only figure out getting a half circle, so we alternate where that half circle is
          // depending on where the "entry point" is so that we get the "full" circle
          if ( getDist( ball2->pos, { p2->x, p2->y } ) <= ball2->radius * 2 ) {
            
            playerLabel( "   Fine Aiming Mode   ", WIDTH, HEIGHT - BORDER / 2 );

            // While we're touching, and while we're close enough to the target
            // Get more than 4 balls away and ghost/aim unsnaps
            while( p.touched && getDist( ball2->pos, { p.x, p.y } ) <= ball2->radius * 8 ) {
              double a = p.x - x1;                          // Determine ghost ball movement in relation to pointer, based
              double b = y1 < p.y ? (p.y - y1) : (y1 - p.y);// on initial entry point. This way ensures ghost moves with
              float c = sqrt( (a*a) + (b*b) );              // pointer instead of opposite
              c = c ? c : 0.1;

              // determines ghost ball location
              double a2 = (a / c) * ball->radius * 2; // ghost x, snapped to target
              double b2 = (b / c) * ball->radius * 2; // ghost y, snapped to target
              
              p2->x = p3->x + a2;
              p2->y = p3->y + (p.y < p3->y ? -b2 : b2); // if touch point is above ball, place ghost in top half

              tft.drawLine( p1->x, p1->y, p2->x, p2->y, BLACK );    // line from cue to ghost ball
              tft.drawCircle( p2->x, p2->y, ball->radius, WHITE );  // ghost ball snapped to target ball
              tft.drawCircle( p2->x, p2->y, ball->radius, DGREEN );
              tft.drawLine( p1->x, p1->y, p2->x, p2->y, DGREEN );

              p = getPoint();
            }
          }
          ball2 = ball2->next;
        }
      }
      
      p = getPoint();
    }
    drawBalls();
    
    tft.drawLine( p1->x, p1->y, p2->x, p2->y, BLACK );  // cue to target line
    tft.drawCircle( p2->x, p2->y, ball->radius, WHITE ); // ghost ball
  }

  free(p3);
  playerLabel( "Press & hold for shot power", WIDTH, HEIGHT - BORDER / 2 );
  
  delay(1000);
}

short shotPower(void) {
  Point p = getPoint();
  unsigned long ms2 = millis();
  short power = 20;

  //while ( !p.touched ) { p = getPoint(); }
  
  while ( p.touched ) {
    if ( (millis() - ms2) >= 300 ) {
      if ( power < 50) {
        power += 1;
        tft.drawRect( 0, 0, 10, 30 * 3, WHITE );
        tft.fillRect( 0, 0, 10, (power - 20) * 3, WHITE );
      } else {
        tft.fillRect( 0, 0, 10, 30 * 3, RED );
        power = 20;
      }

      ms2 = millis();
    }

    p = getPoint();
  }
  
  tft.fillRect( 0, 0, 10, 30 * 3, RED );

  return power;
}

bool shootCue(Ball *ball) {

  Vector p1 = { ball->pos.x, ball->pos.y };
  Vector p2 = { 0, 0 }; // used for ghost ball pos

  short power = 0;

  aimCue( &p1, &p2, ball );

  Point p = getPoint();
    
  while ( !p.touched ) { p = getPoint(); }

  // RE-AIM if we touch near the ghost
  while ( true ) {
    if ( p.touched ) {
      if ( getDist( { p.x, p.y }, p2 ) <= ball->radius * 4 ) {
        tft.drawLine( p1.x, p1.y, p2.x, p2.y, DGREEN );     // cue to target line
        tft.drawCircle( p2.x, p2.y, ball->radius, DGREEN ); // ghost ball
        aimCue( &p1, &p2, ball );
      } else { power = shotPower(); break; }
    }
    p = getPoint();
  }

  tft.drawLine( p1.x, p1.y, p2.x, p2.y, DGREEN );     // cue to target line
  tft.drawCircle( p2.x, p2.y, ball->radius, DGREEN ); // ghost ball
  
  launchBall( ball, p1, p2, power );
  return true;
}
