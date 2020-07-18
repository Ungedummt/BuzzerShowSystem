#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

Preferences preferences;

const char* ssid = "ESP32-Soft-accessPoint";
const char* password = "microcontrollerslab";

WebServer server(80);

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

String Style = "<style>* { font-family: Calibri, Ubuntu, Noto Sans; color: black; margin: 0px; padding: 0px; }#body { border-top-color: #3377ff; border-top-style: solid; border-top-width: 5px; background: white; border-radius: 5px; margin-left: auto; margin-right: auto; width: 700px; }body { background: #e1e8ff; }a { font-weight: bold; border-radius: 5px; border: none; text-align: center; text-decoration: none; display: inline-block; cursor: pointer; line-height: 50px; font-size: 25px; }.grid { display: grid; }.bold { font-weight: bolder; }#nav-bar { display: grid; grid-template-columns: auto auto auto auto auto auto; grid-gap: 10px; padding-top: 20px; padding-right: 20px; padding-left: 20px; padding-bottom: 0px; } #nav-bar a { background: lightblue; } #nav-bar .active { background: red; } #nav-bar #game { grid-column-start: 1; grid-column-end: 4; } #nav-bar #configuration { grid-column-start: 4; grid-column-end: 7; } #nav-bar #basics { grid-column-start: -7; grid-column-end: -5; } #nav-bar #actions { grid-column-start: -5; grid-column-end: -3; } #nav-bar #dmx { grid-column-start: -3; grid-column-end: -1; }#content { display: grid; grid-gap: 10px; padding: 20px 30px; } #content legend { padding-left: 4px; margin: 0px auto; padding-right: 8px; font-size: 21px; } #content fieldset { border-radius: 5px; border-width: 2px; border-color: #37f; border-style: solid; } #content select { width: 100%; } #content p { font-size: 16px; } #content input[type=number] { width: 80%; } #content input[type=checkbox] { margin: auto; zoom: 1.5; transform: scale(1.5); } #content input[type=color] { width: 100%; height: 100%; } #content #mar { display: block; width: 95%; margin-left: auto; margin-right: auto; margin-top: 10px; margin-bottom: 15px; } #content #mar #boxes { grid-template-columns: auto 12%; grid-gap: 10px; } #content #mar #boxes p { line-height: 30px; } #content #mar #boxes #ch_pos { grid-column-start: 1; grid-column-end: 3; text-align: center; margin-top: 25px; margin-bottom: -5px; font-size: 20px; } #content #mar #boxes #ch_pos_grid { grid-template-columns: auto auto auto auto auto; grid-column-start: 1; grid-column-end: 3; text-align: center; } #content #mar #boxes #ch_pos_grid input { height: 35px; } #content #test_dmx input { font-weight: bold; border-radius: 5px; border: none; text-align: center; text-decoration: none; display: inline-block; cursor: pointer; line-height: 50px; font-size: 25px; background: yellow; margin: 0px auto; width: 50%; display: block; }#save_discard { display: grid; grid-template-columns: auto auto; grid-gap: 10px; padding-top: 0px; padding-right: 20px; padding-left: 20px; padding-bottom: 20px; } #save_discard input { font-weight: bold; border-radius: 5px; border: none; text-align: center; text-decoration: none; display: inline-block; cursor: pointer; line-height: 50px; font-size: 25px; } #save_discard #save { background: green; } #save_discard #discard { background: red; }</style>";

