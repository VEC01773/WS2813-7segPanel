#include "WS2813Panel.h"
//#include <Adafruit_NeoPixel.h>
#include "DHTesp.h"

#define TRIGGER_PIN 0

#define PIN_AM2302 4 //温湿度センサ(AM2302)制御ピン
#define PANEL_NUM 8	 //パネルの数

WS2813Panel MyPanel(PANEL_NUM);
DHTesp dht;

const uint8_t BRITE_MIN = 10;  //LEDの明るさ最低
const uint8_t BRITE_MAX = 100; //LEDの明るさ最高　最大値は255　電流を見て決める
int MODE = 0;

uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};

float humidity;
float temperature;
float tempindex;
bool OUT = false;

int ChangeTime = 3000; //表示切替時間(msec)
uint NextChange;

//-------------------------------------------------
void setup()
{
	pinMode(0, INPUT);

	Serial.begin(115200);
	Serial.println("Start");
	MyPanel.Begin();
  MyPanel.SetBrightness(50);
	MyPanel.Show();
	Serial.println("End");

	dht.setup(PIN_AM2302, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
	GetTempHumidDHT();
}


//-------------------------------------------------
void loop()
{
	if (millis() > NextChange)
	{
		NextChange = millis() + ChangeTime;
	}
	GetTempHumidDHT();

	Serial.print("Temperature = ");
	Serial.print(temperature);
	Serial.println(" *C");

	Serial.print("Humidity = ");
	Serial.print(humidity);
	Serial.println(" %");

	if (temperature == 0 && humidity == 0)
		ESP.restart();

	uint32_t LEDColor = 0xff0000;

	byte R;
	byte G;
	byte B;
	if (tempindex > 85)
	{
		R = 0xff;
		B = 0;
		G = 0;
	}
	else if (tempindex < 55)
	{
		R = 0;
		B = 0xff;
		G = 0;
	}
	else if (tempindex > 67.5)
	{
		R = (tempindex - 67.5) / (85.0 - 67.5) * 0xff;
		G = 0xff - R;
		B = 0;
	}
	else if (tempindex <= 67.5)
	{
		B = (tempindex - 67.5) / (55.0 - 67.5) * 0xff;
		G = 0xff - B;
		R = 0;
	}

	DispTemp(temperature);
	DispHumid(humidity);

	delay(1);
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

	for (int pnl = 4; pnl < 8; pnl++)
	{
		MyPanel.DispNum(pnl, dec_temp[7-pnl], COLOR[0]);
	}
	//パネル2枚目に小数点
	MyPanel.DispDot(5, COLOR[0]);
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

//-------------------------------------------------
//AM2302から温度湿度取得
//-------------------------------------------------
void GetTempHumidDHT()
{
	humidity = dht.getHumidity();
	temperature = dht.getTemperature();
	tempindex = 0.81 * temperature + 0.01 * humidity * (0.99 * temperature - 14.3) + 46.3;
}
