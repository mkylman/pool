#define BORDER 20

typedef struct {
  Vector pos;
  uint8_t radius;
} Pocket;

Pocket pocket[6];

Pocket makePocket( Vector pos  ) {
  Pocket pocket;
  pocket.pos.x = pos.x;
  pocket.pos.y = pos.y;
  pocket.radius = 12;
  return pocket;
}

void loadPockets(void) {
  // POCKETS
  pocket[0] = makePocket( { BORDER, BORDER } );
  pocket[1] = makePocket( { WIDTH - BORDER, BORDER } );
  pocket[2] = makePocket( { BORDER, HEIGHT/2 } );
  pocket[3] = makePocket( { BORDER, HEIGHT - BORDER } );
  pocket[4] = makePocket( { WIDTH - BORDER, HEIGHT - BORDER } );
  pocket[5] = makePocket( { WIDTH - BORDER, HEIGHT/2 } );
}

void drawPocket(Pocket *pocket) {
  tft.fillCircle( pocket->pos.x, pocket->pos.y,
                  pocket->radius, BLACK );
}

void drawPockets(void) {
  // WALLS
  tft.fillRect( 0, 0, BORDER, HEIGHT, RED );
  tft.fillRect( WIDTH-BORDER, 0, BORDER, HEIGHT, RED );
  tft.fillRect( 0, 0, WIDTH, BORDER, RED );
  tft.fillRect( 0, HEIGHT-BORDER, WIDTH, BORDER, RED );
  for (int i = 0; i < 6; i++){
    drawPocket( &pocket[i] );
  }
}
