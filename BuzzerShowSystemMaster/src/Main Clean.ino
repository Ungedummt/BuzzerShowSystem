#include <ESP8266WiFi.h>

//Variabeln I/O------------------------------------------------------------------------------
//Buttons
int addBuzzerButton[2]{825,828};
int ResetGameButton[2]{845,846};
int wrongButton[2]{842,843};
int rightButton[2]{835,838};
int GameBegin[2]{805,806};


//LEDS
#define addBuzzerLED D3
#define RichtigButtonLED D0
#define SpielBeginButtonLED D4
#define ResetButtonLED D2
#define FalschButtonLED D1

//Arrays
int IPArray [20] = {};
int TimeArray [20] = {};
String TypeArray [20] = {};


//Sonstiges
long requestTime = 0;
String type;
String ip;
boolean finish;
boolean GamesState;
int MasterAnswer; //0 = nichts / 1 = Richtig / 2 = Falsch
float millis_;
float millisOld;
float Time;
boolean newBuzzer = false;
String request;

//Network Variabeln--------------------------------------------------------------------------
char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password
#define CHANNEL 1
#define HIDDEN true
int max_connection = 0;
int ClientNum = 0;

//Komunikation
WiFiServer APServerPort(80);
IPAddress IP(192, 168, 4, 1);
IPAddress mask = (255, 255, 255, 0);

IPAddress ClientServerAddress(192,168,4,100);     // IP address of the ClientServer
WiFiClient APclient;

//===========================================================================================

void setup() {
  prepare();
  Display_Setup();
  WiFi_Setup();
}

//===========================================================================================

void loop() {
  TheGameMaster();
}

//===========================================================================================

void prepare(){
  Serial.begin(9600);
  WelcomeMessage();
}

//===========================================================================================

void Display_Setup(){
  //here will be the incoming Script for the Nextion Display
}

//===========================================================================================

void WiFi_Setup(){
WiFi.mode(WIFI_AP);
WiFi.softAP(SSID, KEY, CHANNEL, HIDDEN, max_connection);
WiFi.softAPConfig(IP, IP, mask);
APServerPort.begin();
Serial.println();
Serial.println("MasterServer started.");
Serial.println("IP: " + WiFi.softAPIP());
Serial.println("MAC:" + WiFi.softAPmacAddress());
}

//===========================================================================================

void WelcomeMessage(){
  Serial.println("Welcome to the Serial-Monitor of the BuzzerShowSystem"):
  Serial.println("here you will see all the important things that are");
  Serial.println("happening in the background");
}

//===========================================================================================

void TheGameMaster(){
DeviceHandler();
MessageReciever();
}

//===========================================================================================

String MessageReciever(String Message){

}

//===========================================================================================

void MessageTransmitter(String Message, int ID){

}

//===========================================================================================

void DeviceHandler(){       //Sorgt dafür das sich Geräte Verbinden und Vebunden Bleiben
DeviceTimeout();
}

//===========================================================================================

void DeviceTimeout(){       //Sorgt dafür das Geräte die nicht mehr Verbunden sind auch nicht mehr Angezeigt werden

}

void AddDevice(){           //Solte ein timeout beinhalten

}
