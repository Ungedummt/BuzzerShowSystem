#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>

//codename: Julie

Preferences preferences;

const char* ssid = "ESP32-Soft-accessPoint";
const char* password = "microcontrollerslab";

WebServer server(80);

const byte DNS_PORT = 53;
DNSServer dnsServer;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

String head = "<!DOCTYPE html><html lang='en'>";

String main_header =  "<head><meta http-equiv='content-type' content='text/html; charset=UTF-8'><meta charset='utf-8'><title>Configuration</title></head>";
String login_header = "<head><meta http-equiv='content-type' content='text/html; charset=UTF-8'><meta charset='utf-8'><title>Login</title></head>";

String new_actions =  "<body><div id='body'><div id='nav-bar' class='grid'><a id='game' href=''>Game</a><a class='active' id='configuration' href=''>Configuration</a><a id='basics' href='/basics'>Basics</a> <a class='active' id='actions' href=''>Actions</a> <a id='dmx' href='/dmx'>DMX</a> </div> <form action='/save' method='POST'> <div id='content'> <div id='buzzer_pressed'> <fieldset> <legend>Buzzer pressed:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Pressed Player</p> <input type='color' name='PressedPlayerColor' value='#0000FF'> <p>Other Players too late</p> <input type='color' name='OtherPlayersColor' value='#FFFF00'> <select name='BuzzerPressedSound'> <option value='Hard' selected>Hard</option> <option value='Medium'>Medium</option> <option value='Soft'>Soft</option> </select> <input type='submit' name='PlayBuzzerPressedSound' value='Play'> </div> </div> </fieldset> </div> <div id='right_anwser'> <fieldset> <legend>Right Answer:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Right pressed Player</p> <input type='color' name='RightPressePlayerColor' value='#00FF00'> <p>Right other Players</p> <input type='color' name='RightOtherPlayersColor' value='#00FF00'> <select name='RightAnswerSound'> <option value='Hard' selected>Hard</option> <option value='Medium'>Medium</option> <option value='Soft'>Soft</option> </select> <input type='submit' name='PlayRightAnswerSound' value='Play'> </div> </div> </fieldset> </div> <div id='wrong_anwser'> <fieldset> <legend>Wrong Answer:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Right pressed Player</p> <input type='color' name='WrongPressedPlayersColor' value='#FF0000'> <p>Right other Players</p> <input type='color' name='WrongOtherPlayersColor' value='#FF0000'> <select name='WrongAnswerSound'> <option value='Hard' selected>Hard</option> <option value='Medium'>Medium</option> <option value='Soft'>Soft</option> </select> <input type='submit' name='PlayWrongAnswerSound' value='Play'> </div> </div> </fieldset> </div> <div id='game_standby'> <fieldset> <legend>Standby:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Standby</p> <input type='color' name='StandbyColor' value='#000000'> <p>Deactivated Players</p> <input type='color' name='DeactivatedPlayersColor' value='#FF0000'> <select name='StandbyMusic'> <option value='WWM' selected>WWM</option> <option value='2'>Sound 2</option> <option value='3'>Sound 3</option> <option value='4'>Sound 4</option> <option value='5'>Sound 5</option> </select> <input type='submit' name='PlayStandbyMusic' value='Play'> </div> </div> </fieldset> </div> </div> <div id='save_discard'> <input id='discard' type='reset' value='Discard'> <input id='save' type='submit' value='Save'> </div> </form> </div> <script> const BuzzerPressed = {'PressedPlayerColor':'#0000FF', 'OtherPlayersColor':'#FFFF00', 'BuzzerPressedSound':'Hard'}; const RightAnswer = {'RightPressePlayerColor':'#00FF00', 'RightOtherPlayersColor':'#00FF00', 'RightAnswerSound':'Hard'}; const WrongAnswer = {'WrongPressedPlayersColor':'#FF0000', 'WrongOtherPlayersColor':'#FF0000', 'WrongAnswerSound':'Hard'}; const Standby = {'StandbyColor':'#000000', 'DeactivatedPlayersColor':'#FF0000', 'StandbyMusic':'WWM'}; function ChangeValues (values) { for (i in values) { document.getElementsByName(i)[0].value = values[i]; } } ChangeValues(BuzzerPressed); ChangeValues(RightAnswer); ChangeValues(WrongAnswer); ChangeValues(Standby);</script>";

