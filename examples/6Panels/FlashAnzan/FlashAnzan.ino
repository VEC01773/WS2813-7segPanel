/*******************************************************
　Flash　暗算
********************************************************/

#include "WS2813Panel.h"

#define PIN_BUTTON 0 //ボタン接続ピン
#define PANEL_NUM 6

const uint8_t BRITE_MIN = 0x10; //LEDの明るさ最低
const uint8_t BRITE_MAX = 0xFF; //LEDの明るさ最高　最大値はFF(255)　電流を見て決める

WS2813Panel MyPanel(PANEL_NUM);

uint32_t color[] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xff00ff, 0x00ffff, 0xffffff};
uint8_t bright = 0x20; //明るさ
uint32_t swstat = 0;   //ボタンの状態記録用　チャタリング防止

int PreBright = 0xff;
int CurBright = 0;

uint32_t flash_tnterval = 1000; //msec
uint8_t keta = 1;               //桁
uint8_t flash_count = 10;       //回数

//-------------------------------
void setup()
{
    pinMode(PIN_BUTTON, INPUT);

    Serial.begin(115200);
    Serial.println("Program Start");

    Serial.println("LED Initialze");
    MyPanel.Begin();
    //明るさ
    MyPanel.SetBrightness(50);
    delay(10);
    MyPanel.Clear();
    for (int i = 0; i < keta; i++)
    {
        MyPanel.DispNum(i, 0x11, 0xffffff);
    }
    MyPanel.Show();
}
//-------------------------------
void loop()
{
    CurBright = MyPanel.GetBright();
    //    Serial.printf("CurBright = %d  PreBright = %d\n", CurBright, PreBright);
    //スイッチ監視(チャタリング防止)
    if (digitalRead(PIN_BUTTON) == LOW)
    {
        swstat = (swstat << 1) | 0x1;

        //4秒長押しで間隔を短く
        if (swstat == 0xFFFFFFFF)
        {
            keta -= 1;
            flash_tnterval -= 200;
            if (flash_tnterval <= 0)
                flash_tnterval = 2000;
            //           swstat = 0;
            MyPanel.Clear();
            MyPanel.Show();
            int32_t temp = flash_tnterval;
            for (int i = 0; i < PANEL_NUM; i++)
            {
                if (temp > 0)
                    MyPanel.DispNum(i, temp % 10, 0xff0000);
                else
                    MyPanel.DispNum(i, temp % 10, 0);
                temp /= 10;
            }
            MyPanel.Show();
            delay(400);
            Serial.printf("flash_interval = %d\n", flash_tnterval);
            return;
        }
        //桁数を増やす
        if (swstat == 0x0F)
        {
            keta += 1;
            if (keta >= PANEL_NUM)
                keta = 1;
            Serial.printf("keta = %d\n", keta);
            MyPanel.Clear();
            for (int i = 0; i < keta; i++)
            {
                MyPanel.DispNum(i, 0x11, 0xffffff);
            }
            MyPanel.Show();
        }
        delay(100);
        return;
    }
    else
    {
        swstat = 0;
    }

    if ((CurBright - PreBright) > 5 && PreBright < 5)
    {
        Flash();
    }
    PreBright = CurBright;
    delay(200);
}

//-------------------------------
void Flash()
{
    randomSeed(millis());
    int num;
    int pre_num;
    int cnt = flash_count;
    int32_t ans = 0;
    int32_t temp;
    MyPanel.Clear();
    MyPanel.Show();
    //Start前にコロンを点滅
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < keta; i++)
        {
            MyPanel.DispColon(i, 0x7f7f7f);
        }
        MyPanel.Show();
        delay(400);
        for (int i = 0; i < keta; i++)
        {
            MyPanel.DispColon(i, 0);
        }
        MyPanel.Show();
        delay(400);
    }

    //数字をflash_count回表示
    while (cnt > 0)
    {
        num = random(pow10(keta - 1), pow10(keta) - 1);
        if (pre_num == num)
        {
            Serial.println("連続");
            continue;
        }
        pre_num = num;
        //合計
        ans += num;
        temp = num;
        for (int i = 0; i < keta; i++)
        {
            MyPanel.DispNum(i, temp % 10, 0x0000ff);
            temp /= 10;
        }
        MyPanel.Show();
        delay(flash_tnterval);
        cnt--;
    }

    MyPanel.Clear();
    MyPanel.Show();
    delay(1000);

    //合計をゆっくり表示
    for (int j = 0; j <= 100; j += 5)
    {
        temp = ans;
        for (int i = 0; i < PANEL_NUM; i++)
        {
            if (temp > 0)
            {
                if(j < 100)
                    MyPanel.DispNum(i, temp % 10, j + (j << 8) + (j << 16));
                else
                    MyPanel.DispNum(i, temp % 10, 0 + (j << 8) + (0 << 16));
             }
            else
            {
                MyPanel.DispNum(i, temp % 10, 0);
            }
            temp /= 10;
        }
        MyPanel.Show();
        delay(200);
    }
}
