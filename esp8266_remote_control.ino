//TODO: Remote configure WiFi, instead of hardcode SSID and password
//TODO: Fix WPA2 Enterprise connectivity issue (e.g. eduroam and NTUSECURE)

#include<ESP8266WiFi.h>
#include<ESP8266WiFiMulti.h>
#include<WebSocketsClient.h>

#define LEDPIN 2
#define RELAYPIN 0

const char ssid[] = "Steve_2.4";
const char password[] = "qwqwqwqwq";
const char host[] = "remote-control.stevezheng.cf";
const int remote_port = 443;
const char remote_url[] = "/";

const char LEDON[] = "ledOn";
const char LEDOFF[] = "ledOff";

bool LEDStatus;

WebSocketsClient webSocket;
ESP8266WiFiMulti wifi_client;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length){
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("Connected!\n");
      if(LEDStatus){
        webSocket.sendTXT(LEDON);
      }
      else{
        webSocket.sendTXT(LEDOFF);
      }
      break;
    case WStype_TEXT:
      Serial.printf("Get text: %s\r\n",payload);
      if (strcmp(LEDON, (const char *)payload) == 0) {
        Serial.printf("On!\n");
        writeLED(true);
      }
      else if (strcmp(LEDOFF, (const char *)payload) == 0) {
        Serial.printf("Off!\n");
        writeLED(false);
      }
      else {
        Serial.println("Unknown command");
      }
      break;
   default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

void writeLED(bool state){
  LEDStatus = state;
  if(state){
    digitalWrite(LEDPIN,0);
    digitalWrite(RELAYPIN,0);
  }
  else{
    digitalWrite(LEDPIN,1);
    digitalWrite(RELAYPIN,1);
  }
}

void setup(){
  pinMode(LEDPIN,OUTPUT);
  pinMode(RELAYPIN,OUTPUT);
  writeLED(true);
  Serial.begin(115200);
  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  
  wifi_client.addAP(ssid,password);
  while(wifi_client.run()!= WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.beginSSL(host,remote_port,remote_url);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000,3000,2);
}
void loop(){
  webSocket.loop();
}
