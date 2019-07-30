//Includes-----------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Variabeln I/O------------------------------------------------------------------------------
#define analogbattery A0
// SCL = D1
// SDA = D2
#define NeoPixelPin D3
#define BuzzerPin D4

#define NeoPixelNum  4
#define colored true
int NeoStart1[NeoPixelNum] = { 255,   0,   0,   0 };
int NeoStart2[NeoPixelNum] = {   0, 255,   0,   0 };
int NeoStart3[NeoPixelNum] = {   0,   0, 255,   0 };
int NeoStart4[NeoPixelNum] = { 255,   0,   0, 200 };
int NeoStart5[NeoPixelNum] = {   0, 255,   0, 200 };
int NeoStart6[NeoPixelNum] = {   0,   0, 255, 200 };
int NeoNone  [NeoPixelNum] = {   0,   0,   0,   0 };

//int NeoRed[NeoPixelNum] = { 255,0,0,200 };
//int NeoGreen[NeoPixelNum] = { 0,255,0,200 };
//int NeoBlue[NeoPixelNum] = { 0,0,255,200 };

//Variablen Display--------------------------------------------------------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix it!");
#endif

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NeoPixelNum, NeoPixelPin, NEO_GRB + NEO_KHZ800);

//Variabeln Sonstige-------------------------------------------------------------------------
#define type B
long lastMessage;
#define MessageInterval 2000
long lastPixel;
#define PixelInterval 100
long lastPressed = 0;
#define PressedInterval 500
//long lastDontPress = 0;
//#define DontPressInterval 1500
long lastDisplay = 0;
#define DisplayInterval 2000
//#define BrightInterval 300

//Variabeln Network--------------------------------------------------------------------------
char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password

bool SendAllowed = false;
bool GameisTrue = true;
IPAddress IP_Master(192, 168, 4, 1);     // IP address of the AP
String IP_long;
String IP_short;
String MAC_Address;
String MAC_Master;
WiFiClient Client;
WiFiServer Port_Master(88);

//----------------------------------------------------------------------------//
//----------------------------------Setup-------------------------------------//
//----------------------------------------------------------------------------//

void setup() {
	Prepare();
  NeoPixel_Setup();
  Display_Setup();
	#if (colored == true)
	NeoPixel_Write( NeoStart1, NeoNone, NeoNone);
	delay(500);
	NeoPixel_Write( NeoStart1, NeoStart2, NeoNone);
	delay(500);
	NeoPixel_Write( NeoStart1, NeoStart2, NeoStart3);
	delay(500);
	NeoPixel_Write( NeoStart4, NeoStart5, NeoStart6);
	delay(500);
	#else

	#endif
	WiFi_Setup();
	/*NeoPixelColor("RESET", 0);
	for (int i = 0; i < NeoPixelNum; ++i) {
		NeoRed[i] = 0;
	}
	for (int i = 0; i < NeoPixelNum; ++i) {
		NeoGreen[i] = 0;
	}
	for (int i = 0; i < NeoPixelNum; ++i) {
		NeoBlue[i] = 0;
	}
  /**/
}

//----------------------------------------------------------------------------//
//-----------------------------------Loop-------------------------------------//
//----------------------------------------------------------------------------//

void loop() {
	if (digitalRead(BuzzerPin) == LOW) {
		if ((millis() - lastPressed) >= PressedInterval && SendAllowed == true) {
			WiFi_Write("BP", "");
			lastMessage = millis();
			Display_Write("Pressed");
			Serial.println("Pressed");
			NeoPixelColor_Write("BLUE", 255);
			lastPressed = millis();
		}
  }
  else {
		if ((millis() - lastDisplay) >= DisplayInterval){
			int BatteryCharge = round(analogRead(analogbattery) / 1000.00 * 100);
			Display_Write("Bat.: " + String(BatteryCharge) + "%");
			lastDisplay = millis();
		}
	}

  if (WiFi.status() != WL_CONNECTED) {
    WiFi_Setup();
  }

	if ((millis() - lastMessage) >= MessageInterval && SendAllowed == true) {
		WiFi_Write("SM", "");
		lastMessage = millis();
	}

	// Use WiFiClient class to create TCP connections
	//if((millis() - lastPixel) >= PixelInterval){
	  //NeoPixel(NeoRed, NeoGreen, NeoBlue);
	//}
	//read back one line from server
	WiFiClient client = Port_Master.available();
	if (client) {
		Serial.print("Nachricht: ");
		String request = client.readStringUntil('\r');
		Serial.println("********************************");
		Serial.println("From the station: " + request);
		Display_Write(request);
		lastDisplay = millis();
		Client.flush();

		if (request == "AC") {
			SendAllowed = true;
		}
		if (request == "AR" && GameisTrue == true) {
			NeoPixelColor_Write("GREEN", 255);
		}
		if (request == "AW" && GameisTrue == true) {
			NeoPixelColor_Write("RED", 255);
		}
		if (request == "GR" && GameisTrue == true) {
			NeoPixelColor_Write("RESET", 0);
		}
		if (request == "GB" && GameisTrue == false) {
			GameisTrue = false;
		}
		if (request == "GS" && GameisTrue == true) {
			GameisTrue = false;
		}
	}
}

