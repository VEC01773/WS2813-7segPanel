#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <Ambient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

extern "C" {
#include <user_interface.h>
}
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
WiFiClient client;
Ambient ambient;
unsigned int channelId = 99999;            // AmbientのチャネルID
const char *writeKey = "123456789abcde"; // ライトキー
int Interval = 300; //測定間隔(sec)



// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Start");

  // Start up the library
  sensors.begin();

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

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  float tempC = sensors.getTempCByIndex(0);
  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
  }
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
  ambient.set(1, tempC);     // 温度をデータ1にセット
 
  ambient.send(); // データをAmbientに送信
  ESP.deepSleep(Interval * 1000 * 1000);
}

void loop(void)
{

}