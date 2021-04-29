/*******************************************************
　電子サイコロ
********************************************************/

#include "WS2813Panel.h"

#define PIN_BUTTON 0 //ボタン接続ピン

const uint8_t BRITE_MIN = 0x10; //LEDの明るさ最低
const uint8_t BRITE_MAX = 0xFF; //LEDの明るさ最高　最大値はFF(255)　電流を見て決める

WS2813Panel MyPanel(1);

uint32_t color[] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xff00ff, 0x00ffff, 0xffffff};
uint8_t bright = 0x20; //明るさ
uint16_t swstat = 0;   //ボタンの状態記録用　チャタリング防止

int PreBright = 0xff;
int CurBright = 0;

//-------------------------------
void setup()
{
    pinMode(PIN_BUTTON, INPUT);

    Serial.begin(115200);
    Serial.println("Program Start");

    Serial.println("LED Initialze");
    MyPanel.Begin();
    //   MyPanel.SetBrightness(bright);
    //明るさ自動調整ON
    MyPanel.AutoBright = true;
    delay(10);
    MyPanel.Clear();
    MyPanel.DispNum(0, 0, 0xffffff);
    MyPanel.Show();
}
//-------------------------------
void loop()
{
    CurBright = MyPanel.GetBright();
    //    Serial.printf("CurBright = %d  PreBright = %d\n", CurBright, PreBright);
 
    if ((CurBright - PreBright) > 5 && PreBright < 5)
    {
        Dice();
    }
    PreBright = CurBright;
    delay(200);
}

//-------------------------------
void Dice()
{
    randomSeed(millis());
    int num;
    int temp;
    uint32_t clr = color[random(0, 7)];

    //数字を30回変化させる　変化する間隔はだんだん遅くなる
    for (int i = 0; i < 30; i++)
    {
        temp = random(1, 7);
        if (temp == num)
            continue;
        num = temp;
        MyPanel.DispNum(0, num, clr);
        MyPanel.Show();
        delay(i * 20);
    }
    //停止した数字を点滅3回
    for (int j = 0; j < 3; j++)
    {
        MyPanel.DispNum(0, num, 0);
        MyPanel.Show();
        delay(200);
        MyPanel.DispNum(0, num, clr);
        MyPanel.Show();
        delay(200);
    }
}
