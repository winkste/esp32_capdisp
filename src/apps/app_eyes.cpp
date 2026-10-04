// App: Animierte Augen
//   - Antippen: Augen schließen sich bzw. öffnen sich wieder
//   - Solange der Finger auf dem Display liegt, schauen die Augen dorthin
//   - Sonst blinzeln sie ab und zu und schauen zufällig umher
//
// Lernpunkt: Ein Auge wird zuerst in einen Sprite (Speicherpuffer) gezeichnet
// und dann in einem Rutsch auf das Display kopiert -> kein Flackern.

#include "app.h"
#if APP == APP_EYES

#include <math.h>

static const int EYE_R = 44;               // Radius des Augapfels
static const int IRIS_R = 20;
static const int PUPIL_R = 9;
static const int EYE_DX = 52;              // Abstand Augenmitte <-> Bildmitte
static const int SPR = EYE_R * 2 + 10;     // Kantenlänge des Sprites

static const uint16_t COL_LID = 0x5A69;    // Augenlid (graulila)
static const uint16_t COL_LASH = 0x18C3;   // Lidkante (fast schwarz)
static const uint16_t COL_IRIS = 0x1C9F;   // Iris außen (blau)
static const uint16_t COL_IRIS_IN = 0x5EDF;// Iris innen (hellblau)

static LGFX_Sprite eye(&lcd);

static float openness = 1.0f;  // 0 = zu, 1 = offen
static float target = 1.0f;    // Ziel: auf oder zu (per Tippen)
static bool blinking = false;  // läuft gerade ein automatisches Blinzeln?
static float gazeX = 0, gazeY = 0;    // aktuelle Blickrichtung (-1..1)
static float gazeTX = 0, gazeTY = 0;  // Ziel-Blickrichtung
static bool wasTouched = false;
static uint32_t lastToggle = 0, nextGlance = 0, nextBlink = 0;

// Zeichnet ein Auge in den Sprite und kopiert ihn an die Position (px, py)
static void drawEye(int px, int py) {
  const int c = SPR / 2;
  eye.fillScreen(TFT_BLACK);
  eye.fillCircle(c, c, EYE_R + 4, COL_LID);
  eye.fillCircle(c, c, EYE_R, TFT_WHITE);

  // Iris und Pupille, verschoben je nach Blickrichtung
  float maxOff = EYE_R - IRIS_R - 3;
  int ix = c + (int)(gazeX * maxOff);
  int iy = c + (int)(gazeY * maxOff);
  eye.fillCircle(ix, iy, IRIS_R, COL_IRIS);
  eye.fillCircle(ix, iy, IRIS_R - 6, COL_IRIS_IN);
  eye.fillCircle(ix, iy, PUPIL_R, TFT_BLACK);
  eye.fillCircle(ix - 6, iy - 6, 4, TFT_WHITE);  // Glanzpunkt

  // Lider: Oberlid kommt weiter herunter als das Unterlid hoch,
  // sie treffen sich etwas unterhalb der Mitte.
  float closed = 1.0f - openness;
  int top = -EYE_R + (int)(closed * EYE_R * 1.3f);
  int bottom = EYE_R - (int)(closed * EYE_R * 0.7f);
  for (int dy = -EYE_R; dy <= EYE_R; dy++) {
    if (dy >= top && dy <= bottom) continue;  // dieser Bereich ist offen
    int half = (int)sqrtf(EYE_R * EYE_R - dy * dy);
    eye.drawFastHLine(c - half, c + dy, 2 * half + 1, COL_LID);
  }
  // Lidkante oben
  if (top > -EYE_R) {
    int half = (int)sqrtf(EYE_R * EYE_R - top * top);
    eye.fillRect(c - half, c + top - 1, 2 * half + 1, 3, COL_LASH);
  }

  eye.pushSprite(px, py);
}

void appSetup() {
  if (!eye.createSprite(SPR, SPR)) {
    Serial.println("Fehler: Sprite konnte nicht angelegt werden");
  }
  randomSeed(micros());
  nextBlink = millis() + 3000;
}

void appLoop() {
  uint32_t now = millis();

  // --- Eingabe ---
  int32_t tx, ty;
  bool touched = lcd.getTouch(&tx, &ty);
  if (touched && !wasTouched && now - lastToggle > 300) {
    // neue Berührung -> auf/zu umschalten
    target = (target > 0.5f) ? 0.0f : 1.0f;
    blinking = false;
    lastToggle = now;
  }
  if (touched) {
    // dorthin schauen, wo der Finger ist
    gazeTX = constrain((tx - CX) / 90.0f, -1.0f, 1.0f);
    gazeTY = constrain((ty - CY) / 90.0f, -1.0f, 1.0f);
    nextGlance = now + 1500;
  }
  wasTouched = touched;

  // --- Verhalten, wenn niemand tippt ---
  if (!touched && now > nextGlance) {
    gazeTX = random(-100, 101) / 100.0f;
    gazeTY = random(-60, 61) / 100.0f;
    nextGlance = now + random(800, 3000);
  }
  if (target > 0.5f && !blinking && now > nextBlink) {
    blinking = true;
  }

  // --- Animation: Werte schrittweise dem Ziel annähern ---
  float goal = blinking ? 0.0f : target;
  float speed = blinking ? 0.25f : 0.10f;
  if (openness < goal) openness = min(goal, openness + speed);
  else                 openness = max(goal, openness - speed);
  if (blinking && openness <= 0.0f) {
    blinking = false;  // ganz zu -> wieder öffnen
    nextBlink = now + random(2500, 6000);
  }
  gazeX += (gazeTX - gazeX) * 0.25f;
  gazeY += (gazeTY - gazeY) * 0.25f;

  // --- Zeichnen ---
  drawEye(CX - EYE_DX - SPR / 2, CY - SPR / 2);
  drawEye(CX + EYE_DX - SPR / 2, CY - SPR / 2);

  delay(20);  // ca. 30-40 Bilder pro Sekunde
}

#endif
