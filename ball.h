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

void pocketCollision(Ball *ball) {
  for (int i = 0; i < 6; i++) {
    double a = ball->pos.x - pocket[i].pos.x;
    double b = ball->pos.y - pocket[i].pos.y;
    float  c = sqrt( (a*a) + (b*b) );
    c = c ? c : 0.1;
    if ( c <= pocket[i].radius + ball->radius ) {
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
        double a = ball2->pos.x - ball->pos.x;
        double b = ball2->pos.y - ball->pos.y;
        float  c = sqrt( (a*a) + (b*b) );
        c = c ? c : 0.1;
        if ( c < ball->radius * 2 ) {
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
          double a = p.x - check->pos.x;
          double b = p.y - check->pos.y;
          float c = sqrt( (a*a) + (b*b) );
          if ( c <= check->radius * 2 ) {
            valid = false;
          }
          check = check->next;
        }
      }
      if (valid) { ball->pos = { p.x, p.y }; ball->sunk = false; drawBall( ball ); delay(300); }
    }
  }
}

void aimCue( Vector *p1, Vector *p2, Vector *p3, Vector *p4, Ball *ball ) {
  Point p = getPoint();
  
  while ( !p.touched ) { p = getPoint(); }
  
  if ( p.touched ) {
    while ( p.touched ) {
      p2->x = p.x;
      p2->y = p.y;
      
      tft.drawLine( p1->x, p1->y, p2->x, p2->y, BLACK );
      tft.drawLine( p1->x, p1->y, p2->x, p2->y, DGREEN );
      tft.drawCircle( p2->x, p2->y, ball->radius, WHITE );
      tft.drawCircle( p2->x, p2->y, ball->radius, DGREEN );
      
      for (int i = 1; i < 4; i++) {
        Ball *ball2 = ball_list[i];
        while (ball2 != NULL) {
          double a = ball2->pos.x - p2->x;
          double b = ball2->pos.y - p2->y;
          float c = sqrt( (a*a) + (b*b) );
          c = c ? c : 0.1;
          if (c <= ball2->radius * 2) {
            p3->x = ball2->pos.x;
            p3->y = ball2->pos.y;
            p4->x = p3->x + (a / c) * 200;
            p4->y = p3->y + (b / c) * 200;
            tft.drawLine( p3->x, p3->y, p4->x, p4->y, BLACK );
            tft.drawLine( p3->x, p3->y, p4->x, p4->y, DGREEN );
          }
          ball2 = ball2->next;
        }
      }
      
      p = getPoint();
    }
    drawBalls();
    tft.drawLine( p1->x, p1->y, p2->x, p2->y, BLACK );
    tft.drawCircle( p2->x, p2->y, ball->radius, WHITE );
    tft.drawLine( p3->x, p3->y, p4->x, p4->y, BLACK );
  }
  
  delay(1000);
}

short shotPower(void) {
  Point p = getPoint();
  unsigned long ms2 = millis();
  short power = 20;

  while ( !p.touched ) { p = getPoint(); }
  
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

  return power;
}

bool shootCue(Ball *ball) {

  Vector p1 = { ball->pos.x, ball->pos.y };
  Vector p2 = { 0, 0 };
  Vector p3 = { 0, 0 };
  Vector p4 = { 0, 0 };

  aimCue( &p1, &p2, &p3, &p4, ball );

  Point p = getPoint();
    
  while ( !p.touched ) { p = getPoint(); }

  while ( true ) {
    if ( p.touched ) {
      double a = p.x - p2.x;
      double b = p.y - p2.y;
      float c = sqrt( (a*a) + (b*b) );
      if ( c <= ball->radius * 3 ) {
        tft.drawLine( p1.x, p1.y, p2.x, p2.y, DGREEN );
        tft.drawCircle( p2.x, p2.y, ball->radius, DGREEN );
        tft.drawLine( p3.x, p3.y, p4.x, p4.y, DGREEN );
        aimCue( &p1, &p2, &p3, &p4, ball );
      } else { break; }
    }
    p = getPoint();
  }
  
  short power = shotPower();

  tft.drawLine( p1.x, p1.y, p2.x, p2.y, DGREEN );
  tft.drawCircle( p2.x, p2.y, ball->radius, DGREEN );
  tft.drawLine( p3.x, p3.y, p4.x, p4.y, DGREEN );
  tft.fillRect( 0, 0, 10, 30 * 3, RED );
  
  launchBall( ball, p1, p2, power );
  return true;
}
