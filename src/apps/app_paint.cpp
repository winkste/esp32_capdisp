// App: Touch-Paint für das runde Display
//   - Äußerer Ring: Farbauswahl (antippen)
//   - Innen: mit dem Finger malen
//   - BOOT-Taste: Zeichenfläche löschen

#include "app.h"
#if APP == APP_PAINT

#include <math.h>

static const int RING_OUTER = 119;
static const int RING_INNER = 100;
static const int BRUSH = 4;

static const uint16_t PALETTE[] = {  // RGB565 – uint32_t würde als RGB888 interpretiert
    TFT_RED, TFT_ORANGE, TFT_YELLOW, TFT_GREEN,
    TFT_CYAN, TFT_BLUE, TFT_MAGENTA, TFT_WHITE,
};
static const int NUM_COLORS = sizeof(PALETTE) / sizeof(PALETTE[0]);
static const float SEG_DEG = 360.0f / NUM_COLORS;

static int currentColor = 0;
static int lastX = -1, lastY = -1;
static bool canvasDirty = false;

static void drawRing() {
  for (int i = 0; i < NUM_COLORS; i++) {
    float a0 = i * SEG_DEG + 1;
    float a1 = (i + 1) * SEG_DEG - 1;
    int inner = (i == currentColor) ? RING_INNER - 6 : RING_INNER;  // Auswahl hervorheben
    lcd.fillArc(CX, CY, RING_OUTER, RING_INNER - 6, a0, a1, TFT_BLACK);
    lcd.fillArc(CX, CY, RING_OUTER, inner, a0, a1, PALETTE[i]);
  }
}

static void clearCanvas() {
  lcd.fillCircle(CX, CY, RING_INNER - 7, TFT_BLACK);
  lcd.setTextColor(TFT_DARKGREY);
  lcd.setTextDatum(middle_center);
  lcd.setFont(&fonts::FreeSansBold9pt7b);
  lcd.drawString("Male mich!", CX, CY);
  canvasDirty = false;
}

void appSetup() {
  pinMode(PIN_BOOT_BTN, INPUT_PULLUP);
  drawRing();
  clearCanvas();
}

void appLoop() {
  if (digitalRead(PIN_BOOT_BTN) == LOW) {
    clearCanvas();
    while (digitalRead(PIN_BOOT_BTN) == LOW) delay(10);
  }

  int32_t x, y;
  if (lcd.getTouch(&x, &y)) {
    float dx = x - CX, dy = y - CY;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist >= RING_INNER - 6) {
      // Winkel wie bei fillArc: 0° = rechts, im Uhrzeigersinn
      float deg = atan2f(dy, dx) * 180.0f / M_PI;
      if (deg < 0) deg += 360.0f;
      int sel = (int)(deg / SEG_DEG) % NUM_COLORS;
      if (sel != currentColor) {
        currentColor = sel;
        drawRing();
      }
      lastX = -1;
    } else if (dist < RING_INNER - 7 - BRUSH) {
      if (!canvasDirty) {
        lcd.fillCircle(CX, CY, RING_INNER - 7, TFT_BLACK);  // Hinweistext entfernen
        canvasDirty = true;
      }
      uint16_t c = PALETTE[currentColor];
      if (lastX >= 0) {
        lcd.drawWideLine(lastX, lastY, x, y, BRUSH, c);
      }
      lcd.fillCircle(x, y, BRUSH, c);
      lastX = x;
      lastY = y;
    }
  } else {
    lastX = -1;
  }

  delay(5);
}

#endif