//----------------------------------------------------------------------------//
//--------------------------------Functions-----------------------------------//
//----------------------------------------------------------------------------//

String getValue(String data, char separator, int index) {
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length() - 1;
	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void Prepare() {  // for pinModes and more .....
  pinMode(BuzzerPin, INPUT_PULLUP);
	Serial.begin(9600);
}

//----------------------------------------------------------------------------//
//---------------------------------WiFi---------------------------------------//
//----------------------------------------------------------------------------//

void WiFi_Setup() {
	Display_Write("Connecting    ...");
  WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, KEY);           // connects to the WiFi AP
  while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		NeoPixelColor_Write("RED", 100);
		Display_Write("Connecting     ...");
		delay(500);
		Display_Write("Connecting    . ..");
		delay(125);
		NeoPixelColor_Write("RESET", 0);
		delay(375);
		Display_Write("Connecting    .. .");
		delay(250);
		NeoPixelColor_Write("RESET", 100);
		delay(250);
		Display_Write("Connecting    ... ");
		delay(375);
		NeoPixelColor_Write("RESET", 0);
		delay(125);
		Display_Write("Connecting    ....");
		delay(500);
	}
  IP_long = WiFi.localIP().toString();
  IP_short = getValue(IP_long, '.', 3);
  MAC_Address = WiFi.macAddress();
  MAC_Master = WiFi.BSSIDstr();
	Serial.println();
	Serial.println("Connected");
	Serial.println("station_bare_01.ino");
	Serial.println("LocalIP: " + IP_long);
  Serial.println("LocalID: " + IP_short);
	Serial.println("MAC: " + MAC_Address);
	Serial.println("IP  Master: " + WiFi.gatewayIP());
	Serial.println("MAC Master: " + MAC_Master);
	Port_Master.begin();
  WiFi_Write("NB", MAC_Address);
  lastMessage = millis();
	Display_Write("Connected");
}

void WiFi_Write(String MessageType, String Parameters){
  if (WiFi.status() == WL_CONNECTED) {
		Client.connect(IP_Master, 80);
		IP_long = WiFi.localIP().toString();
		IP_short = getValue(IP_long, '.', 3);
    if (Parameters != NULL || Parameters != "") {
      Client.print(MessageType + ";" + IP_short + ";" + Parameters + '\r');
      Serial.println(MessageType + ";" + IP_short + ";" + Parameters + '\r');
    }
    else{
      Client.print(MessageType + ";" + IP_short + '\r');
      Serial.println(MessageType + ";" + IP_short + '\r');
    }
	}
	else {
		Serial.println("No connectinon");
  }
}

//----------------------------------------------------------------------------//
//-----------------------------------Display----------------------------------//
//----------------------------------------------------------------------------//

void Display_Setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
	display.setRotation(2);
	display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
  Display_Write("Starting..");
}

void Display_Write(String Message) {
  display.setCursor(0, 0);
	display.println(Message);
	display.display();
	display.clearDisplay();
}

void Display_Reset() {
  display.setCursor(0, 0);
	display.println("");
	display.display();
	display.clearDisplay();
}

//----------------------------------------------------------------------------//
//----------------------------------NeoPixel----------------------------------//
//----------------------------------------------------------------------------//

void NeoPixel_Setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  //NeoPixel_Write(NeoRed, NeoGreen, NeoBlue);
	Serial.println("Pixel aktuell");
}

void NeoPixel_Write(int Red[NeoPixelNum], int Green[NeoPixelNum], int Blue[NeoPixelNum]) {
	for (int i = 0; i < NeoPixelNum; i++) {
		pixels.setPixelColor(i, pixels.Color(Red[i], Green[i], Blue[i]));
		pixels.show();
	}
	lastPixel = millis();
}

void NeoPixelColor_Write(String color, int brightness) {
	int RED[NeoPixelNum];
	int GREEN[NeoPixelNum];
	int BLUE[NeoPixelNum];
	int NONE[NeoPixelNum];
	for (int i = 0; i < NeoPixelNum; ++i) {
		NONE[i] = 0;
	}
	if (brightness == 0) {
		NeoPixel_Write(NONE, NONE, NONE);
	}
	if (color == "RESET") {
		NeoPixel_Write(NONE, NONE, NONE);
	}
	if (color == "RED") {
		for (int i = 0; i < NeoPixelNum; ++i) {
			RED[i] = brightness;
		}
		NeoPixel_Write(RED, NONE, NONE);
	}
	if (color == "GREEN") {
		for (int i = 0; i < NeoPixelNum; ++i) {
			GREEN[i] = brightness;
		}
		NeoPixel_Write(NONE, GREEN, NONE);
	}
	if (color == "BLUE") {
		for (int i = 0; i < NeoPixelNum; ++i) {
			BLUE[i] = brightness;
		}
		NeoPixel_Write(NONE, NONE, BLUE);
	}
}
