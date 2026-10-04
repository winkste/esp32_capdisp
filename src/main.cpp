// Einstiegspunkt: initialisiert das Display, zeigt den Startbildschirm
// und startet die in include/app_config.h ausgewählte App.

#include <Arduino.h>
#include "app.h"

LGFX lcd;

static void showSplash() {
  lcd.fillScreen(TFT_BLACK);
  for (int r = 119; r > 0; r -= 6) {
    lcd.drawCircle(CX, CY, r, lcd.color565(0, 255 - r * 2, r * 2));
  }
  lcd.setTextDatum(middle_center);
  lcd.setTextColor(TFT_WHITE);
  lcd.setFont(&fonts::FreeSansBold18pt7b);
  lcd.drawString("Hallo!", CX, CY - 12);
  lcd.setFont(&fonts::FreeSans9pt7b);
  lcd.drawString("Sporty", CX, CY + 22);
  lcd.drawString(APP_NAME, CX, CY + 44);
  delay(1500);
}

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.setBrightness(200);

  showSplash();
  lcd.fillScreen(TFT_BLACK);
  Serial.printf("Starte App: %s\n", APP_NAME);
  appSetup();
}

void loop() {
  appLoop();
}
