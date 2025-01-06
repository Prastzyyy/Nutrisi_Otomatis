//web.h

#ifndef WEB_H
#define WEB_H
#include <Arduino.h>

extern String html;

String SendHTML(float pH, float PPM, float suhu, float tinggi_cm, String kondisiOtomatisasi);

#endif
