

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//ADC
extern "C"
{
#include "user_interface.h"
}

#define PANEL_NUM 1

WS2813Panel MyPanel(PANEL_NUM);

uint8_t bright = 0x20;
uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};
uint8_t ColorIndex = 1;
uint8_t swstat = 0;

//------------------------------------
void setup()
{
    Serial.begin(115200);
    Serial.println("Program Start");

    Serial.println("LED Initialze");
    MyPanel.Begin();
    delay(10);
    MyPanel.Clear();
    MyPanel.Show();

    pinMode(0, INPUT);
}

//------------------------------------
void loop()
{
    //moistレベルを0-9に　
    uint16_t sum = 0;
    double ave = 0;
    for (int i = 0; i < 8; i++)
    {
        sum += system_adc_read();
    }
    ave = ((double)sum / 8.0);
    int moist = (950.0 - ((ave - 350.0) * 1.66)) / 100.0;

    int color = 0xffffff; //white

    byte R;
    byte G;
    byte B;
    if (moist > 9 || moist < 0) //取得失敗
    {
        R = 128.0;
        B = 0;
        G = 128.0;
    }
    else if (moist >= 5)
    {
        R = (255.0 * ((float)moist - 5.0) / 4.0);
        G = (255.0 * (9.0 - (float)moist) / 4.0);
        B = 0;
    }
    else
    {
        G = (255.0 * (float)moist / 4.0);
        B = (255.0 * (4.0 - (float)moist) / 4.0);
        R = 0;
    }
    //Serial.printf("moist = %d B = %d G = %d R = %d\n", moist, B, G, R);

    color = B | (G << 8) | (R << 16);

    Serial.println(ave);
    Serial.println(moist);
    MyPanel.SetBrightness(bright);
    MyPanel.DispNum(0, (uint8_t)moist, color);
    MyPanel.Show();

    //スイッチ監視(チャタリング防止)
    if (digitalRead(0) == LOW)
    {
        swstat = (swstat << 1) | 0x1;
        Serial.println(swstat);
        if (swstat == 0x0F)
        {
            bright += 0x08;
            if (bright < 0x10 )
                bright = 0x10;
            swstat = 0;
        }
    }
    else
    {
        swstat = 0;
    }

    delay(10);
}
