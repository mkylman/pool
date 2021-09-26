typedef struct {
  bool stripe;
  bool solid;
  bool choice;
} Player;

Player player[2] = { { false, false, true }, { false, false, true } };

void playerLabel(const char *s, int xpos, int ypos) {
  int len = strlen(s) * 6;
  tft.setTextColor(WHITE, RED);
  tft.setCursor((xpos - len) / 2, ypos);
  tft.print(s);
}
