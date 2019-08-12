//----------------------------------------------------------------------------//
//-------------------------------Includes-------------------------------------//
//----------------------------------------------------------------------------//

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//----------------------------------------------------------------------------//
//-----------------------------Variables I/O----------------------------------//
//----------------------------------------------------------------------------//

#define analogbattery A0
// SCL = D1
// SDA = D2
#define NeoPixelPin D3
#define BuzzerPin D4

//----------------------------------------------------------------------------//
//-----------------------------Variables NeoPixel-----------------------------//
//----------------------------------------------------------------------------//

#define NeoPixelNum  4
#define StartColored true
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

//----------------------------------------------------------------------------//
//--------------------------------Variables Display---------------------------//
//----------------------------------------------------------------------------//

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

//----------------------------------------------------------------------------//
//----------------------------Variables Millis--------------------------------//
//----------------------------------------------------------------------------//
long lastMessage;
#define MessageInterval 2000
long FirstMessage;
#define FirstInterval 5000
long lastPixel;
#define PixelInterval 100
long lastPressed = 0;
#define PressedInterval 1000
//long lastDontPress = 0;
//#define DontPressInterval 1500
long lastDisplay = 0;
#define DisplayInterval 2000
//#define BrightInterval 300

//----------------------------------------------------------------------------//
//--------------------------Variabeln Network---------------------------------//
//----------------------------------------------------------------------------//

char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password

bool SendAllowed = false;
bool GameisTrue = false;
bool PressedTrue = false;
//bool AnswerTrue = false;
IPAddress IP_Master(192, 168, 4, 1);     // IP address of the AP
String IP_long;
String IP_short;
String MAC_Address;
String MAC_Master;
String ToLate = "";
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
    if ((millis() - lastPressed) >= PressedInterval) {
      if(ToLate != ""){
        //int ToLateAsInt = float(ToLate).toInt;
        //if(ToLateAsInt <= 10){
        Display_Write(ToLate + " Sec to late");
        //}else{
          //Display_Write("far to late");
        //}
      }else{
        if(PressedTrue == false){
        Display_Write("Pressed");
        Serial.println("Pressed");
        lastPressed = millis();
        }

      lastDisplay = millis();
      if (SendAllowed && GameisTrue) {// && !AnswerTrue) {
        WiFi_Write("BP", "");
        lastMessage = millis();
        lastPressed = millis();
        PressedTrue = true;
      }
      }
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

  if (!SendAllowed && (FirstMessage + FirstInterval) <= millis()) {
    WiFi_Write("NB", MAC_Address);
    FirstMessage = millis();
  }

  // Use WiFiClient class to create TCP connections
  //if((millis() - lastPixel) >= PixelInterval){
    //NeoPixel(NeoRed, NeoGreen, NeoBlue);
  //}
  //read back one line from server
  WiFiClient client = Port_Master.available();
  if (client) {
    String request = client.readStringUntil('\r');
    Client.flush();
    Serial.print("Nachricht: ");
    Serial.println("********************************");
    Serial.println("From the station: " + request);
    //Display_Write(request);
    lastDisplay = millis();

    if (request == "AC") {
      SendAllowed = true;
    }
    if (SendAllowed) {
      if (request == "DE") {
        SendAllowed = false;
        GameisTrue = false;
        //AnswerTrue = false;
      }
      if (request == "AR" && GameisTrue) {// && !AnswerTrue) {
        NeoPixelColor_Write("GREEN", 255);
        //AnswerTrue = true;
      }
      if (request == "AW" && GameisTrue) {//&& !AnswerTrue) {
        NeoPixelColor_Write("RED", 255);
        //AnswerTrue = true;
      }
      if (request == "GR" && GameisTrue) {
        NeoPixelColor_Write("RESET", 0);
        PressedTrue = false;
        ToLate = "";
        //AnswerTrue = false;
      }
      if (request == "GB" && GameisTrue == false) {
        GameisTrue = true;
      }
      if (request == "GS" && GameisTrue) {
        GameisTrue = false;
        ToLate = "";
        //AnswerTrue = false;
      }
      if (request == "PA" && GameisTrue && PressedTrue) {
        NeoPixelColor_Write("BLUE", 255);
        PressedTrue = true;
      }
      String InputMessageType = getValue(request, ';', 0);
      if (InputMessageType == "TL" && GameisTrue) {
        ToLate = getValue(request, ';', 1);
        //int ToLateAsInt = float(ToLate).toInt
        //if(ToLateAsInt <= 10){
        Display_Write(ToLate + " Sec to late");
        //}else{
          //Display_Write("far to late");
        }
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
  Serial.println("\n");
}

//----------------------------------------------------------------------------//
//---------------------------------WiFi---------------------------------------//
//----------------------------------------------------------------------------//

void WiFi_Setup() {
  if (WiFi.status() != WL_CONNECTED) {
    SendAllowed = false;
    GameisTrue = false;
    PressedTrue = false;
    ToLate = "";
    //AnswerTrue = false;
    Display_Write("Connecting    ...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, KEY);           // connects to the WiFi AP
  }
  while (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() != WL_CONNECTED) {
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
    }
    if (WiFi.status() != WL_CONNECTED) {
      delay(250);
      Display_Write("Connecting    ... ");
      delay(375);
      NeoPixelColor_Write("RESET", 0);
      delay(125);
      Display_Write("Connecting    ....");
      delay(500);
    }
  }
  IP_long = WiFi.localIP().toString();
  IP_short = getValue(IP_long, '.', 3);
  MAC_Address = WiFi.macAddress();
  MAC_Master = WiFi.BSSIDstr();
  Serial.println();
  Serial.println("Connected");
  Display_Write("Connected");
  lastDisplay = millis();
  Serial.println("buzzer.ino");
  Serial.println("LocalIP: " + IP_long);
  Serial.println("LocalID: " + IP_short);
  Serial.println("MAC: " + MAC_Address);
  Serial.println("MAC Master: " + MAC_Master);
  Port_Master.begin();
  WiFi_Write("NB", MAC_Address);
  FirstMessage = millis();
  lastMessage = millis();
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
  /*NeoPixelColor_Write("PURPLE", 255);
  delay(1000);
  NeoPixelColor_Write("AQUA", 255);
  delay(1000);
  NeoPixelColor_Write("YELLOW", 255);
  delay(1000);
  NeoPixelColor_Write("BLUE", 255);
  delay(1000);
  NeoPixelColor_Write("RED", 255);
  delay(1000);
  NeoPixelColor_Write("GREEN", 255);
  delay(1000);
  /**/
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
  if (color == "RESET" || brightness == 0) {
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
  if (color == "YELLOW") {
    for (int i = 0; i < NeoPixelNum; i++) {
      RED[i] = brightness/2;
      GREEN[i] = brightness/2;
    }
    NeoPixel_Write(RED, GREEN, NONE);
  }
  if (color == "PURPLE") {
    for (int i = 0; i < NeoPixelNum; i++) {
      RED[i] = brightness/2;
      BLUE[i] = brightness/2;
    }
    NeoPixel_Write(RED, NONE, BLUE);
  }
  if (color == "AQUA") {
    for (int i = 0; i < NeoPixelNum; i++) {
      BLUE[i] = brightness/2;
      GREEN[i] = brightness/2;
    }
    NeoPixel_Write(NONE, GREEN, BLUE);
  }
}
