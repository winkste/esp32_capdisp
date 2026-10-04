#pragma once
// ============================================================
//  Auswahl des Beispiels
//  Einfach die gewünschte App bei "#define APP" eintragen,
//  dann neu bauen und flashen.
// ============================================================

#define APP_PAINT         1  // Malspiel: Farbring + mit dem Finger malen
#define APP_EYES          2  // Animierte Augen: Tippen öffnet/schließt sie
#define APP_CLOCK_DIGITAL 3  // Digitale Uhr mit Sekundenring
#define APP_CLOCK_ANALOG  4  // Analoge Uhr mit Zeigern

#ifndef APP  // kann auch per build_flags (-D APP=...) gesetzt werden
#define APP APP_EYES
#endif
