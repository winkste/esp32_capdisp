// App: Analoge Uhr
//   - Zifferblatt mit Strichen und Zahlen, Stunden-, Minuten- und Sekundenzeiger
//   - Der Sekundenzeiger läuft flüssig durch
//   - Uhrzeit startet mit der Kompilierzeit (siehe clock_util.h)
//
// Lernpunkt: Das ganze Bild wird in einen Sprite gezeichnet und dann
// komplett übertragen. 8 Bit Farbtiefe halbiert den Speicherbedarf
// (240*240 = 57,6 kB statt 115 kB bei 16 Bit).

#include "app.h"
#if APP == APP_CLOCK_ANALOG

#include <math.h>
#include "clock_util.h"

static const uint16_t COL_FACE = 0x0866;    // dunkles Blau
static const uint16_t COL_TICK = TFT_LIGHTGREY;
static const uint16_t COL_HANDS = TFT_WHITE;
static const uint16_t COL_SECOND = TFT_RED;

static LGFX_Sprite canvas(&lcd);

// Punkt auf einem Kreis; Winkel in Grad, 0° = 12 Uhr, im Uhrzeigersinn
static void polar(float deg, float r, int &x, int &y) {
  float rad = (deg - 90.0f) * DEG_TO_RAD;
  x = CX + (int)(cosf(rad) * r);
  y = CY + (int)(sinf(rad) * r);
}

static void drawHand(float deg, int length, int tail, int width, uint16_t color) {
  int x1, y1, x2, y2;
  polar(deg + 180.0f, tail, x1, y1);
  polar(deg, length, x2, y2);
  canvas.drawWideLine(x1, y1, x2, y2, width, color);
}

static void drawFace() {
  canvas.fillScreen(TFT_BLACK);
  canvas.fillCircle(CX, CY, 119, COL_FACE);

  for (int i = 0; i < 60; i++) {
    int x1, y1, x2, y2;
    bool hour = (i % 5 == 0);
    polar(i * 6.0f, hour ? 100 : 108, x1, y1);
    polar(i * 6.0f, 114, x2, y2);
    canvas.drawWideLine(x1, y1, x2, y2, hour ? 2 : 1, COL_TICK);
  }

  canvas.setFont(&fonts::FreeSansBold12pt7b);
  canvas.setTextDatum(middle_center);
  canvas.setTextColor(COL_TICK);
  const char *nums[] = {"12", "3", "6", "9"};
  for (int i = 0; i < 4; i++) {
    int x, y;
    polar(i * 90.0f, 84, x, y);
    canvas.drawString(nums[i], x, y);
  }
}

void appSetup() {
  canvas.setColorDepth(8);
  if (!canvas.createSprite(SCREEN_W, SCREEN_H)) {
    Serial.println("Fehler: Sprite konnte nicht angelegt werden");
  }
}

void appLoop() {
  float t = clockMillis() / 1000.0f;  // Sekunden seit Mitternacht (mit Nachkommastellen)
  float s = fmodf(t, 60.0f);
  float m = fmodf(t / 60.0f, 60.0f);
  float h = fmodf(t / 3600.0f, 12.0f);

  drawFace();
  drawHand(h * 30.0f, 55, 12, 6, COL_HANDS);    // Stundenzeiger
  drawHand(m * 6.0f, 85, 15, 4, COL_HANDS);     // Minutenzeiger
  drawHand(s * 6.0f, 100, 20, 2, COL_SECOND);   // Sekundenzeiger
  canvas.fillCircle(CX, CY, 6, COL_SECOND);
  canvas.fillCircle(CX, CY, 2, TFT_BLACK);

  canvas.pushSprite(0, 0);
  delay(30);
}

#endif
