#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

#define MPU_ADDR 0x68

// MPU6050 registers
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B

const char* ssid = "NUE";
const char* password = "15122007";

WebServer server(80);

// Raw data
int16_t rawAx, rawAy, rawAz;
int16_t rawTemp;
int16_t rawGx, rawGy, rawGz;

// Converted values
float ax, ay, az;
float gx, gy, gz;

// Offsets
float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;

float accelOffsetX = 0;
float accelOffsetY = 0;
float accelOffsetZ = 0;

// Angles
float accelAngleX = 0;
float accelAngleY = 0;

float angleX = 0;
float angleY = 0;

unsigned long lastTime = 0;

// HTML page
const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 MPU6050 Monitor</title>
  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: #111;
      color: #eee;
      text-align: center;
    }

    h2 {
      margin-top: 20px;
    }

    .info {
      margin: 20px auto;
      width: 320px;
      text-align: left;
      background: #1c1c1c;
      padding: 15px;
      border-radius: 12px;
      box-shadow: 0 0 10px rgba(255,255,255,0.08);
      font-size: 18px;
      line-height: 1.8;
    }
  </style>
</head>
<body>
  <h2>ESP32 MPU6050 Monitor</h2>

  <div class="info">
    <div>Roll (angleX): <span id="roll">0</span></div>
    <div>Pitch (angleY): <span id="pitch">0</span></div>
    <div>AccelAngleX: <span id="accx">0</span></div>
    <div>AccelAngleY: <span id="accy">0</span></div>
    <div>Gyro X: <span id="gyrox">0</span></div>
    <div>Gyro Y: <span id="gyroy">0</span></div>
  </div>

  <script>
    async function updateData() {
      try {
        const response = await fetch("/data");
        const data = await response.json();

        document.getElementById("roll").innerText = data.angleX.toFixed(2);
        document.getElementById("pitch").innerText = data.angleY.toFixed(2);
        document.getElementById("accx").innerText = data.accelAngleX.toFixed(2);
        document.getElementById("accy").innerText = data.accelAngleY.toFixed(2);
        document.getElementById("gyrox").innerText = data.gx.toFixed(2);
        document.getElementById("gyroy").innerText = data.gy.toFixed(2);
      } catch (e) {
        console.log("Fetch error:", e);
      }
    }

    setInterval(updateData, 100);
    updateData();
  </script>
</body>
</html>
)rawliteral";

// ---------------------- MPU Helpers ----------------------

void writeRegister(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void setupMPU() {
  writeRegister(PWR_MGMT_1, 0);
}

bool readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0) return false;

  Wire.requestFrom(MPU_ADDR, 14);
  if (Wire.available() != 14) return false;

  rawAx = (Wire.read() << 8) | Wire.read();
  rawAy = (Wire.read() << 8) | Wire.read();
  rawAz = (Wire.read() << 8) | Wire.read();

  rawTemp = (Wire.read() << 8) | Wire.read();

  rawGx = (Wire.read() << 8) | Wire.read();
  rawGy = (Wire.read() << 8) | Wire.read();
  rawGz = (Wire.read() << 8) | Wire.read();

  ax = rawAx / 16384.0;
  ay = rawAy / 16384.0;
  az = rawAz / 16384.0;

  gx = rawGx / 131.0;
  gy = rawGy / 131.0;
  gz = rawGz / 131.0;

  return true;
}

void calibrateMPU() {
  const int samples = 1000;

  float sumAx = 0;
  float sumAy = 0;
  float sumAz = 0;
  float sumGx = 0;
  float sumGy = 0;
  float sumGz = 0;

  Serial.println("Calibration started. Keep MPU still.");

  for (int i = 0; i < samples; i++) {
    if (readMPU()) {
      sumAx += ax;
      sumAy += ay;
      sumAz += az;

      sumGx += gx;
      sumGy += gy;
      sumGz += gz;
    }
    delay(3);
  }

  accelOffsetX = sumAx / samples;
  accelOffsetY = sumAy / samples;
  accelOffsetZ = (sumAz / samples) - 1.0;

  gyroOffsetX = sumGx / samples;
  gyroOffsetY = sumGy / samples;
  gyroOffsetZ = sumGz / samples;

  Serial.println("Calibration finished.");
}

void initAnglesFromAccel() {
  if (!readMPU()) return;

  float axC = ax - accelOffsetX;
  float ayC = ay - accelOffsetY;
  float azC = az - accelOffsetZ;

  accelAngleX = atan2(ayC, sqrt(axC * axC + azC * azC)) * 180.0 / PI;
  accelAngleY = atan2(-axC, sqrt(ayC * ayC + azC * azC)) * 180.0 / PI;

  angleX = accelAngleX;
  angleY = accelAngleY;
}

void updateAngles() {
  if (!readMPU()) return;

  float axC = ax - accelOffsetX;
  float ayC = ay - accelOffsetY;
  float azC = az - accelOffsetZ;

  float gxC = gx - gyroOffsetX;
  float gyC = gy - gyroOffsetY;

  accelAngleX = atan2(ayC, sqrt(axC * axC + azC * azC)) * 180.0 / PI;
  accelAngleY = atan2(-axC, sqrt(ayC * ayC + azC * azC)) * 180.0 / PI;

  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  if (dt <= 0 || dt > 0.1) dt = 0.01;

  angleX += gxC * dt;
  angleY += gyC * dt;

  angleX = 0.98 * angleX + 0.02 * accelAngleX;
  angleY = 0.98 * angleY + 0.02 * accelAngleY;
}

// ---------------------- Web Handlers ----------------------

void handleRoot() {
  server.send_P(200, "text/html", MAIN_page);
}

void handleData() {
  String json = "{";
  json += "\"angleX\":" + String(angleX, 2) + ",";
  json += "\"angleY\":" + String(angleY, 2) + ",";
  json += "\"accelAngleX\":" + String(accelAngleX, 2) + ",";
  json += "\"accelAngleY\":" + String(accelAngleY, 2) + ",";
  json += "\"gx\":" + String(gx - gyroOffsetX, 2) + ",";
  json += "\"gy\":" + String(gy - gyroOffsetY, 2);
  json += "}";

  server.send(200, "application/json", json);
}

// ---------------------- Setup / Loop ----------------------

void setup() {
  Serial.begin(115200);
  Wire.begin();

  delay(500);
  setupMPU();
  delay(1000);

  calibrateMPU();
  initAnglesFromAccel();
  lastTime = micros();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("Web server started.");
}

void loop() {
  updateAngles();
  server.handleClient();
}