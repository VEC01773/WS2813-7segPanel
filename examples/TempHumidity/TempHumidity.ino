/*********************************/
// 温度・湿度計　センサー：AM2302
//
/*********************************/

#include "WS2813Panel.h"
#include "DHTesp.h"

#define PIN_BUTTON 0 //ボタン接続ピン
#define PIN_AM2302 4 //温湿度センサ(AM2302)制御ピン

#define PANEL_NUM 8 //パネルの数

//連結したパネル
WS2813Panel MyPanel(PANEL_NUM);
//温湿度センサ
DHTesp dht;

const uint8_t BRITE_MIN = 0x10;  //LEDの明るさ最低
const uint8_t BRITE_MAX = 0xFF; //LEDの明るさ最高　最大値はFF(255)　電流を見て決める
int MODE = 0;

uint32_t COLOR[3] = {0xff0000, 0x00ff00, 0x0000ff};

float humidity;	   //湿度
float temperature; //温度
float THI;		   //不快指数

int Interval = 3000; //データ更新時間(msec)
uint NextGetTime;	 //次回データ取得時間(msec)

uint8_t bright = 0x20; //明るさ
uint8_t swstat = 0;	 //ボタンの状態記録用　チャタリング防止

//-------------------------------------------------
// setup
//-------------------------------------------------
void setup()
{
	pinMode(0, INPUT);

	Serial.begin(115200);
	Serial.println("setup Start");

	//WS2813Panel初期化
	MyPanel.Begin();
	MyPanel.SetBrightness(bright);
	MyPanel.Show();

	//AM2302初期化
	dht.setup(PIN_AM2302, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
	//データ取得
	GetTempHumidDHT();
	NextGetTime = millis() + Interval;

	Serial.println("setup End");
}

//-------------------------------------------------
// loop
//-------------------------------------------------
void loop()
{
	//スイッチ監視(チャタリング防止)
	if (digitalRead(PIN_BUTTON) == LOW)
	{
		swstat = (swstat << 1) | 0x1;
		//Serial.println(swstat);
		if (swstat == 0x0F)
		{
			bright += 0x20;
			if (bright < BRITE_MIN)
				bright = BRITE_MIN;
			swstat = 0;
			MyPanel.SetBrightness(bright);
		}
	}
	else
	{
		swstat = 0;
	}
	//データ更新時間に到達したらデータ取得、次回取得時間の設定
	if (millis() > NextGetTime)
	{
		NextGetTime = millis() + Interval;
		GetTempHumidDHT();
	}

	//どちら0の場合は取得エラーとしてESPをリセット
	if (temperature == 0 && humidity == 0)
		ESP.restart();

	//不快指数を色にする、0(青)-50(緑)-100(赤)　寒い0　暑い100
	uint32_t LEDColor = ConvertColorTHI(THI);
	DispTemp(temperature, LEDColor);
	DispHumid(humidity, LEDColor);

	delay(1);
}

//------------------------------------
// 温度を表示
//------------------------------------
void DispTemp(float t, uint color)
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
		MyPanel.DispNum(pnl, dec_temp[7 - pnl], color);
	}
	//パネル2枚目に小数点
	MyPanel.DispDot(5, color);
	MyPanel.Show();
}

//------------------------------------
// 湿度を表示
//------------------------------------
void DispHumid(float h, uint color)
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
			MyPanel.DispNum(pnl, dec_temp[pnl], color);
	}
	//パネル2枚目に小数点
	MyPanel.DispDot(2, color);
	MyPanel.Show();
}

//-------------------------------------------------
//AM2302から温度・湿度・不快指数　取得　
//-------------------------------------------------
void GetTempHumidDHT()
{
	humidity = dht.getHumidity();
	temperature = dht.getTemperature();
	THI = 0.81 * temperature + 0.01 * humidity * (0.99 * temperature - 14.3) + 46.3;

	Serial.print("Temperature = ");
	Serial.print(temperature);
	Serial.println(" *C");

	Serial.print("Humidity = ");
	Serial.print(humidity);
	Serial.println(" %");

	Serial.print("THI = ");
	Serial.print(THI);
	Serial.println();
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