#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NUE";
const char* password = "15122007";

WebServer server(80);

int joyX = 0;
int joyY = 0;
int buttonA = 0;

void handleRoot() {
  String html = "<html><head><meta http-equiv='refresh' content='0.01'/></head><body>";
  html += "<h2>ESP32 Controller Data</h2>";
  html += "<p>joyX: " + String(joyX) + "</p>";
  html += "<p>joyY: " + String(joyY) + "</p>";
  html += "<p>buttonA: " + String(buttonA) + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  // here update joyX, joyY, buttonA from your controller code
  
  server.handleClient();
}