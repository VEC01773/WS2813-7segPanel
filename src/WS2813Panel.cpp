// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "WS2813Panel.h"

#define LED_COUNT 24
#define PIN 14


WS2813Panel::WS2813Panel()
{
    pixels = new Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
    pixels->begin();
    pixels->show();
}

void WS2813Panel::AllPixelShow(uint8_t r, uint8_t g, uint8_t b)
{
    AllPixelShow(pixels->Color(r, g, b));
}

void WS2813Panel::AllPixelShow(int color)
{
    for (int j = 0; j < 24; j++)
    {
        pixels->setPixelColor(j, color);
    }
    pixels->show();
}

void WS2813Panel::Blink()
{
    for (int j = 0; j < 24; j++)
    {
        pixels->setPixelColor(j, pixels->Color(255, 255, 255));
    }
    pixels->show();
}
