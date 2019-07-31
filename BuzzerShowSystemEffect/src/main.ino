//Includes-----------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <ESPDMX.h>

//Variabeln I/O------------------------------------------------------------------------------
#define statusLED D7

//Variabeln Sonstige-------------------------------------------------------------------------
int updateInterval = 20;
long lastUpdateDMX;
int StartChannel;
DMXESPSerial dmx;

//DMX Config
int IPArray[20] = { 100,101,103 };
int StartChannelNum[20] = { 1,9,18 };
int BRGBWChannelpostition[5] = { 4,5,6,7,8 }; //das ist die position im bereich wo RGB liegt
int channelPerEffect = 8;// Rasunehmen aund differenz berechnen únd StopChannelNum einfügen
int effect[20];

int rgbwRightColor[4] = { 0,255,0,0 }; //w nur falls existent ansonsten 0
int rgbwWrongColor[4] = { 255,0,0,0 }; //w nur falls existent ansonsten 0
int rgbwStandbyColor[4] = { 150,150,150,150 }; //w nur falls existent ansonsten 0
int rgbwPressedPlayerColor[4] = { 255,255,255,255 }; //dies ist die Farbe die bei dem Spieler Erscheint der als Erster Gedrückt hat
int rgbwOtherPlayersColorRight[4] = { 0,0,0,50 }; //dies ist die Farbe die bei den anderen Spielern Erscheint wenn jemand gedrückt hat
int rgbwOtherPlayersColorWrong[4] = { 0,0,0,50 }; //dies ist die Farbe die bei den anderen Spielern Erscheint wenn jemand gedrückt hat
int rgbwNone[4] = { 0,0,0,0 };

//Animations------------------------------------------------------------------------------------------------------------------
int Animation1[4] = { 0  ,0  ,0  ,0 };
int Animation2[4] = { 145,160,145,145 };
int Animation3[4] = { 135,180,135,135 };
int Animation4[4] = { 115,205,115,115 };
int Animation5[4] = { 75 ,235,75 ,75 };
int Animation6[4] = { 0  ,255,0  ,0 };

//Variabeln Network--------------------------------------------------------------------------
char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password
IPAddress serverIP(192, 168, 4, 1);    // IP address of the AP
WiFiClient APinput;
WiFiClient ClientOutput; // APinput name for AP mode
WiFiServer clientPort(80);

#define type 'E'
String IP_long;
String ClientNumber;
long lastMessage;

void setup() {
	Serial.begin(9600);               // initialization for first 32 addresses by default
	dmx.init();           // initialization for complete bus
	//delay(200);
	//put your setup code here, to run once:
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, KEY);           // connects to the WiFi AP

	if(WiFi.status() != WL_CONNECTED) {
	  Serial.println();
	  Serial.println("Connection to the AP");
	  connecting();
	}
	if(WiFi.status() == WL_CONNECTED){
	  APinput.connect(serverIP, 80);
	  IP_long = WiFi.localIP().toString();
	  ClientNumber = getValue(IP_long, '.', 3);
	  APinput.print(ClientNumber + ';' + "N" + type + ';' + false + '\r');
	  Serial.println(ClientNumber + ';' + "N" + type + ';' + false + '\r');
	  lastMessage = millis();
	  clientPort.begin();
	}
}

void loop() {
	//Auf Daten Warten
	if (clientPort.available()){
		Serial.print("Nachricht: ");
		String line = ClientOutput.readStringUntil('\r');
		Serial.println(line);
		ClientOutput.flush();
		ClientOutput.stop();
	}

	if (WiFi.status() != WL_CONNECTED) {
		Serial.println();
		Serial.println("Connection lost retry");
		connecting();
		if (WiFi.status() == WL_CONNECTED) {
			APinput.connect(serverIP, 80);
			IP_long = WiFi.localIP().toString();
			ClientNumber = getValue(IP_long, '.', 3);
			APinput.print(ClientNumber + ';' + "N" + type + ';' + false + '\r');
			Serial.println(ClientNumber + ';' + "N" + type + ';' + false + '\r');
			lastMessage = millis();
		}
	}
	// Use WiFiClient class to create TCP connections
	if ((millis() - lastMessage) >= 2000) {
		if (WiFi.status() == WL_CONNECTED) {
			lastMessage = millis();
			APinput.connect(serverIP, 80);
			IP_long = WiFi.localIP().toString();
			ClientNumber = getValue(IP_long, '.', 3);
			APinput.print(ClientNumber + ';' + type + ';' + false + '\r');
			Serial.println(ClientNumber + ';' + type + ';' + false + '\r');
		}else{
			Serial.println("No connectinon");
		}
	}
	//String SerRead = Serial.readString();
	// put your main code here, to run repeatedly:
	delay(5);

	if (Serial.read() == 'G') {
		UpdateLamp(1, BRGBWChannelpostition, rgbwStandbyColor);// Muss Array mit 5 und 4 stellen sein
		UpdateLamp(2, BRGBWChannelpostition, rgbwStandbyColor);
	}
	if ((lastUpdateDMX + updateInterval) <= millis()) {
		SendDMX();
		dmx.update();
		lastUpdateDMX = millis();
	}
}


void SendDMX() {
	for (int x = 1; x < 21; x++) {
		dmx.write(x, effect[x - 1]);
		//Serial.print(x);
		//Serial.print(" / ");
		//Serial.println(effect[x - 1]);
	}
	dmx.update();
}

void UpdateEffectVar(int channel, int value) {
	effect[channel] = value;
	//Serial.print(channel + 1);
	//Serial.print(" / ");
	//Serial.println(effect[channel]);
}

void UpdateLamp(int LampNum, int BRGBWPosition[5], int RGBWColor[4]) {
	StartChannel = StartChannelNum[LampNum - 1];
	//Serial.print(StartChannel);
	//Serial.print(" / ");
	//Serial.println(LampNum);
	if (BRGBWPosition[0] != 0) {
		UpdateEffectVar((StartChannel + BRGBWPosition[0] - 2), 255);
	}
	for (int i = 0; i < 4; i++) {
		if (BRGBWPosition[0] == 0) {
			int Channelposition = BRGBWPosition[i + 1];
		}
		int Channelposition = BRGBWPosition[i + 1];
		int Channel = (StartChannel + Channelposition - 2);
		int Color = RGBWColor[i];
		UpdateEffectVar(Channel, Color);
	}
}

void connecting() {
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		analogWrite(statusLED, 150);
		delay(250);
		analogWrite(statusLED, 0);
		delay(250);
	}
	analogWrite(statusLED, 50);

	Serial.println();
	Serial.println("Connected");
	Serial.println("station_bare_01.ino");
	Serial.println("LocalIP:" + WiFi.localIP());
	Serial.println("MAC:" + WiFi.macAddress());
	Serial.println("Gateway:" + WiFi.gatewayIP());
	Serial.println("AP MAC:" + WiFi.BSSIDstr());
	IP_long = WiFi.localIP().toString();
	ClientNumber = getValue(IP_long, '.', 3);
	Serial.println("Client Number: " + ClientNumber);
}

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
