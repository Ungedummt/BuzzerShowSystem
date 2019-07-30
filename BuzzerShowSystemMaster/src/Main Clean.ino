#include <ESP8266WiFi.h>

//Variabeln I/O------------------------------------------------------------------------------
//Buttons Ranges
int AddBuzzerButtonRange[2]{825,828};
int ResetGameButtonRange[2]{845,846};
int WrongButtonRange[2]{842,843};
int RightButtonRange[2]{835,838};
int GameBeginRange[2]{805,806};
//Button States
int AddBuzzerButtonState = false;
int ResetGameButtonState = false;
int WrongButtonState = false;
int RightButtonState = false;
int GameBeginState = false;


//LEDS
#define addBuzzerLED D3
#define RichtigButtonLED D0
#define SpielBeginButtonLED D4
#define ResetButtonLED D2
#define FalschButtonLED D1

//Arrays
#define MaxClients 10
int IPArray [MaxClients] = {};
int TimeArray [MaxClients] = {};
String TypeArray [MaxClients] = {};
String MacArray [MaxClients] = {};
String AcceptedMacsArray [MaxClients] = {"3C:71:BF:3A:0F:93","3C:71:BF:39:B2:78"};


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
String request;

//Network Variabeln--------------------------------------------------------------------------
char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password
#define CHANNEL 1
#define HIDDEN true
int max_connection = MaxClients;
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
  //Das ist unsere Uhr
  if ((millis_ - millisOld) >= 1000) {
    millisOld = millis_;
    millis_ = millis();
    requestTime = requestTime + 1;
  }
  else {
    millis_ = millis();
  }

  TheGameMaster(); //Hier werden Nachricheten ausgelesen und verwertet
  DeviceHandler();
  String Message = WiFi_Read();
  if(Message != ""){
    Serial.println(Message);
  }

//Nachricht in einzelne Variabeln aufteilen
  String DeviceMAC = getValue(Message, ';', 2);
  String DeviceID = getValue(Message, ';', 1);
  String MessageType = getValue(Message, ';', 0);
  int IDasInt = DeviceID.toInt();

//Nachricht Verwerten
  if(MessageType == "NB" || MessageType == "NE"){
      //Überprüft ob die Mac Zu den zugelassenen Macs gehört
      for (int i = 0; i < MaxClients;) {
        if (DeviceMAC == AcceptedMacsArray[i]) {
          Serial.println("Buzzer Zugelassen");
          WiFi_Write("AC", IDasInt);//This Code Accepts the Buzzer and requests his messages
          AddDevice(MessageType, IDasInt, DeviceMAC);
          i = MaxClients;
        }
        i++;
      }
  }

  char SerRead = Serial.read();
  if (SerRead == 'A') {
    WiFi_Write("AR", 101);
  }
  if (SerRead == 'B') {
    WiFi_Write("AW", 101);
  }

  if (SerRead == 'C') {
    WiFi_Write("AR", 100);
  }
  if (SerRead == 'D') {
    WiFi_Write("AW", 100);
  }

   if (SerRead == '0') {
    WiFi_Write("GB", 100);
  }
  if (SerRead == '1') {
    WiFi_Write("GB", 101);
  }


  SerRead = NULL;



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
  Serial.println();
  Serial.println("Welcome to the Serial-Monitor of the BuzzerShowSystem");
  Serial.println("here you will see all the important things that are");
  Serial.println("happening in the background");
}

//===========================================================================================

void BuzzerHandler(String Action, String Who){  //Sorgt dafir das die Buzzer vereinfacht angesprochen werden können

}

//===========================================================================================

String WiFi_Read(){
request = "";
WiFiClient client = APServerPort.available();
if (client) {
  request = client.readStringUntil('\r');
  client.flush();
}
return request;
}

//===========================================================================================

void TheGameMaster(){

}

//===========================================================================================

void WiFi_Write(String Message, int ID){
IPAddress ClientServerAddress(192, 168, 4, ID);
  APclient.connect(ClientServerAddress, 88);
  APclient.print(Message + "\r");
  Serial.print("Send: ");
  Serial.println(Message + '\r');
  Serial.print(" / To: ");
  Serial.println(ID);
}

//===========================================================================================

void DeviceHandler(){       //Sorgt dafür das sich Geräte Verbinden und Vebunden Bleiben (TimeOut)
  //GeräteTimeout Start--------------------------------------------------------------------
  if (ClientNum > 0) {
    for (int i = 0; i < ClientNum;) {
      //Serial.println((requestTime - TimeArray[ClientNum]));
      if ((requestTime - TimeArray[i]) > 100) {
        Serial.println("");
        Serial.print("Geräte Timeout: IP: ");
        Serial.print(IPArray[i]);
        Serial.print(" /  Type: ");
        Serial.print("Buzzer");
        for (int x = i; x < ClientNum;) {
          //Alle werte Zurückrüken
          Serial.print("point1");
          IPArray[x] = IPArray[i + 1];
          Serial.print("point2");
          TimeArray[x] = TimeArray[x + 1];
          Serial.print("point3");
          x++;
        }
        Serial.print("point4");
        ClientNum = ClientNum - 1;
        float TIME = millis();
        float TIME_ = millis();
        if ((TIME_ - TIME) >= 1000) {
          int minus = round((TIME_ - TIME) / 1000);
          requestTime = requestTime - minus;
        }
        Serial.print("point5");
      }
      i++;
    }
  }
}

//===========================================================================================

void AddDevice(String DeviceType, int DeviceID, String DeviceMAC){           //Solte ein timeout beinhalten
//Überprüft ob die ID bereits exestriert
  for (int i = 0; i < ClientNum;) {
    if (DeviceID == IPArray[i]) {
      for (int x = i; x < ClientNum;) {
        //Alle werte Zurückrüken
        IPArray[x] = IPArray[i + 1];
        TimeArray[x] = TimeArray[x + 1];
        TypeArray[x] = TypeArray[i + 1];
        MacArray[x] = MacArray[x + 1];
        x++;
      }
      ClientNum = ClientNum - 1;
    }
    i++;
  }

  Serial.println("Neuer Buzzer angemeldet");
  if(ClientNum > 0){
  ArrayDebug_function(IPArray);
  }
  if(DeviceType == "NB"){DeviceType = "B";}
  if(DeviceType == "NE"){DeviceType = "E";}
  IPArray[ClientNum] = DeviceID;
  MacArray[ClientNum] = DeviceMAC;
  TypeArray[ClientNum] = DeviceType;
  TimeArray[ClientNum] = requestTime;
  ClientNum = ClientNum + 1;
  ArrayDebug_function(IPArray);



  ArrayDebug_function(TimeArray);
  digitalWrite(addBuzzerLED, LOW);

  //GeräteTimeoutRefresh Start-------------------------------------------------------------------
    ArrayDebug_function(TimeArray);
      for(int i = 0; i < ClientNum ;){
         if(DeviceID == IPArray[i]){
          TimeArray[i] = requestTime;
         }
        i++;
        }
      ArrayDebug_function(TimeArray);
  //GeräteTimeoutRefresh Ende--------------------------------------------------------------------
  }


//===========================================================================================


void LedHandler(String LedName, String Effect){

}

//===========================================================================================


void AnalogButtonsHandler(){

}

//===========================================================================================

void DataSync(){            //Sorgt dafür das alle Geräte die Neusten Configs haben

}

//===========================================================================================

String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int ArrayDebug_function(int var[MaxClients]) {
  for (int i = 0; i < ClientNum; i++) {
    Serial.print("//");
    Serial.print(var[i]);
    Serial.print("//");
  }
}
