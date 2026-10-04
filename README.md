# ESP32 CapDisp – Beispiele für das runde Touch-Display

Ein Lern- und Spielprojekt für das **ESP32-2424S012C**: ein kleines Board mit
ESP32-C3 und einem runden 1,28"-Display (240 × 240 Pixel) mit Touch.
Das Projekt enthält mehrere Beispiel-Apps. Mit einem einzigen `#define`
wählst du aus, welche App gebaut und auf das Board geflasht wird.

---

## 1. Das Projekt

### Hardware

| Komponente | Details                                                   |
|------------|-----------------------------------------------------------|
| Prozessor  | ESP32-C3 (RISC-V, 160 MHz, 400 kB RAM, 4 MB Flash)        |
| Display    | GC9A01, rund, 240 × 240 Pixel, angeschlossen über SPI     |
| Touch      | CST816D, kapazitiv, angeschlossen über I²C (Adresse 0x15) |
| Taste      | BOOT-Taste an GPIO 9 (gedrückt = LOW)                     |
| USB        | USB-C, direkt am ESP32-C3 (Serial über USB-CDC)           |

Die vollständige Pinbelegung steht in [include/board.h](include/board.h).

### Verzeichnisstruktur

```
esp32_capdisp/
├── platformio.ini            Build-Konfiguration (Board, Bibliotheken, Ports)
├── include/
│   ├── board.h               Pins + Display-/Touch-Konfiguration (LovyanGFX)
│   ├── app_config.h          ← HIER wird die App ausgewählt
│   ├── app.h                 gemeinsame Schnittstelle aller Apps
│   └── clock_util.h          Software-Uhr für die Uhren-Apps
└── src/
    ├── main.cpp              Start: Display einschalten, Splash, App starten
    └── apps/
        ├── app_paint.cpp         Malspiel
        ├── app_eyes.cpp          Animierte Augen
        ├── app_clock_digital.cpp Digitaluhr
        └── app_clock_analog.cpp  Analoguhr
```

### Verwendete Bibliothek

