

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

WS2813Panel Panelx5(5);

uint8_t bright = 50;
unsigned long delayTime;
int COLOR = 0;
int MODE = 0;

//------------------------------------
void setup()
{
  pinMode(0, INPUT);

  Serial.begin(115200);
  Serial.println("Start");
  Panelx5.Begin();
  Panelx5.Show();
  Serial.println("End");

  Wire.begin(4, 5);
  // default settings
  unsigned status = bme.begin(0x76);
  // You can also pass in a Wire library object like &Wire2
  //status = bme.begin(0x76, &Wire2)
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1)
      delay(10);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;
}

//------------------------------------
void loop()
{
  Panelx5.SetBrightness(bright);
  // bright -= 8;

  printValues(MODE);
  MODE = (MODE + 1) % 3;
  delay(3000);
  return;
  /*
  Serial.println("Red Start");
  Panelx5.SetPixcelColor(0, 100, 0, 0);
  Panelx5.Show();
  delay(500);
  Serial.println("Red End");

  Serial.println("Blue Start");
  Panelx5.SetPanelColor(0, 0, 100, 0);
  Panelx5.Show();
  delay(500);
  Serial.println("Blue End");

  Serial.println("Green Start");
  Panelx5.SetPanelColor(0, 0, 0, 100);
  Panelx5.Show();
  delay(500);
  Serial.println("Green end");

  Panelx5.Clear();
  int cnt = 99999;
  uint8_t num = 0;
  int temp = cnt;
  while (cnt > 0)
  {
    temp = cnt;
    for (int pnl = 0; pnl < 5; pnl++)
    {
      num = temp % 10;
      Panelx5.DispNum(pnl, num, 100);
      temp = temp / 10;
    }
    Panelx5.Show();
    delay(1);
    cnt--;
  }
  */
}
//--------------------------------
void printValues(int mode)
{
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure();

  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" *C");

  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println(" %");

  Serial.print("Pressure = ");
  Serial.print(p / 100.0F);
  Serial.println(" hPa");
  /*
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
*/
  Serial.println();

  switch (mode)
  {
  case 0:
    DispTemp(t);
    break;
  case 1:
    DispHumid(h);
    break;
  case 2:
    DispPress(p / 100.0F);
    break;

  default:
    break;
  }
}
//--------------------------------
void DispTemp(float t)
{
  uint8_t dec_temp[5];  // [4]符号 [3][2].[1] [0]℃
  dec_temp[0] = 20; //℃
  //温度を10倍
  int tempx10 = int(abs(t * 10.0));
  //マイナス
  if(t < 0)
    dec_temp[4] = 0x11;
  else
    dec_temp[4] = 0x10;
  
  for (int i = 1; i < 4; i++)
  {
    dec_temp[i] = tempx10 % 10;
    tempx10 = tempx10 / 10;
  }
  if(abs(t) < 10)
    dec_temp[3] = 0x10;

  for (int pnl = 0; pnl < 5; pnl++)
  {
      Panelx5.DispNum(pnl, dec_temp[pnl], 100);
  }
  //パネル2枚目に小数点
  Panelx5.DispDot(2, 100);
  Panelx5.Show();
}

//------------------------------------
void DispHumid(float h)
{
  uint8_t dec_temp[5];
  dec_temp[0] = 21; //%
  //湿度を10倍
  int tempx10 = int(h * 10.0);

  for (int i = 1; i < 5; i++)
  {
    dec_temp[i] = tempx10 % 10;
    tempx10 = tempx10 / 10;
  }

  for (int pnl = 0; pnl < 5; pnl++)
  {
    if (pnl == 4 && dec_temp[pnl] == 0)
      Panelx5.SetPanelColor(pnl, 0);
    else
      Panelx5.DispNum(pnl, dec_temp[pnl], 100);
  }
  //パネル2枚目に小数点
  Panelx5.DispDot(2, 100);
  Panelx5.Show();
}
//------------------------------------
void DispPress(float p)
{
  uint8_t dec_temp[5];
  dec_temp[0] = 22; //h
  //湿度を10倍
  int tempx10 = p;

  for (int i = 1; i < 5; i++)
  {
    dec_temp[i] = tempx10 % 10;
    tempx10 = tempx10 / 10;
  }

  for (int pnl = 0; pnl < 5; pnl++)
  {
    if (pnl == 4 && dec_temp[pnl] == 0)
      Panelx5.SetPanelColor(pnl, 0);
    else
      Panelx5.DispNum(pnl, dec_temp[pnl], 100);
  }
  Panelx5.Show();
}