String new_basics =   "<body> <div id='body'> <div id='nav-bar' class='grid'><a id='game' href=''>Game</a> <a class='active' id='configuration' href=''>Configuration</a> <a class='active' id='basics' href=''>Basics</a> <a id='actions' href='/actions'>Actions</a> <a id='dmx' href='/dmx'>DMX</a> </div> <form action='/save' method='POST'> <div id='content' class='grid'> <div id='quiz_type'> <fieldset> <legend>Quiztype:</legend> <div id='mar'> <select name='quiz_type'> <option value='normal' selected>NormalQuiz</option> <option value='unnormal'>UnnormalQuiz</option> </select> </div> </fieldset> </div> <div id='checkboxses'> <fieldset> <legend>Game Rules:</legend> <div id='mar'> <div id='boxes' class='grid'> <p id='text1'>When a answer was wrong the others players are Activated by the next reset</p> <input id='box1' type='checkbox' name='PlayerWrongNextPlayserbyReset' value='1'> <p id='text2'>The too late players can see his delays on the diplay</p> <input id='box2' type='checkbox' name='PlayersCanSeeDelays' value='1'> <p id='text3'>The players can press multiple times, with sound(Not recomended)</p> <input id='box3' type='checkbox' name='PlayersPressMultipleTimes' value='1' checked> <p id='text4'>The too late players get the to late color</p> <input id='box4' type='checkbox' name='ToLatePlayersGetToLateColor' value='1' checked> <p id='text5'>Only the pressed player get the answer color</p> <input id='box5' type='checkbox' name='OnlyPressedPlayerGetAnswerColor' value='1' checked> </div> </div> </fieldset> </div> </div> <div id='save_discard' class='grid'> <input id='discard' type='reset' value='Discard'> <input id='save' type='submit' value='Save'> </div> </form> </div> <script> const QuizType = 'normal'; const GameRules = {'box1':false, 'box2':false, 'box3':true, 'box4':true , 'box5':true}; document.getElementsByName('quiz_type')[0].value = QuizType; for (i in GameRules) { document.getElementById(i).checked = GameRules[i];}</script>";

String new_dmx =      "<body> <div id='body'> <div id='nav-bar' class='grid'><a id='game' href=''>Game</a> <a class='active' id='configuration' href=''>Configuration</a> <a id='basics' href='/basics'>Basics</a> <a id='actions' href='/actions'>Actions</a> <a class='active' id='dmx' href=''>DMX</a> </div> <form action='/save' method='POST'> <div id='content' class='grid'> <div id='lights'> <fieldset> <legend>Lights: </legend> <div id='mar'> <div id='boxes' class='grid'> <p id='text1'>Number of Lights</p> <input id='box1' type='number' name='NumberOfLights' value='1'> <p id='text2'>Start Channel</p> <input id='box2' type='number' name='StartChannel' value='2'> <p id='text3'>Channel per Light</p> <input id='box3' type='number' name='ChannelPerLight' value='3'> <p id='ch_pos'>Channel Position</p> <div id='ch_pos_grid' class='grid'> <p class='bold'>B</p> <p class='bold'>R</p> <p class='bold'>G</p> <p class='bold'>B</p> <p class='bold'>W</p> <input type='number' name='Brightness' value='1'> <input type='number' name='Red' value='2'> <input type='number' name='Green' value='3'> <input type='number' name='Blue' value='4'> <input type='number' name='White' value='5'> <P>Brightness</P> <P>Red</P> <P>Green</P> <P>Blue</P> <P>White</P> </div> </div> </div> </fieldset> </div> <div id='test_dmx'> <input id='button' type='submit' value='Test DMX'> </div> </div> <div id='save_discard' class='grid'> <input id='discard' type='reset' value='Discard'> <input id='save' type='submit' value='Save'> </div> </form> </div> <script> const Lights = {'NumberOfLights':'1', 'StartChannel':'2', 'ChannelPerLight':'3'}; const ChannelPosition = {'Brightness':'1', 'Red':'2', 'Green':'3', 'Blue':'4', 'White':'5'}; function ChangeValues (values) { for (i in values) { document.getElementsByName(i)[0].value = values[i]; } } ChangeValues(Lights); ChangeValues(ChannelPosition);</script>";

