//Includes-----------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

//Variabeln I/O------------------------------------------------------------------------------
//Buttons
int addBuzzerButton[2]{825,828};
int ResetGameButton[2]{845,846};
int wrongButton[2]{842,843};
int rightButton[2]{835,838};
int GameBegin[2]{805,806};

float Time;
boolean newBuzzer = false;

//LEDS
#define addBuzzerLED D3
#define RichtigButtonLED D0
#define SpielBeginButtonLED D4
#define ResetButtonLED D2
#define FalschButtonLED D1

//Sonstiges

//
long requestTime = 0;
String type;
String ip;
boolean finish;
boolean gameistrue;
int MasterAnswer;//0 = nichts / 1 = Richtig / 2 = Falsch
int IPArray [20] = {};
int TimeArray [20] = {};
String TypeArray [20] = {};
int rqt = 0;
float millis_;
float millisOld;

//Variabeln Network--------------------------------------------------------------------------
char* SSID = "Buzzer-AP";           // SSID
char* KEY = "6732987frkubz3458";    // password
#define CHANNEL 1
#define HIDDEN true
int max_connection = 0;
int ClientNum = 0;
String request;

WiFiServer APServerPort(80);
IPAddress IP(192, 168, 4, 1);
IPAddress mask = (255, 255, 255, 0);

IPAddress ClientServerAddress(192,168,4,100);     // IP address of the ClientServer
WiFiClient APclient;


//WiFiServer server(80);
//IPAddress IP(192, 168, 4, 1);
//WiFiServer server(80);
//IPAddress clientPort(192, 168, 4, 100); //das
//WiFiClient ClientOutput; // Client name for AP mode (AP to Client) //das
//WiFiClient client;	 // Client name for AP mode (Clinet to AP)
//IPAddress IP(192,168,4,1);
//IPAddress mask = (255, 255, 255, 0);
//===========================================================================================

