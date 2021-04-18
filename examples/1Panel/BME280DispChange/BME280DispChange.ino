
/***************************************************************************
  BME280 で気温・湿度・気圧を測定して
  順番に表示
 ***************************************************************************/

#include "WS2813Panel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define PANEL_NUM 4

Adafruit_BME280 bme; // I2C

WS2813Panel MyPanel(PANEL_NUM);

uint8_t bright = 50;
uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};
int MODE = 0;

int ChangeTime = 3000; //表示切替時間(msec)
uint NextGetTime;

//------------------------------------
void setup()
{
  pinMode(0, INPUT);

  Serial.begin(115200);
  Serial.println("Start");

  MyPanel.Begin();
  MyPanel.Show();

  //I2c初期化　SDA=4　SDC=5　ピン
  Wire.begin(4, 5);
  unsigned status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    while (1)
      delay(10);
  }
  //次回画面切替タイム設定
  NextGetTime = millis() + ChangeTime;
}

//------------------------------------
void loop()
{
  if (millis() > NextGetTime)
  {
    MODE = (MODE + 1) % 3;
    NextGetTime = millis() + ChangeTime;
  }
  printValues(MODE);
  // int light = system_adc_read(); // 1024.0 ;
  uint8_t light = MyPanel.GetBright();
  Serial.printf("Bright = %d\n", light);
  MyPanel.SetBrightness(light * 2 + 20);

  delay(10);
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
	uint8_t dec_temp[4]; // [3][2].[1] [0]℃  -の場合は[3]が「-」で-9.9℃まで
	dec_temp[0] = 20;	 //℃
	//温度を10倍
	int tempx10 = int(abs(t * 10.0));

	for (int i = 1; i < 4; i++)
	{
		dec_temp[i] = tempx10 % 10;
		tempx10 = tempx10 / 10;
	}

	//マイナス
	if (t < 0)
	{
		dec_temp[3] = 0x11;
	}
	else
	{
		if (abs(t) < 10)
			dec_temp[3] = 0x10;
	}

	for (int pnl = 0; pnl < 4; pnl++)
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
  uint8_t dec_temp[4];
  dec_temp[0] = 21; //%
  //湿度を10倍
  int tempx10 = int(h * 10.0);

  for (int i = 1; i < 4; i++)
  {
    dec_temp[i] = tempx10 % 10;
    tempx10 = tempx10 / 10;
  }

  for (int pnl = 0; pnl < 4; pnl++)
  {
    if (pnl == 3 && dec_temp[pnl] == 0)
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
  uint8_t dec_temp[4];
  dec_temp[0] = 22; //h

  int tempx10 = p;

  for (int i = 1; i < 4; i++)
  {
    dec_temp[i] = tempx10 % 10;
    tempx10 = tempx10 / 10;
  }

  for (int pnl = 0; pnl < 4; pnl++)
  {
      MyPanel.DispNum(pnl, dec_temp[pnl], COLOR[2]);
  }
  MyPanel.Show();
}
