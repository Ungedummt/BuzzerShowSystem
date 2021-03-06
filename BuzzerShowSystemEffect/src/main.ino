//----------------------------------------------------------------------------//
//-------------------------------Includes-------------------------------------//
//----------------------------------------------------------------------------//

#include <ESP8266WiFi.h>
#include <ESPDMX.h>

//----------------------------------------------------------------------------//
//-----------------------------Variables I/O----------------------------------//
//----------------------------------------------------------------------------//

#define statusLED D7

//----------------------------------------------------------------------------//
//-----------------------------Variables DMX----------------------------------//
//----------------------------------------------------------------------------//

int StartChannelNum[20] = { 1,9,18 };
int brgbwChannelpostition[5] = { 4,5,6,7,8 }; //das ist die position im bereich wo RGB liegt
int effect[20];

DMXESPSerial dmx;
int Channelposition;
int StartChannel = 0;

int brgbwRightColor[5] = { 255,0,255,0,0 }; //w nur falls existent ansonsten 0
int brgbwWrongColor[5] = { 255,255,0,0,0 }; //w nur falls existent ansonsten 0
int brgbwStandbyColor[5] = { 255,150,150,150,150 }; //w nur falls existent ansonsten 0
int brgbwPressedPlayerColor[5] = { 255,255,255,255,255 }; //dies ist die Farbe die bei dem Spieler Erscheint der als Erster Gedrückt hat
int brgbwOtherPlayersColorRight[5] = { 0,0,0,0,50 }; //dies ist die Farbe die bei den anderen Spielern Erscheint wenn jemand gedrückt hat
int brgbwOtherPlayersColorWrong[5] = { 0,0,0,0,50 }; //dies ist die Farbe die bei den anderen Spielern Erscheint wenn jemand gedrückt hat
int brgbwColor[5];
int brgbwNone[5] = { 0,0,0,0,0 };

//Animations------------------------------------------------------------------------------------------------------------------
int Animation1[5] = { 255,0  ,0  ,0  ,0   };
int Animation2[5] = { 255,145,160,145,145 };
int Animation3[5] = { 255,135,180,135,135 };
int Animation4[5] = { 255,115,205,115,115 };
int Animation5[5] = { 255,100,235,100,100 };
int Animation6[5] = { 255,0  ,255,0  ,0   };

//----------------------------------------------------------------------------//
//----------------------------Variables Millis--------------------------------//
//----------------------------------------------------------------------------//

long lastMessage;
#define MessageInterval 2000
long lastDisplay = 0;
#define DisplayInterval 2000
long lastDMX = 0;
#define DMXInterval 20

//----------------------------------------------------------------------------//
//--------------------------Variabeln Network---------------------------------//
//----------------------------------------------------------------------------//

char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password

bool SendAllowed = false;
bool GameisTrue = false;
bool AnswerTrue = false;
IPAddress IP_Master(192, 168, 4, 1);     // IP address of the AP
String IP_long;
String IP_short;
String MAC_Address;
String MAC_Master;
WiFiClient Client;
WiFiServer Port_Master(88);

void setup() {
	Prepare();
	DMX_Setup();
	//WiFi_Setup();
}

void loop() {
	//if (WiFi.status() != WL_CONNECTED) {
  //  WiFi_Setup();
	//}

	//if ((millis() - lastMessage) >= MessageInterval && SendAllowed == true) {
	//	WiFi_Write("SM", "");
	//	lastMessage = millis();
	//}

	//DMX_Lamp(1, brgbwChannelpostition, brgbwRightColor);// Muss Array mit 5 und 4 stellen sein
	//DMX_Lamp(2, brgbwChannelpostition, brgbwWrongColor);

	DMX_Lamp_Color(1, brgbwChannelpostition, "GREEN", 255);
	DMX_Lamp_Color(2, brgbwChannelpostition, "ALL", 255);
	
	//DMX_Lamp(1, brgbwChannelpostition, Animation1);
	//Wait(300);
	//DMX_Lamp(1, brgbwChannelpostition, Animation2);
	//Wait(300);
	//DMX_Lamp(1, brgbwChannelpostition, Animation3);
	//Wait(300);
	//DMX_Lamp(1, brgbwChannelpostition, Animation4);
	//Wait(300);
	//DMX_Lamp(1, brgbwChannelpostition, Animation5);
	//Wait(300);
	//DMX_Lamp(1, brgbwChannelpostition, Animation6);
	//Wait(2000);
	DMX_Update();

	//WiFiClient client = Port_Master.available();
	//if (client) {
	//	Serial.print("Nachricht: ");
	//	String request = client.readStringUntil('\r');
	//	Serial.println("********************************");
	//	Serial.println("From the station: " + request);
	//	Client.flush();
	//	if (request == "AC") {
	//		SendAllowed = true;
	//	}
	//}
}

