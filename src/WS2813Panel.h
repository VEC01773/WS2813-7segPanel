#ifndef _WS2813PANEL_H
#define _WS2813PANEL_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <cstdint>

#define LIBVERSION "1.0.0" // To be deprecated


class WS2813Panel {
public:

	WS2813Panel(int num);

	void AllPixelShow(int color);
	void AllPixelShow(uint8_t r, uint8_t g, uint8_t b);
	void Blink();
	

private:
	Adafruit_NeoPixel *pixels;
};
#endif
