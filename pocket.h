typedef struct {
  Vector pos;
  uint8_t radius;
} Pocket;

Pocket pocket[6];

Pocket makePocket( Vector pos, uint8_t radius  ) {
  Pocket pocket;
  pocket.pos.x = pos.x;
  pocket.pos.y = pos.y;
  pocket.radius = radius;
  return pocket;
}

void loadPockets(void) {
  // POCKETS
  pocket[0] = makePocket( { BORDER, BORDER }, 16 );
  pocket[1] = makePocket( { WIDTH - BORDER, BORDER }, 16 );
  pocket[2] = makePocket( { BORDER - pocket[1].radius / 2, HEIGHT/2 }, 14 );
  pocket[3] = makePocket( { BORDER, HEIGHT - BORDER }, 16 );
  pocket[4] = makePocket( { WIDTH - BORDER, HEIGHT - BORDER }, 16 );
  pocket[5] = makePocket( { WIDTH - BORDER + pocket[1].radius / 2, HEIGHT/2 }, 14 );
}

void drawPocket(Pocket *pocket) {
  tft.fillCircle( pocket->pos.x, pocket->pos.y,
                  pocket->radius, BLACK );
}

void drawPockets(uint8_t p) {
  // WALLS
  tft.fillRect( 0, 0, BORDER, HEIGHT, RED );
  tft.fillRect( WIDTH-BORDER, 0, BORDER, HEIGHT, RED );
  tft.fillRect( 0, 0, WIDTH, BORDER, RED );
  tft.fillRect( 0, HEIGHT-BORDER, WIDTH, BORDER, RED );

  playerLabel( player[p].choice ? "CHOICE" : (player[p].solid ? "SOLIDS" : "STRIPES"), WIDTH, BORDER / 4 );

  for (int i = 0; i < 6; i++){
    drawPocket( &pocket[i] );
  }
}
