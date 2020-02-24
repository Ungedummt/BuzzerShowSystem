#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ssid = "ESP32-Soft-accessPoint";
const char* password = "microcontrollerslab";

WebServer server(80);

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

String html ="<!DOCTYPE html><html><head><link rel='stylesheet' type='text/css' href='style.css'></head><body><div id='body'><div id='nav-bar'><a id='game' href='%'>Game</a><a class='active' id='configuration' href=''>Configuration</a><a id='basics' href='configbasics.html'>Basics</a><a class='active' id='actions' href=''>Actions</a><a id='dmx' href='configdmx.html'>DMX</a></div><form action='/' method='POST'><div id='content'><div id='buzzerpressed'><fieldset><legend>Buzzer pressed:</legend><div id='mar'><div id='boxes' class='grid'><p>pressed Player</p><input type='color' name='pressedp' value='#0000FF'><p>other Players too late</p><input type='color' name='otherp' value='#FFFF00'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='rightanwser'><fieldset><legend>right answer:</legend><div id='mar'><div id='boxes' class='grid'><p>right pressed Player</p><input type='color' name='rpressedp' value='#00FF00'><p>right other players</p><input type='color' name='rotherp' value='#00FF00'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='wronganwser'><fieldset><legend>wrong answer:</legend><div id='mar'><div id='boxes' class='grid'><p>right pressed Player</p><input type='color' name='wpressedp' value='#FF0000'><p>right other players</p><input type='color' name='wotherp' value='#FF0000'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='gamestandby'><fieldset><legend>right answer:</legend><div id='mar'><div id='boxes' class='grid'><p>standby</p><input type='color' name='standby' value='#000000'><p>deactivated players</p><input type='color' name='rotherp' value='#FF0000'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div></div><div id='save_discard'><input id='discard' type='reset' value='Discard'><input id='save' type='submit' value='Save'></div></form></div></body></html>";

String css = "<style>*{font-family:Calibri,Ubuntu,Noto Sans;color:#000;margin:0;padding:0}#body{border-top-color:#37f;border-top-style:solid;border-top-width:5px;background:#fff;border-radius:5px;width:700px;margin:28px auto}body{background:#e1e8ff}a{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px}.grid{display:grid}.bold{font-weight:bolder}#nav-bar{display:grid;grid-template-columns:auto auto auto auto auto auto;grid-gap:10px;padding:20px 20px 0}#nav-bar a{background:#add8e6}#nav-bar .active{background:red}#nav-bar #game{grid-column-start:1;grid-column-end:4}#nav-bar #configuration{grid-column-start:4;grid-column-end:7}#nav-bar #basics{grid-column-start:-7;grid-column-end:-5}#nav-bar #actions{grid-column-start:-5;grid-column-end:-3}#nav-bar #dmx{grid-column-start:-3;grid-column-end:-1}#content{display:grid;grid-gap:10px;padding:20px 30px}#content legend{padding-left:4px;margin:0 auto;padding-right:8px;font-size:21px}#content fieldset{border-radius:5px;border-width:2px;border-color:#37f;border-style:solid}#content select{width:100%}#content p{font-size:16px}#content input[type=number]{width:80%}#content input[type=checkbox]{margin:auto;zoom:1.5;transform:scale(1.5)}#content input[type=color]{width:100%;height:100%}#content #mar{display:block;width:95%;margin:10px auto 15px}#content #mar #boxes{grid-template-columns:auto 12%;grid-gap:10px}#content #mar #boxes p{line-height:30px}#content #mar #boxes #chpos{grid-column-start:1;grid-column-end:3;text-align:center;margin-top:25px;margin-bottom:-5px;font-size:20px}#content #mar #boxes #chposgrid{grid-template-columns:auto auto auto auto auto;grid-column-start:1;grid-column-end:3;text-align:center}#content #mar #boxes #chposgrid input{height:35px}#content #testdmx input{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px;background:#ff0;margin:0 auto;width:50%;display:block}#savediscard{display:grid;grid-template-columns:auto auto;grid-gap:10px;padding:0 20px 20px}#savediscard input{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px}#savediscard #save{background:green}#savediscard #discard{background:red}</style>";

void Connect_WiFi(){
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
  }
}

bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

void handleLogin(){
  String msg;

  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.sendHeader("Set-Cookie","ESPSESSIONID=0");
    server.send(301);
  }
  else if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
    }
    else{
      msg = "Wrong username/password! try again.";
      Serial.println("Log in Failed");
      String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
      content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
      content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
      content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
      content += "You also can go <a href='/inline'>here</a></body></html>";
      server.send(200, "text/html", content);
    }
  }

  else if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if(is_authentified()){
      Serial.println("Login");
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
    }
    else{
      String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
      content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
      content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
      content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
      content += "You also can go <a href='/inline'>here</a></body></html>";
      server.send(200, "text/html", content);
    }
  }
  else{
  String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
  }
}

void handleRoot(){
  //Serial.println("Enter handleRoot");
  //String header;
  //content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  //server.send(200, "text/html", (html + css));
  //Serial.println(server.hasArg("LED0"));
  //if(server.hasArg("LED0")){
  // Serial.println(server.arg("LED0"));
  //}
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()){
    server.sendHeader("Location","/login");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
    return;
  }
  else {
    Serial.println(server.uri());
    if(server.uri() != "/"){
      server.sendHeader("Location","/");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
      return;
    }
  }

  String content = "<html><body><H2>hello, you successfully connected to esp8266/esp32!</H2><br>";

  if (server.hasHeader("User-Agent")){
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

void handleTest(){
  Serial.println("Enter handleRoot");
  String header;
  //content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", ("Hi"));
}

void setup() {
  Serial.begin(115200);
  Serial.print("Setting soft access point mode");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.on("/", handleRoot);
//server.on("/generate_204", handleRoot); //Android captive portal. Maybe not needed. Might be handled by notFound handler.
//server.on("/connectivitycheck.gstatic.com", handleRoot); //Android captive portal. Maybe not needed. Might be handled by notFound handler.
//server.on("/fwlink", handleRoot);
  server.on("/nmcheck.gnome.org", handleRoot);
  server.on("/test", handleTest);
  server.on("/login", handleLogin);
  server.onNotFound(handleRoot);

  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
}