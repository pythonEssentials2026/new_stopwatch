#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <SPIFFS.h>
#include <FS.h>
#include "html.h"
#include "css.h"
#include "js.h"


// === Globals ===
String resultLog = "";
const char* CSV_PATH = "/results.csv";

// === Matrix and Buzzer Configuration ===
#define CS_PIN 5
#define BUZZER_PIN 27
const byte H_DISPLAYS = 4;
const byte V_DISPLAYS = 1;
Max72xxPanel matrix = Max72xxPanel(CS_PIN, H_DISPLAYS, V_DISPLAYS);

// === Stopwatch Variables ===
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
bool running = false;

// === Wi-Fi Access Point Settings ===
const char* ssid = "Stopwatch";
WebServer server(80);

// === Utils ===
static String sanitizeForCSV(const String& s) {
  String out = s;
  out.replace("\r", " ");
  out.replace("\n", " ");
  out.replace(",", " ");
  return out;
}

static void appendCSVLine(const String& line) {
  File f = SPIFFS.open(CSV_PATH, FILE_APPEND);
  if (!f) { Serial.println("Failed to open CSV for append"); return; }
  f.println(line);
  f.close();
}

static void loadResultsFromCSV() {
  resultLog = "";
  if (!SPIFFS.exists(CSV_PATH)) return;
  File f = SPIFFS.open(CSV_PATH, FILE_READ);
  if (!f) { Serial.println("Failed to open CSV for read"); return; }
  while (f.available()) {
    String row = f.readStringUntil('\n');
    row.trim();
    if (!row.length()) continue;

    // Try to parse both old 4-col format and new 5-col format
    int p1 = row.indexOf(',');
    int p2 = (p1 >= 0) ? row.indexOf(',', p1 + 1) : -1;
    int p3 = (p2 >= 0) ? row.indexOf(',', p2 + 1) : -1;
    int p4 = (p3 >= 0) ? row.indexOf(',', p3 + 1) : -1;

    if (p4 > p3 && p3 > p2 && p2 > p1 && p1 > 0) {
      // name, pool, stroke, distance, time
      String name = row.substring(0, p1);
      String pool = row.substring(p1 + 1, p2);
      String stroke = row.substring(p2 + 1, p3);
      String distance = row.substring(p3 + 1, p4);
      String timeStr = row.substring(p4 + 1);
      resultLog += name + " - " + pool + " - " + stroke + " - " + distance + " - " + timeStr + "<br>";
    } else if (p3 > p2 && p2 > p1 && p1 > 0) {
      // legacy: name, stroke, distance, time
      String name = row.substring(0, p1);
      String stroke = row.substring(p1 + 1, p2);
      String distance = row.substring(p2 + 1, p3);
      String timeStr = row.substring(p3 + 1);
      resultLog += name + " - " + stroke + " - " + distance + " - " + timeStr + "<br>";
    } else {
      resultLog += row + "<br>";
    }
  }
  f.close();
}

// === Colon Drawing Function ===
void drawColon(int x, int y) {
  matrix.drawPixel(x, y + 1, HIGH);
  matrix.drawPixel(x, y + 5, HIGH);
}

// === Display Time as M:SS:MS ===
void displayTime(unsigned long ms) {
  matrix.fillScreen(LOW);
  unsigned long totalSeconds = ms / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  unsigned long hundredths = (ms % 1000) / 10;

  byte m = minutes;
  byte s1 = seconds / 10;
  byte s2 = seconds % 10;
  byte ms1 = hundredths / 10;

  int x = 0, y = 0;
  matrix.drawChar(x, y, '0' + m, HIGH, LOW, 1); x += 6;
  drawColon(x, y);                               x += 2;
  matrix.drawChar(x, y, '0' + s1, HIGH, LOW, 1); x += 6;
  matrix.drawChar(x, y, '0' + s2, HIGH, LOW, 1); x += 6;
  drawColon(x, y);                               x += 2;
  matrix.drawChar(x, y, '0' + ms1, HIGH, LOW, 1);
  matrix.write();
}



void handleStart() {
  if (!running) {
    startTime = millis() - elapsedTime;
    running = true;
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
  }
  server.send(200, "text/plain", "Started");
}

void handleStop() {
  if (running) {
    elapsedTime = millis() - startTime;
    running = false;
  }
  server.send(200, "text/plain", "Stopped");
}

void handleReset() {
  running = false;
  elapsedTime = 0;
  displayTime(0);
  server.send(200, "text/plain", "Reset");
}

void handleTimeRequest() {
  unsigned long ms = elapsedTime;
  if (running) ms = millis() - startTime;

  unsigned long totalSeconds = ms / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  unsigned long hundredths = (ms % 1000) / 10;

  char buf[12];
  snprintf(buf, sizeof(buf), "%lu:%02lu:%02lu", minutes, seconds, hundredths);
  server.send(200, "text/plain", buf);
}

