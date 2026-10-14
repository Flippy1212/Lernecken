#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SERVER_URL    "https://hidd.christianwill.de/api/webhook/WEBHOOK-ID"   // Webhook url
#define AUTH_TOKEN    "TOKEN"                                       // in HA festgelegt (roh_schreiben.sh)
#define DEVICE_ID     "1B131"                                                    // Geräte-ID des Esp
#define SEND_INTERVAL 15000UL                                                    // 15 s in ms

#define WIFI_SSID "LABOR SSID"
#define WIFI_PASS "LABOR PASSWORT"
#define NUM_PIXELS 64       // Anzahl sensor pixel
#define AVG_FRAMES 5       // Frames zur Mittelung
#define NUM_ZONES 4         // Anzahl Zonen
#define ZONE_PIXELS 12      // Pixel pro Zone

#define HUMAN_MIN 24.0f    // untere Menschtemp-Grenze
#define HUMAN_MAX 31.0f    // obere Grenze (darüber sicher kein Mensch) (Sonne etc.)
#define CHANGE_DELTA 0.5f  // ab wann ein Pixel als "verändert" gilt
#define HUMAN_HOLD 120      // Zyklen ohne Bewegung, bevor HUMAN aufgegeben wird (1 Zyklus ist Messung + Delay)

int readFails = 0, postFails = 0, lastCode = 0;
unsigned long lostSince = 0;
unsigned long lastSend = 0;
int humanHold[NUM_ZONES] = { 0 };  // verbleibende Halte-Zyklen pro Zone


const int zonePixels[NUM_ZONES][ZONE_PIXELS] = {
  { 4, 5, 6, 7, 12, 13, 14, 15, 20, 21, 22, 23 },      // Platz 1 von Eingang links im Uhrzeiger sinn
  { 44, 45, 46, 47, 52, 53, 54, 55, 60, 61, 62, 63 },  // Platz 2
  { 40, 41, 42, 43, 48, 49, 50, 51, 56, 57, 58, 59 },  // Platz 3
  { 0, 1, 2, 3, 8, 9, 10, 11, 16, 17, 18, 19 },        // Platz 4
};

// Moegliche Zustaende
enum ZoneState { EMPTY,
                 CHANGING,
                 HUMAN };
const char* stateName[] = { "EMPTY", "CHANGING", "HUMAN" };

ZoneState zoneState[NUM_ZONES] = { EMPTY, EMPTY, EMPTY, EMPTY };
float prevFrame[NUM_PIXELS] = { 0 };

//read temp data from amg8833 sensor (Adafruit amg8833 data sheet as reference) https://cdn-learn.adafruit.com/assets/assets/000/043/261/original/Grid-EYE_SPECIFICATIONS%28Reference%29.pdf?1498680225

bool readSensor(float* out) {
  uint8_t rawdata[128];
  int n = 0;

  Wire.beginTransmission(0x68);
  Wire.write(0x80);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 128);

  while (Wire.available() && n < 128) rawdata[n++] = Wire.read();
  if (n != 128) return false;

  for (int j = 0; j < NUM_PIXELS; j++) {
    int16_t raw = (rawdata[j * 2 + 1] << 8) | rawdata[j * 2];
    raw = (raw << 4) >> 4;  // 12-bit sign extend
    out[j] = raw * 0.25f;
  }
  return true;
}

// Mehrere Frames mitteln, alle 100ms auslesen
bool readAveraged(float* out) {
  float acc[NUM_PIXELS] = { 0 };
  float frame[NUM_PIXELS];
  int got = 0;
  for (int f = 0; f < AVG_FRAMES; f++) {
    if (readSensor(frame)) {
      for (int i = 0; i < NUM_PIXELS; i++) acc[i] += frame[i];
      got++;
    }
    delay(100);
  }
  if (got == 0) return false;
  for (int i = 0; i < NUM_PIXELS; i++) out[i] = acc[i] / got;
  return true;
}


// Max-Temperatur einer Zone
float zoneMaxTemp(const float* pixels, int z) {
  float mx = -100.0f;
  for (int k = 0; k < ZONE_PIXELS; k++) {
    float v = pixels[zonePixels[z][k]];
    if (v > mx) mx = v;
  }
  return mx;
}

// Mittel nur für Debug-Ausgabe
float zoneMeanTemp(const float* pixels, int z) {
  float sum = 0;
  for (int k = 0; k < ZONE_PIXELS; k++) sum += pixels[zonePixels[z][k]];
  return sum / ZONE_PIXELS;
}

// Hat sich IRGENDEIN Pixel der Zone seit letztem Frame geändert?
bool zoneChanged(const float* pixels, int z) {
  for (int k = 0; k < ZONE_PIXELS; k++) {
    int idx = zonePixels[z][k];
    if (fabsf(pixels[idx] - prevFrame[idx]) > CHANGE_DELTA) return true;
  }
  return false;
}


// for every zone: get max temp, decide if in human temp range
// switch depending on current state: if empty, and temps have changed -> state changing
// if in changing, and temp is human, switch to human state and start loop countdown
// if in human state, only switch out once no temp changes for HUMAN_HOLD loops
// if human and temp changes, reset loop countdown

