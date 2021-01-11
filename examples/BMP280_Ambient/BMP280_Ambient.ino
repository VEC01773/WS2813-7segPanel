
/***************************************************************************
  BMP280 で気温と気圧を測定して　Ambientにデータを送信する。
 ***************************************************************************/
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Ambient.h>

Adafruit_BMP280 bmp; // I2C
WiFiClient client;
Ambient ambient;
unsigned int channelId = 99999;            // AmbientのチャネルID
const char *writeKey = "************"; // ライトキー
int Interval = 300;                        //測定間隔(sec)

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Start BMP280 Ambient"));

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
  if (!bmp.begin(0x76))
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1)
      delay(1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
}

void loop()
{
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;

  Serial.print(F("Temperature = "));
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");

  Serial.println();

  ambient.set(1, temp);     // 温度をデータ1にセット
  ambient.set(2, pressure); // 気圧をデータ3にセット

  ambient.send(); // データをAmbientに送信
  delay(Interval * 1000);
}