String style = "<style>*{font-family:Ubuntu,Calibri,Noto Sans;font-size:19px;margin:0;padding:0;cursor:default;color:#000}#body{border-top-color:#37f;border-top-style:solid;border-top-width:5px;background:#fff;border-radius:5px;width:700px;margin:28px auto 78px}body{background-color:#add8e6}a{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px;color:#000}.grid{display:grid}.bold{font-weight:bolder}#nav-bar{display:grid;grid-template-columns:auto auto auto auto auto auto;grid-gap:10px;padding:20px 20px 0}#nav-bar a{background:#add8e6}#nav-bar .active{background:red}#nav-bar #game{grid-column-start:1;grid-column-end:4}#nav-bar #configuration{grid-column-start:4;grid-column-end:7}#nav-bar #basics{grid-column-start:-7;grid-column-end:-5}#nav-bar #actions{grid-column-start:-5;grid-column-end:-3}#nav-bar #dmx{grid-column-start:-3;grid-column-end:-1}#content{display:grid;grid-gap:10px;padding:20px 30px}#content legend{padding-left:4px;margin:0 auto;padding-right:8px;font-size:21px}#content fieldset{border-radius:5px;border-width:2px;border-color:#37f;border-style:solid}#content select{width:100%}#content p{font-size:16px}#content input[type=number]{width:80%}#content input[type=checkbox]{margin:auto;zoom:1.5;transform:scale(1.5)}#content input[type=color]{width:100%;height:100%}#content #mar{display:block;width:95%;margin:10px auto 15px}#content #mar #boxes{grid-template-columns:auto 12%;grid-gap:10px}#content #mar #boxes p{line-height:30px}#content #mar #boxes #ch_pos{grid-column-start:1;grid-column-end:3;text-align:center;margin-top:25px;margin-bottom:-5px;font-size:20px}#content #mar #boxes #ch_pos_grid{grid-template-columns:auto auto auto auto auto;grid-column-start:1;grid-column-end:3;text-align:center}#content #mar #boxes #ch_pos_grid input{height:35px}#content #test_dmx input{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px;background:#ff0;margin:0 auto;width:50%;display:block}#save_discard{display:grid;grid-template-columns:auto auto;grid-gap:10px;padding:0 20px 20px}#save_discard input{font-weight:700;border-radius:5px;border:none;text-align:center;text-decoration:none;display:inline-block;cursor:pointer;line-height:50px;font-size:25px}#save_discard #save{background:green}#save_discard #discard{background:red}.login-block{width:340px;padding:20px;background:#fff;border-radius:5px;border-top:5px solid #37f;margin:0 auto;margin-top:50px}.login-block input,.login-block button{width:100%;height:50px;box-sizing:border-box;border-radius:5px;font-size:16px;outline:none}.login-block h1{text-align:center;color:#000;font-size:22px;text-transform:uppercase;margin-top:2px;margin-bottom:25px}.login-block input{border:1px solid #ccc;margin-bottom:20px;padding:0 20px 0 15px;cursor:text}.login-block input:active,.login-block input:focus{border:1px solid #37f}.login-block button{border:0;background:#37f;color:#fff;font-weight:700;text-transform:uppercase;cursor:pointer;background:#37f}.login-block button:hover{background:#6397ff}.main-block{width:98%;background:#fff;border-radius:5px;margin:0 auto;margin-bottom:60px}.footer{position:fixed;bottom:0;width:100%;background-color:#ff0;display:inline-block}.footer p{margin-right:15px;text-align:right;line-height:50px;width:auto;float:right;cursor:help}.footer img{float:right;padding:12.5px;height:25px;cursor:help}</style>";

String footer = "<div class='footer'><p>Version: Alpha 1.1</p></div>";

String login = "<div class='login-block'><form action='/login' method='POST'> <h1>BuzzerShowSystem</h1> <input type='text' placeholder='Username' name='USERNAME' required=''> <input type='password' value='' placeholder='Password' name='PASSWORD' required=''> <button>Login</button></form></div>";

String BoolToString(bool BoolInString){
  return BoolInString ? "true" : "false";
}