String ConfigBasics = "<!DOCTYPE html><html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head><body><h1>Grid Lines</h1><div id=\"body\"><div id=\"nav-bar\" class=\"grid\"><a id=\"game\" href=\"%\">Game</a><a class=\"active\" id=\"configuration\" href=\"\">Configuration</a><a class=\"active\" id=\"basics\" href=\"\">Basics</a><a id=\"actions\" href=\"config_actions.html\">Actions</a><a id=\"dmx\" href=\"config_dmx.html\">DMX</a></div><form action=\"config_basics.html\" method=\"POST\"><div id=\"content\" class=\"grid\"><div id=\"quiz_type\"><fieldset><legend>Quiztype:</legend><div id=\"mar\"><select name=\"quiz_type\"><option value=\"normal\" selected>NormalQuiz</option><option value=\"unnormal\">UnnormalQuiz</option></select></div></fieldset></div><div id=\"checkboxses\"><fieldset><legend>Game Rules:</legend><div id=\"mar\"><div id=\"boxes\" class=\"grid\"><p id=\"text1\">When a answer was wrong the others players are Activated by the next rese</p><input id=\"box1\" type=\"checkbox\" name=\"1\" value=\"1\"><p id=\"text2\">The too late players can see his delays on the diplay</p><input id=\"box2\" type=\"checkbox\" name=\"2\" value=\"2\"><p id=\"text3\">The players can press multiple times, with sound</p><input id=\"box3\" type=\"checkbox\" name=\"3\" value=\"3\" checked><p id=\"text4\">The too late players get the to late color</p><input id=\"box4\" type=\"checkbox\" name=\"4\" value=\"4\" checked><p id=\"text5\">Only the pressed player get the answer color</p><input id=\"box5\" type=\"checkbox\" name=\"5\" value=\"5\" checked></div></div></fieldset></div></div><div id=\"save_discard\" class=\"grid\"><input id=\"discard\" type=\"reset\" value=\"Discard\"><input id=\"save\" type=\"submit\" value=\"Save\"></div></form></div><p>You can refer to line numbers when placing grid items.</p></body></html>";

String ConfigActions = "<!DOCTYPE html><html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head><body><h1>Grid Lines</h1><div id=\"body\"><div id=\"nav-bar\"><a id=\"game\" href=\"%\">Game</a><a class=\"active\" id=\"configuration\" href=\"\">Configuration</a><a id=\"basics\" href=\"config_basics.html\">Basics</a><a class=\"active\" id=\"actions\" href=\"\">Actions</a><a id=\"dmx\" href=\"config_dmx.html\">DMX</a></div><form action=\"/\" method=\"POST\"><div id=\"content\"><div id=\"buzzer_pressed\"><fieldset><legend>Buzzer pressed:</legend><div id=\"mar\"><div id=\"boxes\" class=\"grid\"><p>pressed Player</p><input type=\"color\" name=\"pressed_p\" value=\"#0000FF\"><p>other Players too late</p><input type=\"color\" name=\"other_p\" value=\"#FFFF00\"><select name=\"quiz_type\"><option value=\"sound1\" selected>Sound1</option><option value=\"sound2\">Sound2</option></select><input type=\"submit\" name=\"play\" value=\"Play\"></div></div></fieldset></div><div id=\"right_anwser\"><fieldset><legend>right answer:</legend><div id=\"mar\"><div id=\"boxes\" class=\"grid\"><p>right pressed Player</p><input type=\"color\" name=\"r_pressed_p\" value=\"#00FF00\"><p>right other players</p><input type=\"color\" name=\"r_other_p\" value=\"#00FF00\"><select name=\"quiz_type\"><option value=\"sound1\" selected>Sound1</option><option value=\"sound2\">Sound2</option></select><input type=\"submit\" name=\"play\" value=\"Play\"></div></div></fieldset></div><div id=\"wrong_anwser\"><fieldset><legend>wrong answer:</legend><div id=\"mar\"><div id=\"boxes\" class=\"grid\"><p>right pressed Player</p><input type=\"color\" name=\"w_pressed_p\" value=\"#FF0000\"><p>right other players</p><input type=\"color\" name=\"w_other_p\" value=\"#FF0000\"><select name=\"quiz_type\"><option value=\"sound1\" selected>Sound1</option><option value=\"sound2\">Sound2</option></select><input type=\"submit\" name=\"play\" value=\"Play\"></div></div></fieldset></div><div id=\"game_standby\"><fieldset><legend>right answer:</legend><div id=\"mar\"><div id=\"boxes\" class=\"grid\"><p>standby</p><input type=\"color\" name=\"standby\" value=\"#000000\"><p>deactivated players</p><input type=\"color\" name=\"r_other_p\" value=\"#FF0000\"><select name=\"quiz_type\"><option value=\"sound1\" selected>Sound1</option><option value=\"sound2\">Sound2</option></select><input type=\"submit\" name=\"play\" value=\"Play\"></div></div></fieldset></div></div><div id=\"save_discard\"><input id=\"discard\" type=\"reset\" value=\"Discard\"><input id=\"save\" type=\"submit\" value=\"Save\"></div></form></div><p>You can refer to line numbers when placing grid items.</p></body></html>";

