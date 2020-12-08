
#include "WS2813Panel.h"

WS2813Panel panel1(1);

void setup()
{
  // put your setup code here, to run once:

  // panel1.Blink();
  panel1.AllPixelShow(0, 100, 0);
  Serial.begin(115200);
  Serial.println("Start");
}

void loop()
{
  Serial.println("Red Start");
  for (int i = 0; i < 255; i += 8)
  {
    panel1.AllPixelShow(i, 0, 0);
    delay(10);
  }
  Serial.println("Red END");
 
  for (uint16_t g = 0; g < 255; g += 8)
  {
    panel1.AllPixelShow(0, g, 0);
    delay(10);
  }

  for (uint16_t b = 0; b < 255; b += 8)
  {
    panel1.AllPixelShow(0, 0, b);
    delay(10);
  }

  for (uint16_t w = 0; w < 255; w += 8)
  {
    panel1.AllPixelShow(w, w, w);
    delay(10);
  }
}
