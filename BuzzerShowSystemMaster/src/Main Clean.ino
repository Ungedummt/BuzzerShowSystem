#include <ESP8266WiFi.h>

//Variabeln I/O------------------------------------------------------------------------------

//Short Codes to Buzzers and Effects
//Buzzer Accepted == BA
//Buzzer Dennied  == BD
//Game Begin      == GB
//Game Stop       == GS
//Game Reset      == GR
//Answer Right    == AR
//Answer Wrong    == AW

//Short Codes from Buzzers
//New Buzzer      == NB
//Status Message  == SM
//Buzzer Pressed  == BP

//Short Codes from Effect
//New Effect      == NE
//Status Message  == SM


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
String AcceptedMacsArray [MaxClients] = {"3C:71:BF:3A:0F:93","3C:71:BF:39:B2:78","DC:4F:22:60:6E:0B"};
String CurrentGameMacsArray [MaxClients] = {"3C:71:BF:3A:0F:93","3C:71:BF:39:B2:78","DC:4F:22:60:6E:0B"};


//Sonstiges
long requestTime = 0;
String type;
String ip;
boolean finish;
boolean GameState;
int MasterAnswer; //0 = nichts / 1 = Richtig / 2 = Falsch
float millis_;
float millisOld;
float Time;
String request;
int BuzzerPressed = 0;

//Network Variabeln--------------------------------------------------------------------------
char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password
#define CHANNEL 1
#define HIDDEN true
int max_connection = MaxClients;
int ClientNum = 0;
int DispClientNum = 0; //Dies ist die Tatsächliche anzahl an verbundenen Geräten

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
  String Message = WiFi_Read();
  if(Message != ""){
    Serial.println(Message);
  }




//Nachricht in einzelne Variabeln aufteilen
  String DeviceMAC = getValue(Message, ';', 2);
  String DeviceID = getValue(Message, ';', 1);
  String MessageType = getValue(Message, ';', 0);
  int IDasInt = DeviceID.toInt();

  if(BuzzerPressed == 0){

   if(MessageType == "BP"){
    BuzzerPressed = 1;
    WiFi_Write("PA", IDasInt);
        }
        }

//Nachricht Verwerten
  if(MessageType == "NB" || MessageType == "NE"){
      //Überprüft ob die Mac Zu den zugelassenen Macs gehört
      for (int i = 0; i < MaxClients;) {
        if (DeviceMAC == AcceptedMacsArray[i]) {
          if(GameState == 0){
            WiFi_Write("AC", IDasInt);//This Code Accepts the Buzzer and requests his messages
            AddDevice(MessageType, IDasInt, DeviceMAC);
          }
          for (int E = 0; E < ClientNum;) {
          if (IDasInt == IPArray[E] && GameState == 1) {
          WiFi_Write("AC", IDasInt);//This Code Accepts the Buzzer and requests his messages
          WiFi_Write("GB", IDasInt);
          AddDevice(MessageType, IDasInt, DeviceMAC);
          }
          E++;
          }

          i = MaxClients;
        }
        i++;
      }
  }
//Timeout von Client erneuern wenn er die Status Message (SM) Sendet
  if(MessageType == "SM"){
      for(int i = 0; i < ClientNum ;){
         if(IDasInt == IPArray[i]){
          TimeArray[i] = requestTime;
         }
        i++;
        }
        }
      if(GameState == 1){
        for(int i = 0; i < ClientNum ;){
         if(TypeArray[i] == "DB"){
          TimeArray[i] = requestTime;
         }
        i++;
        }
       }

  char SerRead = Serial.read();
   if (SerRead == 'G') {
    VIP_WiFi_Write("GB");
    GameState = 1;
  }
  if (SerRead == 'S') {
    VIP_WiFi_Write("GS");
    GameState = 0;
  }
  if (SerRead == 'R') {
    VIP_WiFi_Write("AR");
  }
  if (SerRead == 'W') {
    VIP_WiFi_Write("AW");
  }
  if (SerRead == 'Z') {
    VIP_WiFi_Write("GR");
    BuzzerPressed = 0;
  }



  SerRead = NULL;


