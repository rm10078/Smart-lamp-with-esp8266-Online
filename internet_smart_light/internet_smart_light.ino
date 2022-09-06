#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

String ssid;
String password;
String token,vpin,url_api;

AsyncWebServer server(80);

int light_pin=14;

int w_s=0;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart lamp</title>
    <style>
        body{
    background-color: rgb(0, 0, 0);
}
h1{
    color: white;
}
p{
    color: white;
}
h3{
    color: white;
}
h2{
    color: white;
}

.form form button{
    margin: auto;
    margin-top: 2vw;
}

.all{
    margin: auto;
    text-align: center;
}
input{
    width: 20vw;
    height: 2vw;
    background-color: #fff;
    border-color: black;
    border-style: solid;
    border-width: 2px;
    margin: auto;
}
button{
    margin-top: 2vw;
    width: 9vw;
    height: 3.5vw;
    border-radius: 10vw;
    border-style: none;
    box-shadow: rgb(0, 102, 198) 2px 2px;
    display: block;
    color: rgb(255, 255, 255);
    background-color: rgb(0, 157, 255);
    font-size: 1.5vw;
}
button:hover{
    transition: 100ms;
    background-color: rgb(214, 40, 118);
}
@media only screen and (max-width: 600px) {
    h1{
        font-size: 6vw;
    }
    h2{
        font-size: 4vw;
    }
    h3{
        font-size: 3vw;
    }
    p{
        font-size: 2.5vw;
    }

    input{
        width: 30vw;
        height: 5vw;
    }
    button{
        margin-top: 2vw;
        width: 15vw;
        height: 4vw;
    }

}
    </style>
</head>
<body>
    <div class="title all">
        <h1>Smart light Online mode</h1>
        <p>This is a small project.</p>
    </div>
    <div class="form all">
        <form action="/update" method="get">
            <h3>SSID :</h3>
            <input type="text" name="ssid" placeholder="Type ssid">
            <h3>Password :</h3>
            <input type="text" name="password" placeholder="Type Password">
            <h3>Blynk Token :</h3>
            <input type="text" name="token" placeholder="Type Token">
            <h3>Blynk virtual pin name :</h3>
            <input type="text" name="vpin" placeholder="Type vpin:v0">
            <button type="submit">Submit</button>
        </form>
    </div>
    <div class="note all">
        <h1>Note</h1>
        <p>Only use blynk api.</br>  Type your token without any space.</br>  Type your virtual pin name like(v0).</p>
    </div>
    <div class="about all">
        <h1>About us</h1>
        <p>Hello friends!</br>This is a fun project.</p>
        <a href="https://github.com/rm10078">More</a>
    </div>
</body>
</html>
)rawliteral";

String processor(const String& var){
}


void eeprom_write(String word,int start_add) {
  delay(10);

  for (int i = 0; i < word.length(); ++i) {
    //Serial.println(word[i]);
    EEPROM.write(start_add+i, word[i]);
  }

  EEPROM.write(start_add+word.length(), '\0');
  EEPROM.commit();
}

String eeprom_read(int start_add) {
  String word;
  char readChar='n';
  int i = 0;

  while (readChar != '\0') {
    int tem=start_add+i;
    readChar = char(EEPROM.read(tem));
    delay(10);
    i++;

    if (readChar != '\0') {
      word += readChar;
    }
  }

  return word;
}

void light_write(int duty_cycle){
  int val=map(duty_cycle,0,100,0,255);
  analogWrite(light_pin,val);
  EEPROM.write(500,duty_cycle);
  EEPROM.commit();
  }

void setup() {
  // put your setup code here, to run once:
  analogWriteFreq(10000);
  pinMode(light_pin,OUTPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
  ssid=eeprom_read(0);
  password=eeprom_read(100);
  token=eeprom_read(200);
  vpin=eeprom_read(400);
  url_api="http://blynk.cloud/external/api/get?token="+token+"&"+vpin;
  int tem2=EEPROM.read(500);
  light_write(tem2);
  Serial.println("");
  Serial.println(ssid+"   "+password+"   "+token+"  "+vpin+"  "+url_api);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int count=0;
  while (WiFi.status() != WL_CONNECTED && count<100) {
    delay(100);
    Serial.print(".");
    count++;
  }
    //run all code
if(WiFi.status() == WL_CONNECTED){
  Serial.print("connected");
  w_s=1;
  delay(1000);
}
    
  if (WiFi.status() != WL_CONNECTED){
    //config page
    WiFi.mode(WIFI_OFF);
    delay(250);
    //wifi config
  WiFi.softAP("smart light config", "");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(500);

  //server
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message,ssid_v,password_v,token_v,vpin_v;
    
    if (request->hasParam("ssid")) {
      message = request->getParam("ssid")->value();
      ssid_v = message;
    //call the output function
    }
    if (request->hasParam("password")) {
      message = request->getParam("password")->value();
      password_v = message;
    //call the output function
    }
    if (request->hasParam("token")) {
      message = request->getParam("token")->value();
      token_v = message;
    //call the output function
    }
    if (request->hasParam("vpin")) {
      message = request->getParam("vpin")->value();
      vpin_v = message;
    //call the output function
    }
    else {
      message = "No message sent";
    }
    Serial.println(ssid_v+"  "+password_v+"  "+token_v+"  "+vpin_v);
    
    if(password_v.length()<8){
      request->send(200, "text/plain", "Type password currectly.");
      }
    else{
      request->send(200, "text/plain", "OK REBOOT THE DEVICE");
      if(ssid_v!=""){
        eeprom_write(ssid_v,0);
        }
      if(token_v!=""){
      eeprom_write(token_v,200);
      }
      if(vpin_v!=""){
      eeprom_write(vpin_v,400);
      }
      eeprom_write(password_v,100);
      //http://blynk.cloud/external/api/get?token=tjcf3BYLULWqu3j1z1asFVIzaV_X8PoT&v0 
      }
  });
  
  server.begin();
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (w_s) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, url_api)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          if(payload.length()<=4){
          light_write(payload.toInt());
          }
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(5000);
}