void Connect_WiFi(){
  WiFi.begin(ssid, password);
  MDNS.begin("BSS");
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
      server.sendHeader("Location","/basics");
      server.sendHeader("Cache-Control","no-cache");
      server.sendHeader("Set-Cookie","ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
    }
    else{
      msg = "Wrong username/password! try again.";
      Serial.println("Log in Failed");
      server.send(200, "text/html", (head + login_header + "<body>" + login + msg + footer + "</body>" + style + "</html>"));
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
      server.send(200, "text/html", (head + login_header + "<body>" + login + msg + footer + "</body>" + style + "</html>"));
    }
  }
  else{
    server.send(200, "text/html", (head + login_header + "<body>" + login + msg + footer + "</body>" + style + "</html>"));
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
      server.sendHeader("Location","/basics");
      server.sendHeader("Cache-Control","no-cache");
      server.send(301);
      return;
    }
  }
}

void handleBasics(){
  server.send(200, "text/html", head + main_header + new_basics + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>");
}

void handleDMX(){
  server.send(200, "text/html", head + main_header + new_dmx + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>");
}

void handleActions(){
  server.send(200, "text/html", head + main_header + new_actions + "<a href='/login?DISCONNECT=YES'>disconnect</a>" + footer + "</body>" + style + "</html>");
}

void updateBasics(){
  new_basics = "<body> <div id='body'> <div id='nav-bar' class='grid'><a id='game' href=''>Game</a> <a class='active' id='configuration' href=''>Configuration</a> <a class='active' id='basics' href=''>Basics</a> <a id='actions' href='/actions'>Actions</a> <a id='dmx' href='/dmx'>DMX</a> </div> <form action='/save' method='POST'> <div id='content' class='grid'> <div id='quiz_type'> <fieldset> <legend>Quiztype:</legend> <div id='mar'> <select name='quiz_type'> <option value='normal' selected>NormalQuiz</option> <option value='unnormal'>UnnormalQuiz</option> </select> </div> </fieldset> </div> <div id='checkboxses'> <fieldset> <legend>Game Rules:</legend> <div id='mar'> <div id='boxes' class='grid'> <p id='text1'>When a answer was wrong the others players are Activated by the next reset</p> <input id='box1' type='checkbox' name='PlayerWrongNextPlayserbyReset' value='1'> <p id='text2'>The too late players can see his delays on the diplay</p> <input id='box2' type='checkbox' name='PlayersCanSeeDelays' value='2'> <p id='text3'>The players can press multiple times, with sound(Not recomended)</p> <input id='box3' type='checkbox' name='PlayersPressMultipleTimes' value='3' checked> <p id='text4'>The too late players get the to late color</p> <input id='box4' type='checkbox' name='ToLatePlayersGetToLateColor' value='4' checked> <p id='text5'>Only the pressed player get the answer color</p> <input id='box5' type='checkbox' name='OnlyPressedPlayerGetAnswerColor' value='5' checked> </div> </div> </fieldset> </div> </div> <div id='save_discard' class='grid'> <input id='discard' type='reset' value='Discard'> <input id='save' type='submit' value='Save'> </div> </form> </div> <script> const QuizType = '" + preferences.getString("quiz_type") + "'; const GameRules = {'box1':" + preferences.getString("PWNPR") + ", 'box2':" + preferences.getString("PCSD") + ", 'box3':" + preferences.getString("PPMT") + ", 'box4':" + preferences.getString("TLPGTLC") + " , 'box5':" + preferences.getString("OPPGAC") + "}; document.getElementsByName('quiz_type')[0].value = QuizType; for (i in GameRules) { document.getElementById(i).checked = GameRules[i];}</script>";
}

void updateActions(){
  new_actions = "<body><div id='body'><div id='nav-bar' class='grid'><a id='game' href=''>Game</a><a class='active' id='configuration' href=''>Configuration</a><a id='basics' href='/basics'>Basics</a> <a class='active' id='actions' href=''>Actions</a> <a id='dmx' href='/dmx'>DMX</a> </div> <form action='/save' method='POST'> <div id='content'> <div id='buzzer_pressed'> <fieldset> <legend>Buzzer pressed:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Pressed Player</p> <input type='color' name='PressedPlayerColor' value='#0000FF'> <p>Other Players too late</p> <input type='color' name='OtherPlayersColor' value='#FFFF00'> <select name='BuzzerPressedSound'> <option value='Hard' selected>Hard</option> <option value='Medium'>Medium</option> <option value='Soft'>Soft</option> </select> <input type='submit' name='PlayBuzzerPressedSound' value='Play'> </div> </div> </fieldset> </div> <div id='right_anwser'> <fieldset> <legend>Right Answer:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Right pressed Player</p> <input type='color' name='RightPressePlayerColor' value='#00FF00'> <p>Right other Players</p> <input type='color' name='RightOtherPlayersColor' value='#00FF00'> <select name='RightAnswerSound'> <option value='Hard' selected>Hard</option> <option value='Medium'>Medium</option> <option value='Soft'>Soft</option> </select> <input type='submit' name='PlayRightAnswerSound' value='Play'> </div> </div> </fieldset> </div> <div id='wrong_anwser'> <fieldset> <legend>Wrong Answer:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Right pressed Player</p> <input type='color' name='WrongPressedPlayersColor' value='#FF0000'> <p>Right other Players</p> <input type='color' name='WrongOtherPlayersColor' value='#FF0000'> <select name='WrongAnswerSound'> <option value='Hard' selected>Hard</option> <option value='Medium'>Medium</option> <option value='Soft'>Soft</option> </select> <input type='submit' name='PlayWrongAnswerSound' value='Play'> </div> </div> </fieldset> </div> <div id='game_standby'> <fieldset> <legend>Standby:</legend> <div id='mar'> <div id='boxes' class='grid'> <p>Standby</p> <input type='color' name='StandbyColor' value='#000000'> <p>Deactivated Players</p> <input type='color' name='DeactivatedPlayersColor' value='#FF0000'> <select name='StandbyMusic'> <option value='WWM' selected>WWM</option> <option value='2'>Sound 2</option> <option value='3'>Sound 3</option> <option value='4'>Sound 4</option> <option value='5'>Sound 5</option> </select> <input type='submit' name='PlayStandbyMusic' value='Play'> </div> </div> </fieldset> </div> </div> <div id='save_discard'> <input id='discard' type='reset' value='Discard'> <input id='save' type='submit' value='Save'> </div> </form> </div> <script> const BuzzerPressed = {'PressedPlayerColor':'" + preferences.getString("PPC") + "', 'OtherPlayersColor':'" + preferences.getString("OPC") + "', 'BuzzerPressedSound':'" + preferences.getString("BPS") + "'}; const RightAnswer = {'RightPressePlayerColor':'" + preferences.getString("RPPC") + "', 'RightOtherPlayersColor':'" + preferences.getString("ROPC") + "', 'RightAnswerSound':'" + preferences.getString("RAS") +"'}; const WrongAnswer = {'WrongPressedPlayersColor':'" + preferences.getString("WPPC") + "', 'WrongOtherPlayersColor':'" + preferences.getString("WOPC") + "', 'WrongAnswerSound':'" + preferences.getString("WAS") + "'}; const Standby = {'StandbyColor':'" + preferences.getString("SC") + "', 'DeactivatedPlayersColor':'" + preferences.getString("DPC") + "', 'StandbyMusic':'" + preferences.getString("SM") + "'}; function ChangeValues (values) { for (i in values) { document.getElementsByName(i)[0].value = values[i]; } } ChangeValues(BuzzerPressed); ChangeValues(RightAnswer); ChangeValues(WrongAnswer); ChangeValues(Standby);</script>";
}

void updateDMX(){
  new_dmx = "<body> <div id='body'> <div id='nav-bar' class='grid'><a id='game' href=''>Game</a> <a class='active' id='configuration' href=''>Configuration</a> <a id='basics' href='/basics'>Basics</a> <a id='actions' href='/actions'>Actions</a> <a class='active' id='dmx' href=''>DMX</a> </div> <form action='/save' method='POST'> <div id='content' class='grid'> <div id='lights'> <fieldset> <legend>Lights: </legend> <div id='mar'> <div id='boxes' class='grid'> <p id='text1'>Number of Lights</p> <input id='box1' type='number' name='NumberOfLights' value='1'> <p id='text2'>Start Channel</p> <input id='box2' type='number' name='StartChannel' value='2'> <p id='text3'>Channel per Light</p> <input id='box3' type='number' name='ChannelPerLight' value='3'> <p id='ch_pos'>Channel Position</p> <div id='ch_pos_grid' class='grid'> <p class='bold'>B</p> <p class='bold'>R</p> <p class='bold'>G</p> <p class='bold'>B</p> <p class='bold'>W</p> <input type='number' name='Brightness' value='1'> <input type='number' name='Red' value='2'> <input type='number' name='Green' value='3'> <input type='number' name='Blue' value='4'> <input type='number' name='White' value='5'> <P>Brightness</P> <P>Red</P> <P>Green</P> <P>Blue</P> <P>White</P> </div> </div> </div> </fieldset> </div> <div id='test_dmx'> <input id='button' type='submit' value='Test DMX'> </div> </div> <div id='save_discard' class='grid'> <input id='discard' type='reset' value='Discard'> <input id='save' type='submit' value='Save'> </div> </form> </div> <script> const Lights = {'NumberOfLights':'" + preferences.getString("NOL") + "', 'StartChannel':'" + preferences.getString("SC") + "', 'ChannelPerLight':'" + preferences.getString("CPL") + "'}; const ChannelPosition = {'Brightness':'" + preferences.getString("Bright") + "', 'Red':'" + preferences.getString("Red") + "', 'Green':'" + preferences.getString("Green") + "', 'Blue':'" + preferences.getString("Blue") + "', 'White':'" + preferences.getString("White") + "'}; function ChangeValues (values) { for (i in values) { document.getElementsByName(i)[0].value = values[i]; } } ChangeValues(Lights); ChangeValues(ChannelPosition);</script>";

}

void handleSaveSettings(){
  if(server.hasArg("quiz_type")){
    preferences.putString("quiz_type", server.arg("quiz_type"));
    preferences.putString("PWNPR", BoolToString(server.hasArg("PlayerWrongNextPlayserbyReset")));
    preferences.putString("PCSD", BoolToString(server.hasArg("PlayersCanSeeDelays")));
    preferences.putString("PPMT", BoolToString(server.hasArg("PlayersPressMultipleTimes")));
    preferences.putString("TLPGTLC", BoolToString(server.hasArg("ToLatePlayersGetToLateColor")));
    preferences.putString("OPPGAC", BoolToString(server.hasArg("OnlyPressedPlayerGetAnswerColor")));
    updateBasics();
    server.sendHeader("Location","/basics");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
  }
  else if (server.hasArg("PressedPlayerColor")){
    preferences.putString("PPC", server.arg("PressedPlayerColor"));
    preferences.putString("OPC", server.arg("OtherPlayersColor"));
    preferences.putString("BPS", server.arg("BuzzerPressedSound"));
    preferences.putString("RPPC", server.arg("RightPressePlayerColor"));
    preferences.putString("ROPC", server.arg("RightOtherPlayersColor"));
    preferences.putString("RAS", server.arg("RightAnswerSound"));
    preferences.putString("WPPC", server.arg("WrongPressedPlayersColor"));
    preferences.putString("WOPC", server.arg("WrongOtherPlayersColor"));
    preferences.putString("WAS", server.arg("WrongAnswerSound"));
    preferences.putString("SC", server.arg("StandbyColor"));
    preferences.putString("DPC", server.arg("DeactivatedPlayersColor"));
    preferences.putString("SM", server.arg("StandbyMusic"));
    updateActions();
    server.sendHeader("Location","/actions");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
  }
  else if (server.hasArg("NumberOfLights")){
    preferences.putString("NOL", server.arg("NumberOfLights"));
    preferences.putString("SC", server.arg("StartChannel"));
    preferences.putString("CPL", server.arg("ChannelPerLight"));
    preferences.putString("Bright", server.arg("Brightness"));
    preferences.putString("Red", server.arg("Red"));
    preferences.putString("Green", server.arg("Green"));
    preferences.putString("Blue", server.arg("Blue"));
    preferences.putString("White", server.arg("White"));
    updateDMX();
    server.sendHeader("Location","/dmx");
    server.sendHeader("Cache-Control","no-cache");
    server.send(301);
  }
}

void handleRequest(){
}

void setup() {
  preferences.begin("my-app");
  //preferences.clear();
  Serial.begin(115200);
  Serial.print("Setting soft access point mode");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);

  //This is for the Wlan login websites
  server.on("/generate_204", handleRoot); //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/connectivitycheck.gstatic.com", handleRoot); //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);
  server.on("/nmcheck.gnome.org", handleRoot); //This is for Linux devices with the gnome desktop

  server.on("/login", handleLogin);
  server.on("/basics", handleBasics);
  server.on("/actions", handleActions);
  server.on("/dmx", handleDMX);
  server.on("/save", handleSaveSettings);
  server.onNotFound(handleRoot);

  updateBasics();

  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
}
