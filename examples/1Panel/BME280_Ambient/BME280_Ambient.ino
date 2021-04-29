
/***************************************************************************
  BME280 で気温、湿度と気圧を測定して　Ambientにデータを送信する。
 ***************************************************************************/
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Ambient.h>
#include "WS2813Panel.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define PANEL_NUM 1

uint32_t COLOR[] = {0xffffff, 0x0000ff, 0x00ff00, 0xff0000, 0xff00ff}; //10桁目の温度を色で表示 白、青、緑、赤、紫

Adafruit_BME280 bme; // I2C
WiFiClient client;
Ambient ambient;

WS2813Panel MyPanel(PANEL_NUM);

unsigned int channelId = 99999;            // AmbientのチャネルID
const char *writeKey = "1234567890123456"; // ライトキー
int Interval = 300;                        //測定間隔(sec)

//-------------------------------
void setup()
{

  Serial.begin(115200);

  MyPanel.Begin();
  MyPanel.Clear();
  MyPanel.Show();
  delay(100);
  //Automatic brightness adjustment ON
  MyPanel.AutoBright = true;

  Serial.println(F("Start BME280 Ambient"));

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

  Wire.begin(4, 5);
  if (!bme.begin(0x76))
  {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1)
      delay(1);
  }

  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
}

//-------------------------------
void loop()
{
  float temp = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0;
  float humidity = bme.readHumidity();

  Serial.printf("temp = %f : humid = %f : press = %f\n", temp, humidity, pressure);
  DispTemp(temp);
  delay(500);
  DispHumid(humidity);
  delay(500);
  DispPress(pressure);
  delay(500);

  ambient.set(1, temp);     // 温度をデータ1にセット
  ambient.set(2, humidity); // 湿度をデータ3にセット
  ambient.set(3, pressure); // 気圧をデータ3にセット

  ambient.send(); // データをAmbientに送信
  MyPanel.Clear();
  MyPanel.Show();
  delay(Interval * 1000);
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
