#ifndef _WS2813PANEL_H
#define _WS2813PANEL_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <cstdint>

#define LIBVERSION "1.0.0" // To be deprecated


class WS2813Panel {
public:

	WS2813Panel(uint8_t num);

	bool AutoBright;

	void Begin();
	void Show();
	void Clear();
	void SetBrightness(uint8_t b);

	void SetPixcelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
	void SetPixcelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	void SetPixcelColor(uint16_t n, uint32_t color);

	void SetPanelColor(uint16_t pnl_no, uint8_t r, uint8_t g, uint8_t b);
	void SetPanelColor(uint16_t pnl_no, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	void SetPanelColor(uint16_t pnl_no, uint32_t color);

	void SetColor(uint8_t r, uint8_t g, uint8_t b);
	void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	void SetColor(uint32_t color);

	void DispNum(uint8_t pnl_no, uint8_t num, uint32_t color);
	void DispDot(uint8_t pnl_no, uint32_t color);
	void DispColon(uint8_t pnl_no, uint32_t color);
	void DispLEDIndex(uint8_t pnl_no, uint8_t index, uint32_t color);

	void Fill(uint32_t color, uint16_t first, uint16_t count);

	uint8_t GetBright();//外部の明るさ
	

private:
	Adafruit_NeoPixel *pixels;
	uint8_t panel_num;


};
#endif
