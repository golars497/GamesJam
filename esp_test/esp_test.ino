#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>

#include "ESP8266WiFiGeneric.h"
#include "ESP8266WiFiAP.h"

const int lux_pin = 13;
const int dip_sw_state = 12;
const int dip_sw_result = 14;
const int pot_in = 16;
const int sound_in  = 4;

int lux;
int dip_state;
int dip_res;
int pot;
int sound;

#define DataBufferSize 2056

WebSocketsServer webSocket = WebSocketsServer(81);    // create a websocket server on port 81

const char *ssid = "UOGCar"; // The name of the Wi-Fi network that will be created

const char *password = "VroomVroom";   // The password required to connect to it, leave blank for an open network

const char* mdnsName = "car"; // Domain name for the mDNS responder

 

bool sendData = false;

uint8_t dataBuffer[DataBufferSize];

uint16_t nBytes = 0;

String readString = "";

//void connectToWifi (void) {
////the HTML of the web page
//
//  delay(1000);
//  WiFi.softAP(ssid, password); //begin WiFi access point
//  Serial.println("");
// 
//  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
//  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.softAPIP()); 
//   
//}

void startWiFi()

{

  IPAddress ip(10,1,1,200);  

  IPAddress gateway(10,1,1,254);  

  IPAddress subnet(255,255,255,0);  

  WiFi.softAP(ssid, password);             // Start the access point

  WiFi.softAPConfig(ip, gateway, subnet);
  delay(500);
  Serial1.println(WiFi.localIP());
  Serial.println(WiFi.localIP());

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  Serial.println(type);
  switch (type) {

    case WStype_DISCONNECTED:             // if the websocket is disconnected

      Serial1.printf("[%u] Disconnected!\n", num);
      Serial.println("DISCONENCTED");

      sendData = false;

      break;

     

    case WStype_CONNECTED: {              // if a new websocket connection is established

        IPAddress ip = webSocket.remoteIP(num);

        Serial1.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        Serial.println("WE HAVE CONNECTED FAM");
        sendData = true;

      }

      break;

     

    case WStype_TEXT:                     // if new text data is received

      Serial1.printf("[%u] get Text: %s\n", num, payload);

      Serial.printf("%s",payload);

      break;

  }

}\ 

void startWebSocket() { // Start a WebSocket server

  webSocket.begin();                          // start the websocket server

  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'

  Serial1.println("WebSocket server started.");
  Serial.println("STARTED MATE");

}



void setup() {

  Serial.begin(115200);

  Serial1.begin(115200);

  startWiFi();

  startWebSocket();

  pinMode(lux_pin, INPUT_PULLUP);
  pinMode(dip_sw_state, INPUT_PULLUP);
  pinMode(dip_sw_result, INPUT_PULLUP);
  pinMode(pot_in, INPUT_PULLUP);
  pinMode(sound_in, INPUT_PULLUP);

}

void readArduinoMega(void) {
  //0 = there is NO light
  //1 = there is light
  //Serial.print("lux: ");
  //Serial.print(digitalRead(lux_pin));
  //Serial.print("   sw on: ");
  //Serial.print(digitalRead(dip_sw_state));
  //Serial.print("   sw_res: ");
  //Serial.print(digitalRead(dip_sw_result));
  //Serial.println("");  
  lux = digitalRead(lux_pin);
  dip_state = digitalRead(dip_sw_state);
  dip_res = digitalRead(dip_sw_result);  
  pot = digitalRead(pot_in);
  sound = digitalRead(sound_in);
  //Serial.println(sound);
}

void printWiFiStatus() {
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //readArduinoMega();
//   if (WiFi.status() != WL_CONNECTED) {
//    while (WiFi.status() != WL_CONNECTED) {
//      delay(500);
//      Serial.println("Not connected");
//    }
//    // Print the new IP to Serial. 
//    printWiFiStatus();
//  }

}