void handleFinish() {
  if (!server.hasArg("name") || !server.hasArg("stroke") || !server.hasArg("distance")) {
    server.send(400, "text/plain", "Missing fields");
    return;
  }

  String name = sanitizeForCSV(server.arg("name"));
  String pool = sanitizeForCSV(server.arg("pool"));        // may be empty
  String stroke = sanitizeForCSV(server.arg("stroke"));
  String distance = sanitizeForCSV(server.arg("distance"));

  unsigned long ms = running ? (millis() - startTime) : elapsedTime;
  running = false;

  unsigned long totalSeconds = ms / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  unsigned long hundredths = (ms % 1000) / 10;

  char timeBuf[12];
  snprintf(timeBuf, sizeof(timeBuf), "%lu:%02lu:%02lu", minutes, seconds, hundredths);
  String timeStr = String(timeBuf);

  // CSV now: name,pool,stroke,distance,time
  appendCSVLine(name + "," + pool + "," + stroke + "," + distance + "," + timeStr);

  // Update results panel
  if (pool.length())
    resultLog += name + " - " + pool + " - " + stroke + " - " + distance + " - " + timeStr + "<br>";
  else
    resultLog += name + " - " + stroke + " - " + distance + " - " + timeStr + "<br>";

  // Reset stopwatch
  elapsedTime = 0;
  displayTime(0);
  server.send(200, "text/plain", "Time recorded and reset");
}

void handleDownloadCSV() {
  if (!SPIFFS.exists(CSV_PATH)) {
    // New header with pool column
    server.send(200, "text/csv", "name,pool,stroke,distance,time\n");
    return;
  }
  File f = SPIFFS.open(CSV_PATH, FILE_READ);
  if (!f) { server.send(500, "text/plain", "Failed to open CSV"); return; }
  server.sendHeader("Content-Disposition", "attachment; filename=results.csv");
  server.streamFile(f, "text/csv");
  f.close();
}

void handleClear() {
  SPIFFS.remove(CSV_PATH);
  resultLog = "";
  server.send(200, "text/plain", "Cleared");
}

void handleResultsJSON() {
  String json = "[";
  if (SPIFFS.exists(CSV_PATH)) {
    File f = SPIFFS.open(CSV_PATH, FILE_READ);
    bool first = true;
    while (f.available()) {
      String row = f.readStringUntil('\n'); row.trim();
      if (!row.length()) continue;
      int p1 = row.indexOf(','), p2 = (p1 >= 0) ? row.indexOf(',', p1 + 1) : -1, p3 = (p2 >= 0) ? row.indexOf(',', p2 + 1) : -1, p4 = (p3 >= 0) ? row.indexOf(',', p3 + 1) : -1;
      if (p4 > p3 && p3 > p2 && p2 > p1 && p1 > 0) {
        // name,pool,stroke,distance,time
        String name = row.substring(0, p1);
        String pool = row.substring(p1 + 1, p2);
        String stroke = row.substring(p2 + 1, p3);
        String distance = row.substring(p3 + 1, p4);
        String timeStr = row.substring(p4 + 1);
        if (!first) json += ",";
        json += "{\"name\":\"" + name + "\",\"pool\":\"" + pool + "\",\"stroke\":\"" + stroke + "\",\"distance\":\"" + distance + "\",\"time\":\"" + timeStr + "\"}";
        first = false;
      } else if (p3 > p2 && p2 > p1 && p1 > 0) {
        // legacy 4 columns
        String name = row.substring(0, p1);
        String stroke = row.substring(p1 + 1, p2);
        String distance = row.substring(p2 + 1, p3);
        String timeStr = row.substring(p3 + 1);
        if (!first) json += ",";
        json += "{\"name\":\"" + name + "\",\"pool\":\"\",\"stroke\":\"" + stroke + "\",\"distance\":\"" + distance + "\",\"time\":\"" + timeStr + "\"}";
        first = false;
      }
    }
    f.close();
  }
  json += "]";
  server.send(200, "application/json", json);
}

// === Matrix Initialization ===
void setupMatrix() {
  matrix.setIntensity(3);
  for (int i = 0; i < H_DISPLAYS; i++) {
    matrix.setPosition(i, i, 0);
    matrix.setRotation(i, 1);
  }
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  matrix.setTextColor(HIGH);
  displayTime(0);
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  setupMatrix();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  } else {
    loadResultsFromCSV();
  }

  WiFi.softAP(ssid);
  delay(1000);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.on("/stop", handleStop);
  server.on("/reset", handleReset);
  server.on("/time", handleTimeRequest);
  server.on("/finish", handleFinish);

  server.on("/download", handleDownloadCSV);
  server.on("/clear", HTTP_POST, handleClear);
  server.on("/results.json", handleResultsJSON);

  server.begin();

    server.on("/", handleRoot);
  server.on("/app.css", []() {
    server.send_P(200, "text/css", APP_CSS);
  });
  server.on("/app.js", []() {
    server.send_P(200, "application/javascript", APP_JS);
  });
}

// === Loop ===
void loop() {
  static unsigned long lastDisplay = 0;
  unsigned long now = millis();

  server.handleClient();

  if (running) elapsedTime = now - startTime;

  if (now - lastDisplay >= 50) {
    displayTime(elapsedTime);
    lastDisplay = now;
  }
}