[LovyanGFX](https://github.com/lovyan03/LovyanGFX) übernimmt Display und Touch.
Sie ist schnell, kann Sprites, Schriften und Kreisbögen zeichnen und versteht
den Touch-Controller direkt. PlatformIO lädt sie beim ersten Build automatisch
herunter.

---

## 2. Entwicklungsumgebung aufsetzen

### Variante A: VS Code + PlatformIO (empfohlen)

1. [Visual Studio Code](https://code.visualstudio.com/) installieren.
2. In VS Code links auf **Extensions** klicken, nach **PlatformIO IDE**
   suchen und die Erweiterung installieren. Danach VS Code neu starten.
   Die erste Installation dauert ein paar Minuten, weil PlatformIO seine
   Werkzeuge (Python-Umgebung, Compiler) herunterlädt.
3. Das Projekt holen:
   ```bash
   git clone /Volumes/git_server/esp32_capdisp
   ```
4. In VS Code **File → Open Folder…** und den Ordner `esp32_capdisp` öffnen.
   PlatformIO erkennt das Projekt an der `platformio.ini` und lädt beim
   ersten Build die ESP32-Plattform und LovyanGFX herunter.

### Variante B: nur Kommandozeile

```bash
# PlatformIO Core installieren (z. B. über pipx oder Homebrew)
brew install platformio
# oder:
pipx install platformio
```

Wenn die VS-Code-Erweiterung schon installiert ist, gibt es PlatformIO auch hier:
`~/.platformio/penv/bin/pio`

### USB-Verbindung prüfen

Das Board per USB-C anschließen. Unter macOS erscheint es als
`/dev/cu.usbmodem…`:

```bash
ls /dev/cu.usbmodem*
```

Die `platformio.ini` sucht mit `/dev/cu.usbmodem*` automatisch nach diesem
Port. Unter Linux oder Windows musst du `upload_port` und `monitor_port`
anpassen (z. B. `/dev/ttyACM0` bzw. `COM5`).

---

## 3. Bauen und Flashen

### App auswählen

In [include/app_config.h](include/app_config.h) die gewünschte App eintragen:

```cpp
#define APP APP_EYES   // APP_PAINT, APP_EYES, APP_CLOCK_DIGITAL, APP_CLOCK_ANALOG
```

### In VS Code

In der blauen PlatformIO-Statusleiste unten:

| Symbol | Aktion                                         |
|--------|------------------------------------------------|
| ✓      | **Build**: nur kompilieren                     |
| →      | **Upload**: kompilieren und flashen            |
| 🔌     | **Serial Monitor**: Ausgaben des Boards lesen  |
| 🗑     | **Clean**: Build-Ordner löschen                |

### Auf der Kommandozeile

```bash
pio run                     # bauen
pio run -t upload           # bauen + flashen
pio device monitor          # serielle Ausgabe ansehen (Strg+C beendet)
pio run -t upload -t monitor   # alles in einem
```

Eine App auswählen, ohne `app_config.h` zu ändern:

```bash
PLATFORMIO_BUILD_FLAGS="-DAPP=APP_CLOCK_ANALOG" pio run -t upload
```

### Wenn das Flashen nicht klappt

- **Port nicht gefunden:** USB-Kabel prüfen. Manche Kabel können nur laden
  und übertragen keine Daten.
- **Upload bricht ab / „No serial data received“:** **BOOT-Taste gedrückt
  halten**, USB einstecken (oder RESET drücken), BOOT loslassen. Das Board
  wartet dann im Download-Modus. Danach erneut flashen.
- **Bildstörungen:** In `board.h` den Wert `cfg.freq_write` von `80000000`
  auf `40000000` senken.
- **Serial Monitor bleibt leer:** Nach dem Flashen einmal RESET drücken.
  Nötig sind `ARDUINO_USB_CDC_ON_BOOT=1` (steht schon in der `platformio.ini`)
  und 115200 Baud.

---

## 4. Die Apps

Nach dem Einschalten zeigt jede App 1,5 Sekunden lang einen Startbildschirm
mit dem Namen der App. Danach startet die App.

### 🎨 Malspiel – `APP_PAINT`

Datei: [src/apps/app_paint.cpp](src/apps/app_paint.cpp)

- Am Rand liegt ein **Farbring** mit 8 Farben. Ein Segment antippen wählt die
  Farbe aus. Das gewählte Segment wird breiter dargestellt.
- In der **Innenfläche** malst du mit dem Finger.
- Die **BOOT-Taste** löscht die Zeichenfläche.

### 👀 Augen – `APP_EYES`

Datei: [src/apps/app_eyes.cpp](src/apps/app_eyes.cpp)

- Zwei animierte Augen mit Iris, Pupille und Glanzpunkt.
- **Antippen:** Die Augen schließen sich langsam. Erneutes Antippen öffnet
  sie wieder.
- **Finger liegen lassen:** Die Augen schauen dorthin, wo der Finger ist.
- **Ohne Berührung:** Die Augen schauen zufällig umher und blinzeln alle
  paar Sekunden von selbst.

### 🔢 Digitaluhr – `APP_CLOCK_DIGITAL`

Datei: [src/apps/app_clock_digital.cpp](src/apps/app_clock_digital.cpp)

- Stunden und Minuten groß in 7-Segment-Schrift.
- Der Doppelpunkt blinkt im Sekundentakt.
- Am Rand füllt sich ein **Sekundenring** aus 60 Segmenten. Darunter stehen
  die Sekunden als Zahl.

### 🕰 Analoguhr – `APP_CLOCK_ANALOG`

Datei: [src/apps/app_clock_analog.cpp](src/apps/app_clock_analog.cpp)

- Zifferblatt mit Minuten- und Stundenstrichen und den Zahlen 12, 3, 6 und 9.
- Stunden-, Minuten- und roter Sekundenzeiger. Der Sekundenzeiger läuft
  flüssig.

> **Hinweis zur Uhrzeit:** Das Board hat weder Batterie-Uhr noch Internet.
> Beide Uhren starten deshalb mit der Uhrzeit, zu der das Programm
> **kompiliert** wurde, und zählen dann mit `millis()` weiter. Nach einem
> Neustart geht die Uhr also falsch. Eine echte Uhrzeit über WLAN/NTP wäre
> eine schöne Übung (siehe unten).

---

## 5. Wie der Code funktioniert

### 5.1 Programmablauf (Arduino-Grundgerüst)

Jedes Arduino-Programm besteht aus zwei Funktionen:

```cpp
void setup() { ... }   // läuft genau einmal nach dem Einschalten
void loop()  { ... }   // läuft danach endlos immer wieder
```

In diesem Projekt erledigt [main.cpp](src/main.cpp) das Gemeinsame
(Display starten, Splash zeigen) und gibt dann an die gewählte App weiter:

```
setup()  →  lcd.init()  →  showSplash()  →  appSetup()
loop()   →  appLoop()   →  appLoop()   →  appLoop() …
```

Jede App implementiert also nur `appSetup()` und `appLoop()`.

### 5.2 App-Auswahl mit dem Präprozessor

Alle App-Dateien liegen in `src/apps/` und werden immer kompiliert.
Trotzdem landet nur eine App im Programm, weil jede Datei so aufgebaut ist:

```cpp
#include "app.h"
#if APP == APP_EYES
   ... der ganze Code der App ...
#endif
```

`#if` wird vom **Präprozessor** ausgewertet, also noch vor dem eigentlichen
Kompilieren. Stimmt die Bedingung nicht, sieht der Compiler eine leere Datei.
Deshalb gibt es `appSetup()` und `appLoop()` am Ende genau einmal.
Steht in `APP` ein ungültiger Wert, bricht der Build mit `#error` ab.

### 5.3 Display-Konfiguration (`board.h`)

LovyanGFX wird über eine eigene Klasse `LGFX` eingerichtet. Sie beschreibt:

- **Bus** (`Bus_SPI`): welche Pins, welcher Takt (80 MHz)
- **Panel** (`Panel_GC9A01`): Displaytyp, Größe, Farbinvertierung
- **Hintergrundlicht** (`Light_PWM`): Helligkeit per PWM, `setBrightness(0…255)`
- **Touch** (`Touch_CST816S`): I²C-Pins und Adresse

In `main.cpp` gibt es genau ein Objekt `LGFX lcd;`. Alle Apps greifen darüber
auf das Display zu (`extern LGFX lcd;` in `app.h`).

### 5.4 Koordinaten und Farben

- Der Ursprung **(0, 0)** liegt **oben links**, x wächst nach rechts,
  y nach unten. Die Mitte ist `(CX, CY) = (120, 120)`.
- Das Display ist **rund**: Sichtbar sind nur Pixel mit Abstand ≤ 120 zur
  Mitte. Ecken kann man zeichnen, man sieht sie aber nicht.
- Farben sind **RGB565** (16 Bit: 5 Bit Rot, 6 Bit Grün, 5 Bit Blau):
  ```cpp
  TFT_RED, TFT_CYAN, ...            // vordefinierte Farben
  lcd.color565(255, 128, 0)         // eigene Farbe aus R, G, B (0–255)
  0x1C9F                            // direkt als RGB565-Zahl
  ```
  Achtung: Ein `uint32_t` interpretiert LovyanGFX als RGB888. Farbkonstanten
  deshalb als `uint16_t` speichern (siehe Palette im Malspiel).

### 5.5 Zeichenfunktionen (Auswahl)

```cpp
lcd.fillScreen(color);                      // ganzes Display füllen
lcd.drawPixel(x, y, color);
lcd.drawLine(x1, y1, x2, y2, color);
lcd.drawWideLine(x1, y1, x2, y2, breite, color);   // dicke Linie
lcd.fillRect(x, y, w, h, color);
lcd.drawCircle(x, y, r, color);  lcd.fillCircle(x, y, r, color);
lcd.fillArc(x, y, rAussen, rInnen, winkel0, winkel1, color);  // Ringsegment
```

Bei `fillArc` liegt **0° rechts (3 Uhr)**, die Winkel laufen **im
Uhrzeigersinn**. Für „0° = 12 Uhr“ zieht man 90° ab (siehe Digitaluhr).

### 5.6 Text

```cpp
lcd.setFont(&fonts::FreeSansBold12pt7b);  // Schriftart
lcd.setTextSize(1.3f);                    // Skalierung
lcd.setTextColor(TFT_WHITE);
lcd.setTextDatum(middle_center);          // Bezugspunkt des Textes
lcd.drawString("Hallo", x, y);
```

`setTextDatum` legt fest, welcher Punkt des Textes bei `(x, y)` liegt, z. B.
`middle_center` (zentriert), `middle_right` (rechtsbündig) oder `top_left`.
`fonts::Font7` ist eine 7-Segment-Schrift und enthält nur Ziffern.

### 5.7 Touch abfragen

```cpp
int32_t x, y;
if (lcd.getTouch(&x, &y)) {
  // Finger liegt auf dem Display bei (x, y)
}
```

`getTouch` meldet nur, ob **gerade jetzt** ein Finger auf dem Display liegt.
Für einen **Tipp** (einmal pro Berührung) merkt man sich den vorherigen
Zustand und reagiert nur auf den Wechsel „nicht berührt → berührt“
(Flankenerkennung, siehe `wasTouched` in der Augen-App). Ein Mindestabstand
von 300 ms verhindert doppeltes Auslösen durch Prellen.

**Polarkoordinaten:** Für runde Bedienelemente rechnet man den Touchpunkt in
Abstand und Winkel zur Mitte um:

```cpp
float dx = x - CX, dy = y - CY;
float dist = sqrtf(dx*dx + dy*dy);                 // Abstand zur Mitte
float deg  = atan2f(dy, dx) * 180.0f / M_PI;       // Winkel (-180…180)
```

So erkennt das Malspiel, ob der Finger im Farbring liegt und welches
Segment er trifft.

### 5.8 Flackerfrei zeichnen: drei Strategien

| Strategie | Wo im Projekt | Idee |
|-----------|---------------|------|
| **Direkt zeichnen** | Malspiel | Neue Striche kommen einfach dazu, gelöscht wird nichts. Kein Flackern. |
| **Nur Änderungen zeichnen** | Digitaluhr | Den alten Zustand merken (`lastSec`, `lastMin`) und nur neu zeichnen, was sich geändert hat. Pro Sekunde kommt meist nur ein Ringsegment dazu. |
| **Sprite (Doppelpuffer)** | Augen, Analoguhr | Das Bild entsteht zuerst in einem Speicherbereich (`LGFX_Sprite`) und wird dann mit `pushSprite()` in einem Rutsch übertragen. So sieht man nie ein halb gezeichnetes Bild. |

Ein Sprite braucht RAM: Breite × Höhe × Bytes pro Pixel.

- Augen: ein Sprite mit 98 × 98 × 2 Byte ≈ 19 kB. Er wird für beide Augen
  wiederverwendet.
- Analoguhr: 240 × 240 bei 8 Bit Farbtiefe (`setColorDepth(8)`) = 57,6 kB
  statt 115 kB bei 16 Bit.

```cpp
LGFX_Sprite spr(&lcd);
spr.createSprite(100, 100);     // einmal in setup()
spr.fillScreen(TFT_BLACK);      // im Sprite zeichnen …
spr.fillCircle(50, 50, 40, TFT_WHITE);
spr.pushSprite(x, y);           // … und aufs Display kopieren
```

### 5.9 Animation: Wert schrittweise ans Ziel bringen

Die Augen-App animiert nicht mit festen Bildfolgen. Sie arbeitet mit
**Ist- und Zielwerten**:

```cpp
// linear: feste Schrittweite pro Frame
if (openness < goal) openness = min(goal, openness + speed);
else                 openness = max(goal, openness - speed);

// weich (exponentiell): jedes Frame 25 % des Restwegs
gazeX += (gazeTX - gazeX) * 0.25f;
```

Ein Tipp ändert nur den **Zielwert**. Die Animation läuft dann von selbst.
Dieses Muster eignet sich für fast alles, was sich bewegen soll.

### 5.10 Zeit ohne `delay()`-Blockade

`delay()` hält das Programm komplett an. Für Abläufe wie „alle 3 s blinzeln“
merkt man sich stattdessen einen Zeitpunkt und vergleicht ihn mit `millis()`
(Millisekunden seit dem Start):

```cpp
if (millis() > nextBlink) {
  blinking  = true;
  nextBlink = millis() + random(2500, 6000);
}
```

So bleibt die `loop()` schnell, und Touch und Animation reagieren weiter.
Das kurze `delay(20)` am Ende von `appLoop()` begrenzt nur die Bildrate.

### 5.11 Analoguhr: Winkel → Koordinaten

Ein Zeiger ist eine Linie von der Mitte zu einem Punkt auf einem Kreis:

```cpp
float rad = (deg - 90) * DEG_TO_RAD;   // -90°, damit 0° oben (12 Uhr) liegt
x = CX + cos(rad) * r;
y = CY + sin(rad) * r;
```

Winkel pro Einheit: Sekunde/Minute = 6° (360/60), Stunde = 30° (360/12).
Weil die Zeit als Kommazahl gerechnet wird (`h = 10,5` → 10:30), wandert der
Stundenzeiger stufenlos mit.

---

## 6. Eigene App hinzufügen

1. In `app_config.h` eine neue Nummer anlegen:
   ```cpp
   #define APP_MEINE_APP 5
   ```
2. In `app.h` einen Namen für den Splash ergänzen:
   ```cpp
   #elif APP == APP_MEINE_APP
   #define APP_NAME "Meine App"
   ```
3. Die Datei `src/apps/app_meine_app.cpp` anlegen:
   ```cpp
   #include "app.h"
   #if APP == APP_MEINE_APP

   void appSetup() {
     lcd.fillCircle(CX, CY, 50, TFT_GREEN);
   }

   void appLoop() {
     delay(20);
   }

   #endif
   ```
4. `#define APP APP_MEINE_APP` setzen, dann bauen und flashen.

---

## 7. Übungsideen

Ungefähr nach Schwierigkeit sortiert:

1. **Splash anpassen:** Text, Farben oder Dauer in `main.cpp` ändern.
2. **Malspiel:** Pinselgröße über einen Teil des Rings umschaltbar machen
   oder eine Radiergummi-Farbe (Schwarz) hinzufügen.
3. **Augen:** Irisfarbe bei jedem Tipp zufällig wechseln, oder ein
   „müder Blick“ (Augen nur halb offen, `target = 0.5`).
4. **Digitaluhr:** Farbe je nach Tageszeit (morgens gelb, abends blau) oder
   ein Wechsel zwischen 12- und 24-Stunden-Anzeige per Tipp.
5. **Analoguhr:** Alle zwölf Zahlen zeichnen oder ein Datum im
   Zifferblatt anzeigen.
6. **Uhrzeit per Touch stellen:** Oben tippen = Stunde +1, unten tippen =
   Minute +1 (dafür einen Offset in `clock_util.h` einführen).
7. **Echte Uhrzeit über WLAN:** Mit `WiFi.begin()` und `configTime()`
   (NTP) die Uhrzeit holen und `clockMillis()` darauf umstellen.
8. **App-Wechsel ohne Neu-Flashen:** Mit der BOOT-Taste zur Laufzeit
   zwischen den Apps umschalten (dafür alle Apps mitkompilieren und über
   Funktionszeiger oder eine Tabelle aufrufen).
9. **Eigene Spiele:** z. B. Pong am Kreisrand, ein Reaktionsspiel
   („tippe den Punkt“) oder eine Wasserwaage-Animation.
