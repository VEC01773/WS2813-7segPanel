

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define PANEL_NUM 12

Adafruit_BME280 bme; // I2C

WS2813Panel MyPanel(PANEL_NUM);

uint8_t bright = 50;
unsigned long delayTime;
uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};
int MODE = 0;

int ChangeTime = 3000; //表示切替時間(msec)
uint NextChange;

//------------------------------------
void setup()
{
  pinMode(0, INPUT);

  Serial.begin(115200);
  Serial.println("Start");
  MyPanel.Begin();
  MyPanel.Show();
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
  PanelCheck();
  
  delayTime = 1000;
  NextChange = millis() + ChangeTime;

}

//------------------------------------
void loop()
{
  if (millis() > NextChange)
  {
    MODE = (MODE + 1) % 3;
    NextChange = millis() + ChangeTime;
  }
  printValues(MODE);
  // int light = system_adc_read(); // 1024.0 ;
  uint8_t light = MyPanel.GetBright();
  Serial.printf("Bright = %d\n", light);
  MyPanel.SetBrightness(light * 2 + 20);

  delay(10);
}
//--------------------------------
void PanelCheck()
{
  MyPanel.SetBrightness(30);
  for (int col = 1; col < 8; col++)
  {
    for (int num = 0; num <= 9; num++)
    {
      for (int i = 0; i < PANEL_NUM; i++)
      {
        int color = (0xff0000 * (col & 1)) + (0x00ff00 * ((col>>1) & 1)) + (0x0000ff * ((col>>2) & 1));  
        MyPanel.DispNum(i, num,  color); //0xff0000 * (col & 1)  + );
      }
      MyPanel.Show();
        delay(100);
    }
  }
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
  uint8_t dec_temp[5]; // [4]符号 [3][2].[1] [0]℃
  dec_temp[0] = 20;    //℃
  //温度を10倍
  int tempx10 = int(abs(t * 10.0));
  //マイナス
  if (t < 0)
    dec_temp[4] = 0x11;
  else
    dec_temp[4] = 0x10;

  for (int i = 1; i < 4; i++)
  {
    dec_temp[i] = tempx10 % 10;
    tempx10 = tempx10 / 10;
  }
  if (abs(t) < 10)
    dec_temp[3] = 0x10;

  for (int pnl = 0; pnl < 5; pnl++)
  {
    MyPanel.DispNum(pnl, dec_temp[pnl], COLOR[0]);
  }
  //パネル2枚目に小数点
  MyPanel.DispDot(2, COLOR[0]);
  MyPanel.Show();
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
      MyPanel.SetPanelColor(pnl, 0);
    else
      MyPanel.DispNum(pnl, dec_temp[pnl], COLOR[1]);
  }
  //パネル2枚目に小数点
  MyPanel.DispDot(2, COLOR[1]);
  MyPanel.Show();
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
      MyPanel.SetPanelColor(pnl, 0);
    else
      MyPanel.DispNum(pnl, dec_temp[pnl], COLOR[2]);
  }
  MyPanel.Show();
}
