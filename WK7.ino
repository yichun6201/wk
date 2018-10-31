#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "iPhone";
const char* password = "hihihihi";

WebServer server(80);
String website;
const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html",website);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  ledcSetup(0,5000,8);
  ledcAttachPin(led,0);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  buildWeb();
  server.on("/", handleRoot);
  server.on("/ledDiming",ledDiming);

  server.on("/inline", []() {
    server.send(200, "text/html",website);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop(void) {
  server.handleClient();
}
void ledDiming(){
  int led_Diming=server.arg("ledval").toInt();
  ledcWrite(0,led_Diming);
  Serial.println(led_Diming);
  server.send(200, "text/html",website);
  }

void buildWeb(){
  website +="<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>Diming LED</title> <script src=\"https://code.jquery.com/jquery-3.3.1.js\"></script> <script>$(document).ready(function(){$(\"[type=range]\").change(function(){var ledvalue=$(\"#led\").val(); $(\".ledvalue\").text(ledvalue);});}); </script> <style>body{background-color: lightgoldenrodyellow;}.container{display: flex; flex-direction: column; align-items: center;}.btn{padding: 0.3em 0.5em; font-size: 1.5em;}</style></head><body> <div class=\"container\"> <h1>LED Diming</h1> <h3>LED Light=<span class='ledvalue'>0</span></h3> <form action=\"ledDiming\" method=\"get\" id=\"form1\"> <input type=\"range\" name=\"ledval\" id=\"led\" min=\"0\" max=\"255\"> </form> <br><button type=\"submit\" form=\"form1\" value=\"Submit\" class=\"btn\">Submit</button> </div></body></html>" ;}
