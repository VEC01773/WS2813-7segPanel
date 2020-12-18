

#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include <time.h>

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define PANEL_NUM 12
#define JST 3600 * 9

Adafruit_BME280 bme; // I2C

WS2813Panel MyPanel(PANEL_NUM);

uint8_t bright = 50;
uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};
uint8_t ColorIndex = 1;
uint8_t swstat = 0;
//------------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("Program Start");

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("connected");

  //NTP
  configTzTime("JST-9", "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); // 2.7.0以降, esp32コンパチ

  pinMode(0, INPUT);

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
  time_t t;
  struct tm *tm;
  t = time(NULL);
  tm = localtime(&t);

  int color = (0xff0000 * (ColorIndex & 1)) + (0x00ff00 * ((ColorIndex >> 1) & 1)) + (0x0000ff * ((ColorIndex >> 2) & 1));
 
  //MyPanel.SetBrightness(30);
  //照度自動調整ON
  MyPanel.AutoBright = true;
  // MyPanel.Clear();
  DispDate(tm, color);
  //  delay(1000);
  // MyPanel.Clear();
  DispClock(tm, color);
  //  delay(1000);

  if (digitalRead(0) == LOW)
  {
    swstat = (swstat << 1) | 0x1;
    Serial.println(swstat);
    if (swstat == 0x0F)
    {
      ColorIndex++;
      if (ColorIndex > 7)
        ColorIndex = 1;
      swstat = 0;  
    }
  }
  else
  {
    swstat = 0;
  }

  delay(10);
}

//------------------------------------
void DispDate(struct tm *tm, uint32_t color)
{
  int m = tm->tm_mon + 1;
  int8_t mnum[2];
  mnum[0] = m % 10;
  m = m / 10;
  mnum[1] = m % 10;

  for (int i = 0; i < 2; i++)
  {
    if (i == 1 && mnum[i] == 0)
      MyPanel.DispNum(7 - i, 0x10, color);
    else
      MyPanel.DispNum(7 - i, mnum[i], color);
  }
  MyPanel.DispNum(8, 0x11, color); //-

  int d = tm->tm_mday;
  int8_t dnum[2];
  dnum[0] = d % 10;
  d = d / 10;
  dnum[1] = d % 10;

  for (int i = 0; i < 2; i++)
  {
    if (i == 1 && dnum[i] == 0)
      MyPanel.DispNum(10 - i, 0x10, color);
    else
      MyPanel.DispNum(10 - i, dnum[i], color);
  }

  MyPanel.Show();
}
//------------------------------------
void DispClock(struct tm *tm, uint32_t color)
{
  static int presec;
  static int startsec;

  if (presec != tm->tm_sec)
  {
    startsec = millis();
  }
  presec = tm->tm_sec;

  int h = tm->tm_hour;
  int8_t hnum[2];
  hnum[0] = h % 10;
  h = h / 10;
  hnum[1] = h % 10;
  for (int i = 0; i < 2; i++)
  {
    if (i == 1 && hnum[i] == 0)
      MyPanel.DispNum(i + 4, 0x10, color);
    else
      MyPanel.DispNum(i + 4, hnum[i], color);
  }

  int m = tm->tm_min;
  int8_t mnum[2];
  mnum[0] = m % 10;
  m = m / 10;
  mnum[1] = m % 10;

  for (int i = 0; i < 2; i++)
  {
    MyPanel.DispNum(i + 2, mnum[i], color);
  }

  int s = tm->tm_sec;
  int8_t snum[2];
  snum[0] = s % 10;
  s = s / 10;
  snum[1] = s % 10;

  for (int i = 0; i < 2; i++)
  {
    MyPanel.DispNum(i, snum[i], color);
  }
  //時計のコロンを0.5sec毎に点滅
  if (((float)millis() - (float)startsec) < 500)
  {
    MyPanel.DispColon(2, color);
    MyPanel.DispColon(4, color);
  }
  else
  {
    MyPanel.DispColon(2, 0);
    MyPanel.DispColon(4, 0);
  }
  MyPanel.Show();
}
