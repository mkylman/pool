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

void removeBall( Ball *ball ) {
  if (ball->next != NULL) {
    ball->next->prev = ball->prev == NULL ? NULL : ball->prev;
  }
  if (ball->prev != NULL) {
    ball->prev->next = ball->next == NULL ? NULL : ball->next;
  }
  ball = NULL;
}

void loadBalls(void) {
  uint16_t colors[7] = { YELLOW, BLUE, RED, VIOLET, ORANGE, GREEN, MAGENTA };

  for (int i = 0; i < 4; i++) {
    ball_list[i] = (Ball *)malloc(sizeof(Ball));
  }

  ball_list[0] = makeBall( NULL, WHITE, { WIDTH / 2, HEIGHT - (HEIGHT / 8) }, 0 );
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

/*  
  tft.drawLine( ball->pos.x, ball->pos.y,
                ball->pos.x + ball->vel.x * 100,
                ball->pos.y + ball->vel.y * 100, BLACK );
*/  
  ball->power = power;
}

void moveBall(Ball *ball){
  ball->old_pos.x = ball->pos.x;
  ball->old_pos.y = ball->pos.y;
  ball->pos.x += ball->vel.x * ball->power;
  ball->pos.y += ball->vel.y * ball->power;
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

void pocketCollision(Ball *ball) {
  for (int i = 0; i < 6; i++) {
    double a = ball->pos.x - pocket[i].pos.x;
    double b = ball->pos.y - pocket[i].pos.y;
    float  c = sqrt( (a*a) + (b*b) );
    c = c ? c : 0.1;
    if ( c <= pocket[i].radius + ball->radius ) {
      ball->sunk = true;
      tft.fillCircle(ball->old_pos.x, ball->old_pos.y, ball->radius, DGREEN);
      if (ball->number == 0){
        ball->pos.x = WIDTH / 2;
        ball->pos.y = HEIGHT - HEIGHT / 8;
        ball->vel.x = 0;
        ball->vel.y = 0;
        ball->power = 0;
        ball->sunk = false;
        tft.fillScreen( DGREEN );
        drawPockets();
        drawBalls();
      }
    }
  }
}

void ballCollision(Ball *ball) {
  for (int j = 0; j < 4; j++) {
    Ball *ball2 = ball_list[j];
    while ( ball2 != NULL ){
      if ( ball != ball2 && ball2->sunk != true ) {
        double a = ball2->pos.x - ball->pos.x;
        double b = ball2->pos.y - ball->pos.y;
        float  c = sqrt( (a*a) + (b*b) );
        c = c ? c : 0.1;
        if ( c < ball->radius * 2 ) {
          launchBall( ball2, ball->pos, ball2->pos,
                      ball->power ? ball->power - (ball->power / 8) : ball2->power );
          
          ball->vel.x -= ball2->vel.x;
          ball->vel.y -= ball2->vel.y;
        }
      }
      ball2 = ball2->next;
    }
  }
}

void shootCue(Ball *ball) {
  Point p = getPoint();

  if ( p.touched ) {
    drawBalls();
    
    uint8_t dist = ball->radius * 3;
    if ( ( p.x <= ball->pos.x + dist )
      && ( p.x >= ball->pos.x - dist ) 
      && ( p.y <= ball->pos.y + dist )
      && ( p.y >= ball->pos.y - dist ) ) {
      double x1, x2, y1, y2;
      double a, b, c;
      unsigned long ms2 = millis();
      short power = 2;
      bool inc = true;
      
      x1 = p.x;
      y1 = p.y;
      
      while ( p.touched ) {
        if ( (millis() - ms2) >= 200 ) {
          if ( power >= 50 ) inc = false;
          if ( power <= 2 ) inc = true;
          if (inc) {
            power += 2;
            tft.drawRect( 0, 0, 10, 50 * 3, WHITE );
            tft.fillRect( 0, 0, 10, power * 3, WHITE );
          } else {
            tft.fillRect( 0, 0, 10, 50 * 3, RED );
            power = 2;
          }

          ms2 = millis();
        }
        
        x2 = p.x;
        y2 = p.y;
        p = getPoint();
      }
      tft.fillRect( 0, 0, 10, 50 * 3, RED );
      launchBall( ball, { x1, y1 }, { x2, y2 }, power );
      drawPockets();
      drawBalls();
    }
  }
}