DeviceHandler();
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

void VIP_WiFi_Write(String Message){
  String CurrentDeviceMAC;
  int CurrentID;
  for (int CM = 0; CM < ClientNum;) {
    CurrentDeviceMAC = AcceptedMacsArray[CM];
    if(TypeArray[CM] != "DB"){
       for (int i = 0; i < MaxClients;) {
        if (CurrentDeviceMAC == AcceptedMacsArray[i]) {
          CurrentID = (IPArray[CM]);
          WiFi_Write(Message, CurrentID);//This Code Accepts the Buzzer and requests his messages
          i = MaxClients;
        }
        i++;
      }
    }
      CM++;
  }
}

//===========================================================================================

void DeviceHandler(){       //Sorgt dafür das sich Geräte Verbinden und Vebunden Bleiben (TimeOut)
  //GeräteTimeout Start--------------------------------------------------------------------
  if (ClientNum > 0) {
    for (int i = 0; i < ClientNum;) {
      //Serial.println((requestTime - TimeArray[ClientNum]));
      if ((requestTime - TimeArray[i]) > 5) {
        Serial.println("");
        Serial.print("Geräte Timeout: IP: ");
        Serial.print(IPArray[i]);
        Serial.print(" /  Type: ");
        Serial.println("Buzzer");
        if(GameState == 0){
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
        if(GameState == 1){
        TypeArray[i] = "DB";
        }
        if(DispClientNum > 0){
        DispClientNum = DispClientNum - 1;
        }
        Serial.print("ClientNum");
        Serial.println(ClientNum);
        Serial.print("DispClientNum");
        Serial.println(DispClientNum);
        float TIME = millis();
        float TIME_ = millis();
        if ((TIME_ - TIME) >= 1000) {
          int minus = round((TIME_ - TIME) / 1000);
          requestTime = requestTime - minus;
        }
      }
      i++;
    }
  }

}

//===========================================================================================

void AddDevice(String DeviceType, int DeviceID, String DeviceMAC){           //Solte ein timeout beinhalten
  int ExtistingDevicePosition;
  for (int E = 0; E <= ClientNum;) {
    if (DeviceID == IPArray[E] || GameState == 0) {
//Überprüft ob die ID bereits exestriert
  for (int i = 0; i < ClientNum;) {
    if (DeviceID == IPArray[i]) {
      ExtistingDevicePosition = i;
      if(GameState == 0){
      for (int x = i; x < ClientNum;) {
        //Alle werte Zurückrüken
        IPArray[x] = IPArray[i + 1];
        TimeArray[x] = TimeArray[x + 1];
        TypeArray[x] = TypeArray[i + 1];
        MacArray[x] = MacArray[x + 1];

        x++;
      }
      ClientNum = ClientNum - 1;
      DispClientNum = DispClientNum - 1; //Könnte hier falsch sein
      }
        Serial.print("ClientNum");
        Serial.println(ClientNum);
        Serial.print("DislpClientNum");
        Serial.println(DispClientNum);

  }
  i++;
  }

  Serial.println("Neuer Buzzer angemeldet");
  if(ClientNum > 0){
  ArrayDebug_function(IPArray);
  }
  if(DeviceType == "NB"){DeviceType = "B";}
  if(DeviceType == "NE"){DeviceType = "E";}
  if(GameState == 0){
  IPArray[ClientNum] = DeviceID;
  MacArray[ClientNum] = DeviceMAC;
  TypeArray[ClientNum] = DeviceType;
  TimeArray[ClientNum] = requestTime;
  }
  if(GameState == 1){
  IPArray[ExtistingDevicePosition] = DeviceID;
  MacArray[ExtistingDevicePosition] = DeviceMAC;
  TypeArray[ExtistingDevicePosition] = DeviceType;
  TimeArray[ExtistingDevicePosition] = requestTime;
  }
  if(GameState == 0){
  ClientNum = ClientNum + 1;
  }
  DispClientNum = DispClientNum + 1;
  Serial.print("ClientNum");
        Serial.println(ClientNum);
        Serial.print("DislpClientNum");
        Serial.println(DispClientNum);
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
  ++E;
  }
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
