#pragma once
// Gemeinsame Schnittstelle aller Beispiel-Apps.
// Jede App liegt in src/apps/ und implementiert appSetup() und appLoop().
// Nur die per APP ausgewählte App wird mitkompiliert.

#include <Arduino.h>
#include "board.h"
#include "app_config.h"

#if APP == APP_PAINT
#define APP_NAME "Malspiel"
#elif APP == APP_EYES
#define APP_NAME "Augen"
#elif APP == APP_CLOCK_DIGITAL
#define APP_NAME "Digitaluhr"
#elif APP == APP_CLOCK_ANALOG
#define APP_NAME "Analoguhr"
#else
#error "Unbekannte APP – siehe include/app_config.h"
#endif

// Display-Mittelpunkt
static const int CX = SCREEN_W / 2;
static const int CY = SCREEN_H / 2;

extern LGFX lcd;  // definiert in main.cpp

void appSetup();
void appLoop();