String ConfigDmx = "<!DOCTYPE html><html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head><body><h1>Grid Lines</h1><div id=\"body\"><div id=\"nav-bar\" class=\"grid\"><a id=\"game\" href=\"%\">Game</a><a class=\"active\" id=\"configuration\" href=\"\">Configuration</a><a id=\"basics\" href=\"config_basics.html\">Basics</a><a id=\"actions\" href=\"config_actions.html\">Actions</a><a class=\"active\" id=\"dmx\" href=\"\">DMX</a></div><form action=\"/\" method=\"POST\"><div id=\"content\" class=\"grid\"><div id=\"lights\"><fieldset><legend>Lights: </legend><div id=\"mar\"><div id=\"boxes\" class=\"grid\"><p id=\"text1\">Nuber of Lights</p><input id=\"box1\" type=\"number\" name=\"1\" value=\"1\"><p id=\"text2\">Start Channel</p><input id=\"box2\" type=\"number\" name=\"2\" value=\"2\"><p id=\"text3\">Channel per Light</p><input id=\"box3\" type=\"number\" name=\"3\" value=\"3\" checked><p id=\"ch_pos\">Channel Position</p><div id=\"ch_pos_grid\" class=\"grid\"><p class=\"bold\">B</p><p class=\"bold\">R</p><p class=\"bold\">G</p><p class=\"bold\">B</p><p class=\"bold\">W</p><input type=\"number\" name=\"B\" value=\"1\"><input type=\"number\" name=\"R\" value=\"2\"><input type=\"number\" name=\"G\" value=\"3\"><input type=\"number\" name=\"B\" value=\"4\"><input type=\"number\" name=\"W\" value=\"5\"><P>Brightness</P><P>Red</P><P>Green</P><P>Blue</P><P>White</P></div></div></div></fieldset></div><div id=\"test_dmx\"><input id=\"button\" type=\"submit\" value=\"Test DMX\"></div></div><div id=\"save_discard\" class=\"grid\"><input id=\"discard\" type=\"reset\" value=\"Discard\"><input id=\"save\" type=\"submit\" value=\"Save\"></div></form></div><p>You can refer to line numbers when placing grid items.</p></body></html>";


String html ="<!DOCTYPE html><html><head><link rel='stylesheet' type='text/css' href='style.css'></head><body><div id='body'><div id='nav-bar'><a id='game' href='%'>Game</a><a class='active' id='configuration' href=''>Configuration</a><a id='basics' href='configbasics.html'>Basics</a><a class='active' id='actions' href=''>Actions</a><a id='dmx' href='configdmx.html'>DMX</a></div><form action='/' method='POST'><div id='content'><div id='buzzerpressed'><fieldset><legend>Buzzer pressed:</legend><div id='mar'><div id='boxes' class='grid'><p>pressed Player</p><input type='color' name='pressedp' value='#0000FF'><p>other Players too late</p><input type='color' name='otherp' value='#FFFF00'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='rightanwser'><fieldset><legend>right answer:</legend><div id='mar'><div id='boxes' class='grid'><p>right pressed Player</p><input type='color' name='rpressedp' value='#00FF00'><p>right other players</p><input type='color' name='rotherp' value='#00FF00'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='wronganwser'><fieldset><legend>wrong answer:</legend><div id='mar'><div id='boxes' class='grid'><p>right pressed Player</p><input type='color' name='wpressedp' value='#FF0000'><p>right other players</p><input type='color' name='wotherp' value='#FF0000'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='gamestandby'><fieldset><legend>right answer:</legend><div id='mar'><div id='boxes' class='grid'><p>standby</p><input type='color' name='standby' value='#000000'><p>deactivated players</p><input type='color' name='rotherp' value='#FF0000'><select name='quiztype'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div></div><div id='save_discard'><input id='discard' type='reset' value='Discard'><input id='save' type='submit' value='Save'></div></form></div></body></html>";

String quiz_type = "normal";

