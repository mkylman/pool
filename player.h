typedef struct {
  bool stripe;
  bool solid;
  bool choice;
} Player;

Player player[2] = { { false, false, true }, { false, false, true } };

void playerLabel(const char *s, int ypos) {
  int len = strlen(s) * 6;
  tft.setTextColor(WHITE, RED);
  tft.setCursor((WIDTH - len) / 2, ypos);
  tft.print(s);
}