void setup() {

  //pinMode(addBuzzer, INPUT);
  pinMode(addBuzzerLED, OUTPUT);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, KEY, CHANNEL, HIDDEN, max_connection);
  WiFi.softAPConfig(IP, IP, mask);
  APServerPort.begin();

  Serial.begin(9600);
  Serial.println();
  Serial.println("AP");
  Serial.println("Server started.");
  Serial.println("IP: " + WiFi.softAPIP());
  Serial.println("MAC:" + WiFi.softAPmacAddress());

  Serial.println("The arrayleght is: ");
  //Array wir dal byte gelesen und der StartWert für i muss bei 0 anfangen (Array fägt bei 0 an ([0] = "1"))
  //Quelle https://www.arduino.cc/reference/en/language/variables/utilities/sizeof/
  //for(byte i = 0; i < (sizeof(IPArray) / sizeof(IPArray[0])); i++){
   // Serial.print('/');
   // Serial.print(IPArray[i]);
   // Serial.print('/');
 // }
}
//==============================LOOP=====================================================
void loop() {
	WiFiClient client = APServerPort.available();
	if ((millis_ - millisOld) >= 1000) {
		millisOld = millis_;
		millis_ = millis();
		requestTime = requestTime + 1;
	}
	else {
		millis_ = millis();
	}
	//GeräteTimeout Start--------------------------------------------------------------------
	if (ClientNum > 0) {
		for (int i = 0; i < ClientNum;) {
			//Serial.println((requestTime - TimeArray[ClientNum]));
			if ((requestTime - TimeArray[i]) > 4) {
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
				max_connection = max_connection - 1;
				float TIME = millis();
				WiFi.softAP(SSID, KEY, CHANNEL, HIDDEN, max_connection);
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
	//GeräteTimeout Ende--------------------------------------------------------------------

	char SerRead = Serial.read();

	if (SerRead == 'A') {
		//AddClient();
		digitalWrite(addBuzzerLED, HIGH);
		max_connection++;
		WiFi.softAP(SSID, KEY, CHANNEL, HIDDEN, max_connection);
		SerRead = '0';
	}
	//if (SerRead == 'G') {
		//SendToClient(100, "Laaaall");
		//gameistrue = true;
		//SerRead = '0';
	//}
	if (SerRead == 'G') {
		 // Serial.println(client.println("Testkjguzhghjfghzjfzzfgjfzgfzgth" + '\r'));
		SendToClient(100, "test");
		  //SendToClient(100, "Laaaall");
		  //gameistrue = true;
		  SerRead = '0';
	  }

  if (!client) {return;}
    String request = client.readStringUntil('\r');
    client.flush();
    Serial.println("********************************");
    String pressed = getValue(request, ';', 2);
    String type = getValue(request, ';', 1);
    String ip = getValue(request, ';', 0);
    int ipasint = ip.toInt();
    Serial.println(pressed);
    if (pressed == "1") {
			SendToClient(ipasint, "SR");

      if(gameistrue == true){
		  Serial.print("BuzzerPressed: ");
		  Serial.println(ip);
		  Serial.println("Waiting for Answer");
		  MasterAnswer = NULL;
		  while (MasterAnswer == NULL) { //Timeout einrichten maby
			  char SerRead = Serial.read();
			  if (SerRead == 'C') {
				  MasterAnswer = 1;//0 = nichts / 1 = Richtig / 2 = Falsch
				  SerRead = '0';
			  }
			  if (SerRead == 'W') {
				  MasterAnswer = 2;//0 = nichts / 1 = Richtig / 2 = Falsch
				  SerRead = '0';
			  }
			  //delay(10);
		  }
		  //if (MasterAnswer == 1) { //Right Answer
			//  Serial.println("Right Answer");
			//  APtoEffectC.connect(APtoEffectA, 80);
			//  APtoEffectC.print("E" + ';' + 'P' + ';' + ip + '\r');
			//  Serial.println("E" + ';' + 'P' + ';' + ip + '\r'); //Fehler
		  //}
		 // if (MasterAnswer == 2) { //Wrong Answer
			//  Serial.println("Wrong Answer");


		//  }
      }
    }
	//Überprüft ob ip bereits exestriert Start--------------------------------------
	if (type == "NB" or type == "NE") {
		for (int i = 0; i < ClientNum;) {
			if (ipasint == IPArray[i]) {
				for (int x = i; x < ClientNum;) {
					//Alle werte Zurückrüken
					IPArray[x] = IPArray[i + 1];
					TimeArray[x] = TimeArray[x + 1];
					x++;
				}
				ClientNum = ClientNum - 1;
			}
			i++;
		}


//Überprüft ob ip bereits exestriert Ende---------------------------------------
      Serial.println("Neuer Buzzer angemeldet");
	  newBuzzer = true;
      Serial.println("ID: " + ip);
      Serial.println("********************************");
      Serial.println("Old Array is: ");
      if(ClientNum > 0){
		  bla_function(IPArray);
      }
      IPArray[ClientNum] = ipasint;
      //TimeArray[ClientNum] = Time;
      ClientNum = ClientNum + 1;
      Serial.print("ClientNum = ");
      Serial.println(ClientNum);
      //Serial.println(IPArray[*]);
      Serial.println("");
      Serial.println("New Array is: ");
	  bla_function(IPArray);
      TimeArray[ClientNum-1] = requestTime;
      if(type == "NB"){type = "B";}
      if(type == "NE"){type = "E";}
      TypeArray[ClientNum-1] = type;

      Serial.println("");
      Serial.println("New Time Array is: ");
	  bla_function(TimeArray);
     digitalWrite(addBuzzerLED, LOW);

    }
    Serial.println("");
    Serial.println(request + "\t/Type=" + type + "\t/IP=" + ipasint);

    Serial.println("Type Array is: ");
	bla_function(TimeArray);

//GeräteTimeoutRefresh Start-------------------------------------------------------------------
    Serial.println("");
    Serial.println("Old Time Array is: ");
	bla_function(TimeArray);
    for(int i = 0; i < ClientNum ;){
       if(ipasint == IPArray[i]){
        Serial.println("Time Beschreiben");
        TimeArray[i] = requestTime;
       }
      i++;
      }
      Serial.println("");
      Serial.println("New Time Array is: ");
	  bla_function(TimeArray);
      Serial.println("");
//GeräteTimeoutRefresh Ende--------------------------------------------------------------------
}

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

//String readData(){
  //WiFiClient APClient = APServer.available();
  //if (!APClient){
  //  APClient.connect(IP, 80);
  //  String request = APClient.readStringUntil('\r');
  //  Serial.println("From the station: " + request);
  //  APClient.flush();
  //  APClient.stop();
  //  type = getValue(request, ';', 1);
  //  if(type == "B"){
  //    Serial.println(sizeof(IPArray));
  //  }
  //}
//}

int bla_function(int var[20]) {
	for (int i = 0; i < ClientNum; i++) {
		Serial.print("//");
		Serial.print(var[i]);
		Serial.print("//");
	}
}

void SendToClient(int IP, String Message){
	IPAddress ClientServerAddress(192, 168, 4, IP);
	APclient.connect(ClientServerAddress, 88);
	APclient.print(Message + "\r");
	Serial.print("Send: ");
	Serial.println(Message + '\r');
	Serial.print(" / To: ");
	Serial.println(IP);
}

void AddClient() {
	if (ClientNum == max_connection) {
		digitalWrite(addBuzzerLED, HIGH);
		float Time = millis();
		max_connection++;
		WiFi.softAP(SSID, KEY, CHANNEL, HIDDEN, max_connection);
		Serial.println("Warte auf Buzzer");
	}

		if (ClientNum != max_connection) {  //warten für anmeldung
			Serial.print(".");
			digitalWrite(addBuzzerLED, HIGH);
			delay(100);
			digitalWrite(addBuzzerLED, LOW);
			delay(100);
			if (millis() >= Time + 100000) {
				max_connection--;
				WiFi.softAP(SSID, KEY, CHANNEL, HIDDEN, max_connection);
				newBuzzer = false;
				Serial.println("");
				Serial.println("Buzzer konnte nicht hinzgefügt werden :(");
			}
			if (newBuzzer = true) {
				Serial.println("");
				Serial.println("Buzzer Hinzugefügt");
				newBuzzer = false;
			}
		}


}