//----------------------------------------------------------------------------//
//--------------------------------Function DMX--------------------------------//
//----------------------------------------------------------------------------//

void DMX_Setup() {
	Serial.println("Starting DMX Setup ...");
	dmx.init();  // initialization for complete bus
	delay(100);
	Serial.println("Ending DMX Setup ...");
}

void DMX_Lamp_Color(int Lamp, int brgbwPosition[5], String color, int brightness) {
	int RED = brightness;
	int GREEN = brightness;
	int BLUE = brightness;
	int WHITE = brightness;
	int NONE = 0;
	int BRIGHT = 255;
	if (color == "RESET" || brightness == 0) {
		DMX_Lamp(Lamp, brgbwPosition, brgbwNone);
	}
	if (color == "RED") {
		int brgbwColor[5] = {BRIGHT, RED, NONE, NONE, NONE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "GREEN") {
		int brgbwColor[5] = {BRIGHT, NONE, GREEN, NONE, NONE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "BLUE") {
		int brgbwColor[5] = {BRIGHT, NONE, NONE, BLUE, NONE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "PURPLE") {
		int brgbwColor[5] = {BRIGHT, RED/2, NONE, BLUE/2, NONE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "YELLOW") {
		int brgbwColor[5] = {BRIGHT, RED/2, GREEN/2, NONE, NONE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "AQUA") {
		int brgbwColor[5] = {BRIGHT, NONE, GREEN/2, BLUE/2, NONE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "WHITE") {
		int brgbwColor[5] = {BRIGHT, NONE, NONE, NONE, WHITE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
	if (color == "ALL") {
		int brgbwColor[5] = {BRIGHT, RED, GREEN, BLUE, WHITE};
		DMX_Lamp(Lamp, brgbwPosition, brgbwColor);
	}
}

void DMX_Update() {
	if ((millis() - lastDMX) >= DMXInterval) {
		DMX_Send();
		lastDMX = millis();
	}
}

void DMX_Send() {
	for (int x = 1; x < 20; x++) {
		dmx.write(x, effect[x]);
	}
	dmx.update();
}

void Update_Effect_Var(int Channel, int Value) {
	effect[Channel] = Value;
}

void DMX_Lamp(int Lamp, int brgbwPosition[5], int brgbwColor[5]) {
	for (int i = 0; i < 5; i++) {
		Channelposition = brgbwPosition[i];
		if (Channelposition != 0) {
			int Channel = (StartChannelNum[Lamp - 1] + Channelposition - 1);
			int Color = brgbwColor[i];
			Update_Effect_Var(Channel, Color);
		}
	}
}

//----------------------------------------------------------------------------//
//--------------------------------Functions-----------------------------------//
//----------------------------------------------------------------------------//

String Get_Value(String data, char separator, int index) {
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
	Serial.begin(9600);
}

void Wait(int time) {
	long start = millis();
	while ((start + time) >= millis()) {
		DMX_Update();
		delay(20);
	}
}

//----------------------------------------------------------------------------//
//---------------------------------WiFi---------------------------------------//
//----------------------------------------------------------------------------//

void WiFi_Setup() {
  WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, KEY);           // connects to the WiFi AP
  while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		DMX_Lamp_Color(1, brgbwChannelpostition, "RED", 255);
		DMX_Lamp_Color(2, brgbwChannelpostition, "RESET", 255);
		Wait(500);
		DMX_Lamp_Color(2, brgbwChannelpostition, "RED", 255);
		DMX_Lamp_Color(1, brgbwChannelpostition, "RESET", 255);
		Wait(500);
	}
  IP_long = WiFi.localIP().toString();
  IP_short = Get_Value(IP_long, '.', 3);
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
  WiFi_Write("NE", MAC_Address);
  lastMessage = millis();
}

void WiFi_Write(String MessageType, String Parameters){
  if (WiFi.status() == WL_CONNECTED) {
		Client.connect(IP_Master, 80);
		IP_long = WiFi.localIP().toString();
		IP_short = Get_Value(IP_long, '.', 3);
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
