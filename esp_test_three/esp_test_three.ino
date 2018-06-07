
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress    apIP(42, 42, 42, 42);  // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "ESP8266";
const char *password = "hellojeff123";

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

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

//void handleRoot() {
//  
//  server.send ( 200, "text/html", html );
//  digitalWrite ( LED_BUILTIN, 1 );
//}

void start (void) {
  Serial.println("start");
}

void finish (void){
  Serial.println("finish");
}

void handleNotFound(void) {
  server.send ( 404, "text/plain", "404" );
}

void setup() {
  
  pinMode(lux_pin, INPUT_PULLUP);
  pinMode(dip_sw_state, INPUT_PULLUP);
  pinMode(dip_sw_result, INPUT_PULLUP);
  pinMode(pot_in, INPUT_PULLUP);
  pinMode(sound_in, INPUT_PULLUP);
  pinMode ( ledPin, OUTPUT );
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", start );
  server.on ( "/start", start);
  server.on ( "/stop", finish);
//  server.on ( "/inline", []() {
//    server.send ( 200, "text/plain", "this works as well" );
//  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