String style = "<style>*{font-family:Ubuntu,Calibri,Noto Sans;font-size:19px;margin:0;padding:0;cursor:default;color:#000}#body{border-top-color:#37f;border-top-style:solid;border-top-width:5px;background:#fff;border-radius:5px;width:700px;margin:28px auto 78px}body{background-color:#add8e6}a{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px;color:#000}.grid{display:grid}.bold{font-weight:bolder}#nav-bar{display:grid;grid-template-columns:auto auto auto auto auto auto;grid-gap:10px;padding:20px 20px 0}#nav-bar a{background:#add8e6}#nav-bar .active{background:red}#nav-bar #game{grid-column-start:1;grid-column-end:4}#nav-bar #configuration{grid-column-start:4;grid-column-end:7}#nav-bar #basics{grid-column-start:-7;grid-column-end:-5}#nav-bar #actions{grid-column-start:-5;grid-column-end:-3}#nav-bar #dmx{grid-column-start:-3;grid-column-end:-1}#content{display:grid;grid-gap:10px;padding:20px 30px}#content legend{padding-left:4px;margin:0 auto;padding-right:8px;font-size:21px}#content fieldset{border-radius:5px;border-width:2px;border-color:#37f;border-style:solid}#content select{width:100%}#content p{font-size:16px}#content input[type=number]{width:80%}#content input[type=checkbox]{margin:auto;zoom:1.5;transform:scale(1.5)}#content input[type=color]{width:100%;height:100%}#content #mar{display:block;width:95%;margin:10px auto 15px}#content #mar #boxes{grid-template-columns:auto 12%;grid-gap:10px}#content #mar #boxes p{line-height:30px}#content #mar #boxes #ch_pos{grid-column-start:1;grid-column-end:3;text-align:center;margin-top:25px;margin-bottom:-5px;font-size:20px}#content #mar #boxes #ch_pos_grid{grid-template-columns:auto auto auto auto auto;grid-column-start:1;grid-column-end:3;text-align:center}#content #mar #boxes #ch_pos_grid input{height:35px}#content #test_dmx input{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px;background:#ff0;margin:0 auto;width:50%;display:block}#save_discard{display:grid;grid-template-columns:auto auto;grid-gap:10px;padding:0 20px 20px}#save_discard input{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px}#save_discard #save{background:green}#save_discard #discard{background:red}.login-block{width:340px;padding:20px;background:#fff;border-radius:5px;border-top:5px solid #37f;margin:0 auto;margin-top:50px}.login-block input,.login-block button{width:100%;height:50px;box-sizing:border-box;border-radius:5px;font-size:16px;outline:none}.login-block h1{text-align:center;color:#000;font-size:22px;text-transform:uppercase;margin-top:2px;margin-bottom:25px}.login-block input{border:1px solid #ccc;margin-bottom:20px;padding:0 20px 0 15px;cursor:text}.login-block input:active,.login-block input:focus{border:1px solid #37f}.login-block button{border:0;background:#37f;color:#fff;font-weight:700;text-transform:uppercase;cursor:pointer;background:#37f}.login-block button:hover{background:#6397ff}.main-block{width:98%;background:#fff;border-radius:5px;margin:0 auto;margin-bottom:60px}.footer{position:fixed;bottom:0;width:100%;background-color:#ff0;display:inline-block}.footer p{margin-right:15px;text-align:right;line-height:50px;width:auto;float:right;cursor:help}.footer img{float:right;padding:12.5px;height:25px;cursor:help}</style>";

String header_config = "<head><meta http-equiv='content-type' content='text/html; charset=UTF-8'><meta charset='utf-8'><title>Configuration</title><link rel='stylesheet' type='text/css' href='style.css'></head>";
String header_login = "<head><meta http-equiv='content-type' content='text/html; charset=UTF-8'><meta charset='utf-8'><title>Login</title><link rel='stylesheet' type='text/css' href='style.css'></head>";

String nav_bar_basics = "<div id='nav-bar' class='grid'><a id='game' href='Login.html'>Game</a><a id='configuration' href='' class='active'>Configuration</a><a id='basics' class='active' >Basics</a><a id='actions' href='/actions'>Actions</a><a id='dmx' href='/dmx' >DMX</a></div>";
String nav_bar_actions = "<div id='nav-bar' class='grid'><a id='game' href='Login.html'>Game</a><a id='configuration' href='' class='active'>Configuration</a><a id='basics' href='/basics'>Basics</a><a id='actions' class='active'>Actions</a><a id='dmx' href='/dmx'>DMX</a></div>";
String nav_bar_dmx = "<div id='nav-bar' class='grid'><a id='game' href='Login.html'>Game</a><a id='configuration' href='' class='active'>Configuration</a><a id='basics' href='/basics'>Basics</a><a id='actions' href='/actions'>Actions</a><a id='dmx' class='active'>DMX</a></div>";

