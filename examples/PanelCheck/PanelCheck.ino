

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define PANEL_NUM 12

Adafruit_BME280 bme; // I2C

WS2813Panel MyPanel(PANEL_NUM);

uint8_t bright = 50;
uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};

//------------------------------------
void setup()
{
  pinMode(0, INPUT);

  Serial.begin(115200);
  Serial.println("Program Start");

  Serial.println("LED Initialze");
  MyPanel.Begin();
  MyPanel.Show();

  Serial.println("BME280 Initialize");
  //I2c初期化　SDA=4、SCK=5
  Wire.begin(4, 5);
  unsigned status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    delay(10);
  }
}

//------------------------------------
void loop()
{
  //MyPanel.SetBrightness(30);
  //照度自動調整ON
  MyPanel.AutoBright = true;

  //色
  for (int col = 1; col < 8; col++)
  {
    int color = (0xff0000 * (col & 1)) + (0x00ff00 * ((col >> 1) & 1)) + (0x0000ff * ((col >> 2) & 1));
    for (int num = 0; num <= 9; num++)
    {
      for (int i = 0; i < PANEL_NUM; i++)
      {
        MyPanel.DispNum(i, num, color); //0xff0000 * (col & 1)  + );
      }
      MyPanel.Show();
      delay(200);
    } //

    MyPanel.Clear();
    for (int i = 0; i < PANEL_NUM; i++)
    {
      MyPanel.DispDot(i, color);
      MyPanel.DispColon(i, color);
    }
    MyPanel.Show();
    delay(200);
    MyPanel.Clear();
    MyPanel.Show();
    delay(200);
  }
}
