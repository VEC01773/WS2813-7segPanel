
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include <time.h>

#include "WS2813Panel.h"
#include <Espalexa.h>

#define PIN_BUTTON 0 //ボタン接続ピン

const uint8_t BRITE_MIN = 0x10; //LEDの明るさ最低
const uint8_t BRITE_MAX = 0xFF; //LEDの明るさ最高　最大値はFF(255)　電流を見て決める

void Dice(uint8_t dmy);

Espalexa espalexa;
WS2813Panel MyPanel(1);
uint32_t color = 0x00ff00;
uint8_t bright = 0x20; //明るさ
uint16_t swstat = 0;    //ボタンの状態記録用　チャタリング防止

//-------------------------------
void setup()
{
    pinMode(PIN_BUTTON, INPUT);

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

    Serial.println("LED Initialze");
    MyPanel.Begin();
    MyPanel.SetBrightness(bright);
    delay(10);
    MyPanel.Clear();
    MyPanel.DispNum(0, 0, color);
    MyPanel.Show();
 
    espalexa.addDevice("DICE", Dice);
    espalexa.begin();
}

//-------------------------------
void loop()
{
    //スイッチ監視
    if (digitalRead(PIN_BUTTON) == LOW)
    {
        swstat = (swstat << 1) | 0x1;
        //Serial.println(swstat);
        if (swstat == 0x0FFF)
        {
            bright += 0x20;
            if (bright < BRITE_MIN)
                bright = BRITE_MIN;
            swstat = 0;
            MyPanel.SetBrightness(bright);
            MyPanel.Show();
        }
        delay(5);
    }
    else
    {
        swstat = 0;
    }
    espalexa.loop();
    delay(1);
}

//-------------------------------
void Dice(uint8_t dmy)
{
    randomSeed(millis());
    int num;
    int temp;
    for (int i = 0; i < 30; i++)
    {
        temp = random(1, 7);
        if (temp == num)
            continue;
        num = temp;
        MyPanel.DispNum(0, num, color);
        MyPanel.Show();
        delay(i * 20);
    }
    for (int j = 0; j < 3; j++)
    {
        MyPanel.DispNum(0, num, 0);
        MyPanel.Show();
        delay(200);
        MyPanel.DispNum(0, num, color);
        MyPanel.Show();
        delay(200);
    }
}