String footer = "<div class='footer'><p>Version: Alpha 1.1</p></div>";
String save_discard = "<div id='save_discard' class='grid'><input id='discard' type='reset' value='Discard'><input id='save' type='submit' value='Save'></div>";
String head = "<!DOCTYPE html><html lang='en'>";

String basics = "<div id='content' class='grid'><div id='quiz_type'><fieldset><legend>Quiztype:</legend><div id='mar'><select name='quiz_type'><option value='normal' selected>NormalQuiz</option><option value='unnormal'>UnnormalQuiz</option></select></div></fieldset></div><div id='checkboxses'><fieldset><legend>Game Rules:</legend><div id='mar'><div id='boxes' class='grid'><p id='text1'>When a answer was wrong the others players are Activated by the next rese</p><input id='box1' type='checkbox' name='1' value='1'><p id='text2'>The too late players can see his delays on the diplay</p><input id='box2' type='checkbox' name='2' value='2'><p id='text3'>The players can press multiple times, with sound</p><input id='box3' type='checkbox' name='3' value='3' checked><p id='text4'>The too late players get the to late color</p><input id='box4' type='checkbox' name='4' value='4' checked><p id='text5'>Only the pressed player get the answer color</p><input id='box5' type='checkbox' name='5' value='5' checked></div></div></fieldset></div></div>";

String actions = "<div id='content'><div id='buzzer_pressed'><fieldset><legend>Buzzer pressed:</legend><div id='mar'><div id='boxes' class='grid'><p>pressed Player</p><input type='color' name='pressed_p' value='#0000FF'><p>other Players too late</p><input type='color' name='other_p' value='#FFFF00'><select name='quiz_type'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='right_anwser'><fieldset><legend>right answer:</legend><div id='mar'><div id='boxes' class='grid'><p>right pressed Player</p><input type='color' name='r_pressed_p' value='#00FF00'><p>right other players</p><input type='color' name='r_other_p' value='#00FF00'><select name='quiz_type'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='wrong_anwser'><fieldset><legend>wrong answer:</legend><div id='mar'><div id='boxes' class='grid'><p>right pressed Player</p><input type='color' name='w_pressed_p' value='#FF0000'><p>right other players</p><input type='color' name='w_other_p' value='#FF0000'><select name='quiz_type'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div><div id='game_standby'><fieldset><legend>right answer:</legend><div id='mar'><div id='boxes' class='grid'><p>standby</p><input type='color' name='standby' value='#000000'><p>deactivated players</p><input type='color' name='r_other_p' value='#FF0000'><select name='quiz_type'><option value='sound1' selected>Sound1</option><option value='sound2'>Sound2</option></select><input type='submit' name='play' value='Play'></div></div></fieldset></div></div>";

String dmx = "<div id='content' class='grid'><div id='lights'><fieldset><legend>Lights: </legend><div id='mar'><div id='boxes' class='grid'><p id='text1'>Number of Lights</p><input id='box1' type='number' name='1' value='1'><p id='text2'>Start Channel</p><input id='box2' type='number' name='2' value='2'><p id='text3'>Channel per Light</p><input id='box3' type='number' name='3' value='3' checked><p id='ch_pos'>Channel Position</p><div id='ch_pos_grid' class='grid'><p class='bold'>B</p><p class='bold'>R</p><p class='bold'>G</p><p class='bold'>B</p><p class='bold'>W</p><input type='number' name='B' value='1'><input type='number' name='R' value='2'><input type='number' name='G' value='3'><input type='number' name='B' value='4'><input type='number' name='W' value='5'><P>Brightness</P><P>Red</P><P>Green</P><P>Blue</P><P>White</P></div></div></div></fieldset></div><div id='test_dmx'><input id='button' type='submit' value='Test DMX'></div></div>";