void updateZones(const float* pixels) {
  for (int z = 0; z < NUM_ZONES; z++) {
    float temp = zoneMaxTemp(pixels, z);  // waermster Pixel der Zone
    bool changed = zoneChanged(pixels, z);
    bool humanTemp = (temp >= HUMAN_MIN && temp <= HUMAN_MAX);

    switch (zoneState[z]) {

      case EMPTY:
        if (changed) {
          if (temp >= HUMAN_MIN && temp <= HUMAN_MAX) {
            zoneState[z] = HUMAN;
            humanHold[z] = HUMAN_HOLD;   // start timer
          }
          else zoneState[z] = CHANGING;
        }
        break;

      case CHANGING:
        if (changed) {
          if (humanTemp) {
            zoneState[z] = HUMAN;
            humanHold[z] = HUMAN_HOLD;   // start timer
          }
        } else {
          zoneState[z] = EMPTY;
        }
        break;

      case HUMAN:
        if (changed) {
          if (humanTemp) {
            humanHold[z] = HUMAN_HOLD;  // movement, restart countdown
          } else {
            // movement, but not in human range, count down
            if (humanHold[z] > 0) humanHold[z]--;
            else zoneState[z] = CHANGING;
          }
        } else {
          // no movement, count down by one
          if (humanHold[z] > 0) {
            humanHold[z]--;  // stays human
          } else zoneState[z] = EMPTY;
            
        }
        break;
    }
  }

  for (int i = 0; i < NUM_PIXELS; i++) prevFrame[i] = pixels[i];    //update prev pixel data
}


// Print to serial monitor for debugging


// which zone belongs to which pixel idx
int pixelZone(int idx) {
  for (int z = 0; z < NUM_ZONES; z++)
    for (int k = 0; k < ZONE_PIXELS; k++)
      if (zonePixels[z][k] == idx) return z;
  return -1;
}

// short sign to indicate state
char stateChar(ZoneState s) {
  switch (s) {
    case EMPTY:     return 'E';
    case CHANGING: return '~';
    case HUMAN:    return 'H';
  }
  return '?';
}

void printZoneGrid(const float* pixels) {
  // 8x8 grid: temp + state
  for (int line = 0; line < 8; line++) {
    for (int col = 0; col < 8; col++) {
      int idx = line * 8 + col;
      int z   = pixelZone(idx);
      char marker = (z >= 0) ? stateChar(zoneState[z]) : ' ';
      Serial.printf("%4.1f%c ", pixels[idx], marker);
    }
    Serial.println();
  }
  Serial.println();

  // info per zone
  for (int z = 0; z < NUM_ZONES; z++)
    Serial.printf("Zone %d: %-8s (max %.1f, %s, hold %d)\n",
                  z+1, stateName[zoneState[z]],
                  zoneMaxTemp(pixels, z),
                  zoneChanged(pixels, z) ? "changed" : "steady",
                  humanHold[z]);
  Serial.println("  E EMPTY   ~ changing   H human\n");
}


//POST to HA Webhook, to display live data


void sendStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Kein WiFi, Senden übersprungen");
    return;
  }

  // zones: 1 if human, else 0
  int p[NUM_ZONES];
  for (int z = 0; z < NUM_ZONES; z++)
    p[z] = (zoneState[z] == HUMAN) ? 1 : 0;

  String json = "{";
  json += "\"token\":\"" AUTH_TOKEN "\",";
  json += "\"id\":\"" DEVICE_ID "\",";
  json += "\"platz1\":" + String(p[0]) + ",";
  json += "\"platz2\":" + String(p[1]) + ",";
  json += "\"platz3\":" + String(p[2]) + ",";
  json += "\"platz4\":" + String(p[3]);
  json += "}";

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);
  int code = http.POST(json);
  http.end();

  lastCode = code;
  Serial.printf("POST -> %d  %s\n", code, json.c_str());
  if (code < 200 || code >= 300) { if (++postFails > 10) ESP.restart(); }
  else postFails = 0;
  
}



// setup and loop

void setup() {
  Serial.begin(115200);
  delay(2000);
  Wire.begin(47, 48, 100000);
  Wire.beginTransmission(0x68);
  Serial.println(Wire.endTransmission() == 0 ? "Sensor gefunden!" : "Sensor nicht gefunden!");

  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > 60000UL) ESP.restart();
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi verbunden: " + WiFi.localIP().toString());

  // Baseline: fill prevFrame with first values, for comparsion in updateZones
  if (readAveraged(prevFrame)) Serial.println("Baseline-Frame gesetzt.");
}

//logic: read short avg of pixel values, update zones depending on current state and avg pixel values,
//decide if occupied, post to webhook every SEND_INTERVAL seconds


void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    if (lostSince == 0) { lostSince = millis(); WiFi.reconnect(); }
    if (millis() - lostSince > 60000UL) ESP.restart();   // 1 min kein WLAN -> Neustart
    delay(1000);
    return;
  }
  lostSince = 0;                 // WLAN ist (wieder) da

  float pixels[NUM_PIXELS];
  if (!readAveraged(pixels)) {
    Serial.println("Lesefehler");
    if (++readFails > 20) ESP.restart();               // Sensor/I2C hängt -> Neustart
    delay(500);
    return;
  }
  readFails = 0;

  updateZones(pixels);
  printZoneGrid(pixels);

  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();
    sendStatus();
  }
  delay(500);
}