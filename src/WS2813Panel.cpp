// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "WS2813Panel.h"

//ADC
extern "C" {
#include "user_interface.h"
}


#define LED_COUNT 24
#define PIN 14


int nums[] =
{
  0b1111111111111111110000,//0
  0b0001111110000000000000,//1
  0b1111110001111110001110,//2
  0b1111111111110000001110,//3
  0b0001111110000001111110,//4
  0b1110001111110001111110,//5
  0b1110001111111111111110,//6
  0b1111111110000000000000,//7
  0b1111111111111111111110,//8
  0b1111111111110001111110,//9
  0b1111111110001111111110,//A
  0b0000001111111111111110,//b
  0b0000000001111110001110,//c
  0b0001111111111110001110,//d
  0b1110000001111111111110,//E
  0b1110000000001111111110,//F
  0b0000000000000000000000,//消灯   16
  0b0000000000000000001110,//-	    17
  0b0000000000000000000000,//消灯   18
  0b0000000000000000000000,//消灯	19
  0b0000000001111110101110,// ℃    20
  0b0000100100000100100100,// %     21
  0b0000001110001111111110,// h     22
  0b0000000001111111110000,// L     23
  0b0001111110001111111110,// H     24
  0b1111110000001111111110 // P     25
};

//-----------------
WS2813Panel::WS2813Panel(uint8_t num)
{
    panel_num = num;
    pixels = new Adafruit_NeoPixel(LED_COUNT * num, PIN, NEO_GRB + NEO_KHZ800);
    AutoBright = false;
}
//-----------------
void  WS2813Panel::Begin()
{
    pixels->begin();
}
//-----------------
void  WS2813Panel::Show()
{
    if(AutoBright)
    {
        uint8_t brt = GetBright();
        pixels->setBrightness(10 + brt);
    }
    pixels->show();
}
//-----------------
void WS2813Panel::Clear()
{
    pixels->clear();
}
//-----------------
void WS2813Panel::SetBrightness(uint8_t b)
{
    pixels->setBrightness(b);
}

//-----------------
void WS2813Panel::SetPixcelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    pixels->setPixelColor(n, r, g, b);
}
//-----------------
void WS2813Panel::SetPixcelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    pixels->setPixelColor(n, r, g, b, w);
}
//-----------------
void WS2813Panel::SetPixcelColor(uint16_t n, uint32_t color)
{
    pixels->setPixelColor(n, color);
}

//-----------------
void WS2813Panel::SetPanelColor(uint16_t pnl_no, uint8_t r, uint8_t g, uint8_t b)
{
    int first = pnl_no * LED_COUNT;
    Fill(pixels->Color(r,g,b), first, LED_COUNT);

}
//-----------------
void WS2813Panel::SetPanelColor(uint16_t pnl_no, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    int first = pnl_no * LED_COUNT;
    Fill(pixels->Color(r,g,b,w), first, LED_COUNT);
}
//-----------------
void WS2813Panel::SetPanelColor(uint16_t pnl_no, uint32_t color)
{
    int first = pnl_no * LED_COUNT;
    Fill(color, first, LED_COUNT);
}

//-----------------
void WS2813Panel::SetColor(uint8_t r, uint8_t g, uint8_t b)
{
    int first = 0;
    Fill(pixels->Color(r,g,b), first, LED_COUNT * panel_num);
}
//-----------------
void WS2813Panel::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    int first = 0;
    Fill(pixels->Color(r,g,b, w), first, LED_COUNT * panel_num);
}
//-----------------
void WS2813Panel::SetColor(uint32_t color)
{
    int first = 0;
    Fill(color, first, LED_COUNT * panel_num);
}

//-----------------
void WS2813Panel::DispNum(uint8_t pnl_no, uint8_t num, uint32_t color)
{
  int first = pnl_no * LED_COUNT;
  for (int j = 0; j < 22; j++)
  {
    if (num < 0)
      pixels->setPixelColor(first + j, 0);
    else
      pixels->setPixelColor(first + j, color * ((nums[num] >> (21 - j)) & 0x01));
  }
//   pixels->show();
}
//-----------------
void WS2813Panel::DispDot(uint8_t pnl_no, uint32_t color)
{
  int first = pnl_no * LED_COUNT;
  pixels->setPixelColor(first + 21, color);
  pixels->show();
}

//-----------------
void WS2813Panel::DispColon(uint8_t pnl_no, uint32_t color)
{
  int first = pnl_no * LED_COUNT;
  pixels->setPixelColor(first + 22, color);
  pixels->setPixelColor(first + 23, color);
//   pixels->show();
}

//-----------------
void WS2813Panel::DispLEDIndex(uint8_t pnl_no, uint8_t index, uint32_t color)
{
  int first = pnl_no * LED_COUNT;
  pixels->setPixelColor(first + index, color);
//   pixels->show();
}

//-----------------
void WS2813Panel::Fill(uint32_t color, uint16_t first, uint16_t count)
{
    pixels->fill(color, first, count);
}
//-------------------------------------------------
//外の明るさを取得　8回の平均値 0-100
//-------------------------------------------------
uint8_t WS2813Panel::GetBright()
{
	uint16_t sum = 0;
    double ave = 0;
    for (int i = 0; i < 8; i++)
		sum  += system_adc_read();
    ave = ((double)sum /8.0);

	return (uint8_t)(ave /1024.0 * 100.0);
}