String login = "<div class='login-block'><form action='/login' method='POST'> <h1>BuzzerShowSystem</h1> <input type='text' placeholder='Username' name='USERNAME' required=''> <input type='password' value='' placeholder='Password' name='PASSWORD' required=''> <button>Login</button></form></div>";

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
    }else{
      Serial.println("Authentification Failed");
      return false;
    }
  
  }
}

void updateHtml(){
  Serial.println(quiz_type);
  if(quiz_type == "normal"){
    basics = "<div id='content' class='grid'><div id='quiz_type'><fieldset><legend>Quiztype:</legend><div id='mar'><select name='quiz_type'><option value='normal' selected>NormalQuiz</option><option value='unnormal'>UnnormalQuiz</option></select></div></fieldset></div><div id='checkboxses'><fieldset><legend>Game Rules:</legend><div id='mar'><div id='boxes' class='grid'><p id='text1'>When a answer was wrong the others players are Activated by the next rese</p><input id='box1' type='checkbox' name='1' value='1'><p id='text2'>The too late players can see his delays on the diplay</p><input id='box2' type='checkbox' name='2' value='2'><p id='text3'>The players can press multiple times, with sound</p><input id='box3' type='checkbox' name='3' value='3' checked><p id='text4'>The too late players get the to late color</p><input id='box4' type='checkbox' name='4' value='4' checked><p id='text5'>Only the pressed player get the answer color</p><input id='box5' type='checkbox' name='5' value='5' checked></div></div></fieldset></div></div>";
  }
  else if(quiz_type == "unnormal"){
    basics = "<div id='content' class='grid'><div id='quiz_type'><fieldset><legend>Quiztype:</legend><div id='mar'><select name='quiz_type'><option value='normal'>NormalQuiz</option><option value='unnormal' selected>UnnormalQuiz</option></select></div></fieldset></div><div id='checkboxses'><fieldset><legend>Game Rules:</legend><div id='mar'><div id='boxes' class='grid'><p id='text1'>When a answer was wrong the others players are Activated by the next rese</p><input id='box1' type='checkbox' name='1' value='1'><p id='text2'>The too late players can see his delays on the diplay</p><input id='box2' type='checkbox' name='2' value='2'><p id='text3'>The players can press multiple times, with sound</p><input id='box3' type='checkbox' name='3' value='3' checked><p id='text4'>The too late players get the to late color</p><input id='box4' type='checkbox' name='4' value='4' checked><p id='text5'>Only the pressed player get the answer color</p><input id='box5' type='checkbox' name='5' value='5' checked></div></div></fieldset></div></div>";
  }
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
      server.send(200, "text/html", (head + header_login + "<body>" + login + msg + footer + "</body>" + style + "</html>"));
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
      server.send(200, "text/html", (head + header_login + "<body>" + login + msg + footer + "</body>" + style + "</html>"));
    }
  }
  else{
    server.send(200, "text/html", (head + header_login + "<body>" + login + msg + footer + "</body>" + style + "</html>"));
  }
}

void handleRoot(){
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

  server.send(200, "text/html", (head + header_config + "<body><div id='body'>" + nav_bar_basics + "<form action='/' method='POST'>" + basics + save_discard + "</form></div>" + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>"));
}

void handleBasics(){
  if(server.hasArg("quiz_type")){
    quiz_type = server.arg("quiz_type");
    updateHtml();
    //server.send(200, "text/html", quiz_type);
  }
  server.send(200, "text/html", (head + header_config + "<body><div id='body'>" + nav_bar_basics + "<form action='/basics' method='POST'>" + basics + save_discard + "</form></div>" + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>"));
}

void handleDMX(){
  server.send(200, "text/html", (head + header_config + "<body><div id='body'>" + nav_bar_dmx + "<form action='/dmx' method='POST'>" + dmx + save_discard + "</form></div>" + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>"));
}

void handleActions(){
  server.send(200, "text/html", (head + header_config + "<body><div id='body'>" + nav_bar_actions + "<form action='/actions' method='POST'>" + actions + save_discard + "</form></div>" + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>"));
}

void setup() {

  preferences.begin("my-app",false);
  preferences.putString("QuizType","lalalala");
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
  server.on("/login", handleLogin);
  server.on("/basics", handleBasics);
  server.on("/actions", handleActions);
  server.on("/dmx", handleDMX);
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