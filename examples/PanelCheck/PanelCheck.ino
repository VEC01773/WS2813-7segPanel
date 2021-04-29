/****************************************
 * Panel operation check 
 * LED chek
 * BME280 check
 * Phototransister chek 
 * 
***************************************/

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define PANEL_NUM 12

Adafruit_BME280 bme; // I2C

WS2813Panel MyPanel(PANEL_NUM);

uint32_t COLOR[] = {0xffffff, 0x0000ff, 0x00ff00, 0xff0000, 0xff00ff}; //10桁目の温度を色で表示 白、青、緑、赤、紫

//------------------------------------
void setup()
{
  pinMode(0, INPUT);

  Serial.begin(115200);
  Serial.println("Program Start");

  Serial.println("LED Initialze");
  MyPanel.Begin();
  MyPanel.Clear();
  MyPanel.Show();
  delay(100);
  //Automatic brightness adjustment ON
  MyPanel.AutoBright = true;

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
  //Color
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
    }

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
  //
  GetBME280();
}

void GetBME280()
{
  float temp = bme.readTemperature();
  float humi = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

  Serial.printf("temp = %f : humid = %f : press = %f\n", temp, humi, pressure);

  DispTemp(temp);
  delay(500);
  DispHumid(humi);
  delay(500);
  DispPress(pressure);
  delay(500);
}
//---------------------------
// 温度表示1桁
//---------------------------
void DispTemp(float temp)
{
  int t = round(temp);
  int num = t % 10;
  int colr = t / 10;
  MyPanel.Clear();
  MyPanel.DispLEDIndex(0, 22, COLOR[0]);
  MyPanel.Show();

  if (t < -9) //「L」表示
  {
    MyPanel.DispLEDIndex(0, 22, COLOR[0]);
    MyPanel.DispNum(0, 23, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else if (t < 0) //白点滅
  {
    num = -t % 10;
    colr = 0;
    MyPanel.DispLEDIndex(0, 22, COLOR[0]);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(200);
    MyPanel.Clear();
    MyPanel.DispLEDIndex(0, 22, COLOR[0]);
    MyPanel.Show();
    delay(200);
    MyPanel.DispLEDIndex(0, 22, COLOR[0]);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
  }
  else if (t > 49) //「H」表示
  {
    MyPanel.DispLEDIndex(0, 22, COLOR[0]);
    MyPanel.DispNum(0, 24, COLOR[4]); //H
    MyPanel.Show();                   // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else
  {
    MyPanel.DispLEDIndex(0, 22, COLOR[0]);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
}

//---------------------------
// 湿度表示1桁
//---------------------
void DispHumid(float humi)
{
  int h = round(humi);
  int num = h % 10;
  int colr = (h - 30.0) / 10;
  MyPanel.Clear();
  MyPanel.DispLEDIndex(0, 23, COLOR[0]);
  MyPanel.Show();

  if (h < 20) //「L」表示
  {
    MyPanel.DispLEDIndex(0, 23, COLOR[0]);
    MyPanel.DispNum(0, 23, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else if (h < 30) //白点滅
  {
    colr = 0;
    MyPanel.DispLEDIndex(0, 23, COLOR[0]);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(200);
    MyPanel.Clear();
    MyPanel.DispLEDIndex(0, 23, COLOR[0]);
    MyPanel.Show();
    delay(200);
    MyPanel.DispLEDIndex(0, 23, COLOR[0]);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
  }
  else if (h > 79) //「H」表示
  {
    MyPanel.DispLEDIndex(0, 23, COLOR[0]);
    MyPanel.DispNum(0, 24, COLOR[4]); //H
    MyPanel.Show();                   // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else
  {
    MyPanel.DispLEDIndex(0, 23, COLOR[0]);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
}

//---------------------------
// 気圧表示1桁
//---------------------
void DispPress(float pressure)
{
  int p = round(pressure);
  int num = p % 10;
  int colr = (p - 980.0) / 10;
  MyPanel.Clear();
  MyPanel.DispLEDIndex(0, 21, COLOR[0]);
  MyPanel.Show();

  if (p < 960) //「L」表示
  {
    MyPanel.DispNum(0, 23, COLOR[0]);
    MyPanel.DispLEDIndex(0, 21, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else if (p < 970) //白点滅
  {
    colr = 0;
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.DispLEDIndex(0, 21, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(200);
    MyPanel.Clear();
    MyPanel.DispLEDIndex(0, 21, COLOR[0]);
    MyPanel.Show();
    delay(200);
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.DispLEDIndex(0, 21, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
  }
  else if (p > 1019)
  {
    MyPanel.DispNum(0, 24, COLOR[4]); //H
    MyPanel.DispLEDIndex(0, 21, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else
  {
    MyPanel.DispNum(0, num, COLOR[colr]);
    MyPanel.DispLEDIndex(0, 21, COLOR[0]);
    MyPanel.Show(); // Send the updated pixel colors to the hardware.
    delay(400);
  }
}
