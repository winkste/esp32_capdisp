// App: Digitale Uhr
//   - Stunden und Minuten groß in 7-Segment-Schrift, blinkender Doppelpunkt
//   - Sekunden als Ring am Rand (60 Segmente)
//   - Uhrzeit startet mit der Kompilierzeit (siehe clock_util.h)
//
// Lernpunkt: Es wird nur neu gezeichnet, was sich geändert hat.

#include "app.h"
#if APP == APP_CLOCK_DIGITAL

#include "clock_util.h"

static const int RING_OUTER = 118;
static const int RING_INNER = 106;
static const uint16_t COL_DIGITS = TFT_CYAN;
static const uint16_t COL_RING_ON = TFT_CYAN;
static const uint16_t COL_RING_OFF = 0x2104;  // sehr dunkles Grau

static int lastSec = -1;
static int lastMin = -1;

// Sekundensegment i (0 = oben, im Uhrzeigersinn)
static void drawSecondSegment(int i, uint16_t color) {
  // fillArc: 0° = rechts, daher -90° damit Sekunde 0 oben liegt
  float a0 = i * 6.0f - 90.0f + 0.8f;
  float a1 = a0 + 6.0f - 1.6f;
  lcd.fillArc(CX, CY, RING_OUTER, RING_INNER, a0, a1, color);
}

static void drawColon(bool on) {
  uint16_t c = on ? COL_DIGITS : TFT_BLACK;
  lcd.fillCircle(CX, CY - 15, 5, c);
  lcd.fillCircle(CX, CY + 15, 5, c);
}

static void drawHoursMinutes(int h, int m) {
  char buf[3];
  lcd.fillRect(16, CY - 40, SCREEN_W - 32, 80, TFT_BLACK);
  lcd.setFont(&fonts::Font7);  // 7-Segment-Schrift, nur Ziffern
  lcd.setTextSize(1.3f);
  lcd.setTextColor(COL_DIGITS);

  snprintf(buf, sizeof(buf), "%02d", h);
  lcd.setTextDatum(middle_right);
  lcd.drawString(buf, CX - 14, CY);

  snprintf(buf, sizeof(buf), "%02d", m);
  lcd.setTextDatum(middle_left);
  lcd.drawString(buf, CX + 14, CY);
  lcd.setTextSize(1);
}

static void drawSeconds(int s) {
  char buf[3];
  snprintf(buf, sizeof(buf), "%02d", s);
  lcd.fillRect(CX - 25, CY + 48, 50, 26, TFT_BLACK);
  lcd.setFont(&fonts::FreeSansBold12pt7b);
  lcd.setTextDatum(middle_center);
  lcd.setTextColor(TFT_DARKGREY);
  lcd.drawString(buf, CX, CY + 60);
}

void appSetup() {
  lcd.setFont(&fonts::FreeSans9pt7b);
  lcd.setTextDatum(middle_center);
  lcd.setTextColor(TFT_DARKGREY);
  lcd.drawString("Uhrzeit", CX, CY - 62);
}

void appLoop() {
  uint32_t t = clockMillis() / 1000;
  int s = t % 60;
  int m = (t / 60) % 60;
  int h = t / 3600;

  if (s != lastSec) {
    // Sekundenring: bei 0 (oder beim Start) komplett neu, sonst nur ein Segment
    if (s == 0 || lastSec < 0) {
      for (int i = 0; i < 60; i++) {
        drawSecondSegment(i, i <= s ? COL_RING_ON : COL_RING_OFF);
      }
    } else {
      drawSecondSegment(s, COL_RING_ON);
    }
    if (m != lastMin) {
      drawHoursMinutes(h, m);
      lastMin = m;
    }
    drawColon(s % 2 == 0);
    drawSeconds(s);
    lastSec = s;
  }

  delay(20);
}

#endif
