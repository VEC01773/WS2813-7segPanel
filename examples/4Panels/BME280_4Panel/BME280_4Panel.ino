/*
BME280_4Panels

パネル4枚で、温度、湿度、気圧を表示する。
温湿度の表示色はを不快指数を色で表す。
不快指数に対する色は、0(青)-50(緑)-100(赤)　寒い0　暑い100
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WS2813Panel.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define DEBUG 0 //デバッグ時1

WS2813Panel MyPanel(4);                                                //4連結パネル
Adafruit_BME280 bme;                                                   // I2C
uint32_t COLOR[] = {0xffffff, 0x0000ff, 0x00ff00, 0xff0000, 0xff00ff}; // 白、青、緑、赤、紫

unsigned long delayTime = 2000; //表示更新間隔(msec)

float temp = 10;       //温度
float humi = 50;       //湿度
float pressure = 1010; //気圧

int dispmode = 0; //表示データ　0:温度、1:湿度、2:気圧

//---------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // time to get serial running

  //パネルの初期化
  MyPanel.Begin();
  //自動輝度調整ON
  MyPanel.AutoBright = true;
  delay(10);
  MyPanel.Clear();
  MyPanel.Show();

  Serial.println(F("BME280 1Panel Start"));

  //BME280 Setting
  Wire.begin(4, 5);
  unsigned status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    while (1)
    {
      MyPanel.Clear();
      MyPanel.DispNum(0, 0x0e, 0xff0000);
      MyPanel.Show();
      delay(200);
      MyPanel.DispNum(0, 0x0e, 0);
      MyPanel.Show();
      delay(200);
    }
  }

  Serial.println("BME280 connected.");
  Serial.println();
}

//---------------------
void loop()
{
  //温湿度、気圧取得
  bool ret = GetValueBME();
  if (ret == false)
  {
    MyPanel.Clear();
    MyPanel.DispNum(0, 0x0e, 0xff0000);
    MyPanel.Show();
    delay(200);
    MyPanel.DispNum(0, 0x0e, 0);
    MyPanel.Show();
    delay(200);
    return;
  }
  //不快指数計算 & 不快指数色
  float thi = ConvertHTI(temp, humi);
  uint32_t colr = ConvertColorTHI(thi);

  Serial.printf("temp = %f : humid = %f : press = %f : THI = %f\n", temp, humi, pressure, thi);

  if (dispmode == 0) //温度
  {
    DispTemp(temp, colr);
    dispmode++;
  }
  else if (dispmode == 1) //湿度
  {
    DispHumid(humi, colr);
    dispmode++;
  }
  else if (dispmode == 2) //気圧
  {
    DispPress(pressure, colr);
    dispmode = 0;
  }
  delay(delayTime);
}

//---------------------
bool GetValueBME()
{
  bool ret = true;

  temp = bme.readTemperature();
  humi = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  //温度が-30℃以下はエラーと判断
  if (temp < -30)
    ret = false;
  return ret;
}
//---------------------------
// 温度表示3桁
//---------------------------
void DispTemp(float temp, uint32_t colr)
{
  int t = abs(round(temp * 10.0));
  int num0 = t % 10;
  int num1 = (t / 10) % 10;
  int num10 = (t / 100);

  MyPanel.Clear();
  MyPanel.Show();

  if (temp < -9.9) //「-L」表示
  {
    MyPanel.DispNum(3, 0x11, colr); //-
    MyPanel.DispNum(2, 0x17, colr); //L
    MyPanel.DispNum(0, 0x14, colr); //℃
    MyPanel.Show();                 // Send the updated pixel colors to the hardware.
    delay(400);
  }
  else if (temp < 0) //マイナス
  {
    MyPanel.DispNum(3, 0x11, colr); //-
    MyPanel.DispNum(2, num1, colr); //
    MyPanel.DispDot(2, colr);       //小数点
    MyPanel.DispNum(1, num0, colr); //
    MyPanel.DispNum(0, 0x14, colr); //℃
    MyPanel.Show();                 // Send the updated pixel colors to the hardware.
  }
  else
  {
    if (num10 == 0)
      MyPanel.DispNum(3, 0x10, colr); //空白
    else
      MyPanel.DispNum(3, num10, colr); //10の位
    MyPanel.DispNum(2, num1, colr);    //
    MyPanel.DispDot(2, colr);          //小数点
    MyPanel.DispNum(1, num0, colr);    //少数1桁
    MyPanel.DispNum(0, 0x14, colr);    //℃
    MyPanel.Show();                    // Send the updated pixel colors to the hardware.
    delay(400);
  }
}

//---------------------------
// 湿度表示3桁
//---------------------
void DispHumid(float humi, uint32_t colr)
{
  int h = round(humi * 10.0);
  int num0 = h % 10;
  int num1 = (h / 10) % 10;
  int num10 = (h / 100);

  MyPanel.Clear();
  MyPanel.Show();

  if (num10 == 0)
    MyPanel.DispNum(3, 0x10, colr); //空白
  else
    MyPanel.DispNum(3, num10, colr);

  MyPanel.DispNum(2, num1, colr);
  MyPanel.DispDot(2, colr); //小数点
  MyPanel.DispNum(1, num0, colr);
  MyPanel.DispNum(0, 0x15, colr); //%
  MyPanel.Show();                 // Send the updated pixel colors to the hardware.
  delay(400);
}

//---------------------------
// 気圧表示4桁
//---------------------
void DispPress(float pressure, uint32_t colr)
{
  int p = round(pressure);
  int num1 = p % 10;
  int num10 = (p / 10) % 10;
  int num100 = (p / 100) % 10;
  int num1000 = (p / 1000) % 10;

  MyPanel.Clear();
  MyPanel.Show();
  if (num1000 == 0)
    MyPanel.DispNum(3, 0x10, colr); //空白
  else
    MyPanel.DispNum(3, num1000, colr);
  MyPanel.DispNum(2, num100, colr);
  MyPanel.DispNum(1, num10, colr);
  MyPanel.DispNum(0, num1, colr);
  MyPanel.Show(); // Send the updated pixel colors to the hardware.
  delay(400);
}

//-------------------------------------------------
//温度・湿度から不快指数計算　
//-------------------------------------------------
float ConvertHTI(float temp, float humid)
{
  float THI = 0.81 * temp + 0.01 * humid * (0.99 * temp - 14.3) + 46.3;

  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Humidity = ");
  Serial.print(humid);
  Serial.println(" %");

  Serial.print("THI = ");
  Serial.print(THI);
  Serial.println();
  return THI;
}

//-------------------------------------------------
//不快指数を色にする、0(青)-50(緑)-100(赤)　寒い0　暑い100
//-------------------------------------------------
uint32_t ConvertColorTHI(float thi)
{
  uint32_t LEDColor = 0;
  byte R;
  byte G;
  byte B;
  if (thi > 85)
  {
    R = 0xff;
    B = 0;
    G = 0;
  }
  else if (thi < 55)
  {
    R = 0;
    B = 0xff;
    G = 0;
  }
  else if (thi > 67.5)
  {
    R = (thi - 67.5) / (85.0 - 67.5) * 0xff;
    G = 0xff - R;
    B = 0;
  }
  else if (thi <= 67.5)
  {
    B = (thi - 67.5) / (55.0 - 67.5) * 0xff;
    G = 0xff - B;
    R = 0;
  }
  //	Serial.printf("%2X %2X %2X\n", R, G, B);

  LEDColor = (R << 16) + (G << 8) + B;
  Serial.printf("%2X %2X %2X %8X\n", R, G, B, LEDColor);

  return LEDColor;
}