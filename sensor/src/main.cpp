#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>

#include <DHT.h> // Temperature and humidity sensor

#include <ESPAsyncTCP.h>
#define WEBSERVER_H // avoid conflict with ESP8266WebServer.h
#include <ESPAsyncWebServer.h>

#include <pgmspace.h> // for PROGMEM
// #include <LittleFS.h>
#include <FS.h>

#include <WebSerial.h>
// disable warnings for deprecated Class SPIFFS
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define HOSTNAME "OpenThermostat Sensor"
#define REFRESH_DELAY 10 * 60 * 1000 // 10 minutes
#define DHTPIN 2      // what digital pin we're connected to
#define DHTTYPE DHT11 // DHT 11

AsyncWebServer server(80);
WiFiClient client;
DHT dht(DHTPIN, DHTTYPE, 11);
const char* hub_ip;
char serverUrl[256];

void msgCallback(uint8_t* data, size_t len)
{
  char* msg = new char[len + 1];
  memcpy(msg, data, len);
  msg[len] = '\0';
  WebSerial.print("Received: ");
  WebSerial.println(msg);
  Serial.println(msg);
  delete[] msg;
}
void setup()
{
  SPIFFS.begin();
  WiFiManager wifiManager;
  wifiManager.setHostname(HOSTNAME);
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setWiFiAutoReconnect(true);
  wifiManager.autoConnect(HOSTNAME, "motdepasse");
  Serial.begin(115200);
  WebSerial.begin(&server);
  WebSerial.msgCallback(msgCallback);
  WebSerial.println("Connected to WiFi");
  server.begin();
  WebSerial.println("Server started");
  // dht.begin();
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    float t = dht.readTemperature();
  request->send(200, "text/plain", String(t));
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    float h = dht.readHumidity();
  request->send(200, "text/plain", String(h));
  });
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    request->send(200, "text/plain", "OK");
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    request->send(200, "text/plain", "OK");
  ESP.reset();
  });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    request->send(SPIFFS, "/config.html", "text/html");
  });
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest* request)
  {
    // get ip in body of request
    String ip = request->getParam("ip", true)->value();
  hub_ip = ip.c_str();
  strcpy(serverUrl, "http://");
  strcat(serverUrl, hub_ip);
  strcat(serverUrl, "/api/sensor");
  // save ip in file
  File f = SPIFFS.open("/ip.txt", "w");
  f.println(ip);
  f.close();
  // send response
  Serial.println("IP saved");
  Serial.println(ip);
  request->send(200, "text/plain", "OK"); });
  server.on("/files", HTTP_GET, [](AsyncWebServerRequest* request)
  {
    String html = "<html><head><title>OpenThermostat Sensor</title></head><body><h1>OpenThermostat Sensor</h1><ul>";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    html += "<li><a href='";
    html += dir.fileName();
    html += "'>";
    html += dir.fileName();
    html += "</a></li>";
  }
  html += "</ul></body></html>";
  request->send(200, "text/html", html);
  });
  server.onNotFound([](AsyncWebServerRequest* request)
  {
    // search if file exists
    if (SPIFFS.exists(request->url())) {
      request->send(SPIFFS, request->url(), "text/html");
    }
    else {
      request->send(404, "text/plain", "Not found");
    }
  });
}

void loop()
{
  if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
  {
    WebSerial.println("Sending data to hub");
    // get temperature and humidity
    int temp = dht.readTemperature();
    int hum = dht.readHumidity();
    // send data to server with POST
    HTTPClient http;
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"temperature\": " + String(temp) + ", \"humidity\": " + String(hum) + "}";
    int httpCode = http.POST(payload);
    WebSerial.println(httpCode);
    http.end();

    // light sleep for 10 minutes
    wifi_set_sleep_type(LIGHT_SLEEP_T);
    delay(REFRESH_DELAY);
    // wake up
    wifi_set_sleep_type(NONE_SLEEP_T);
  }
}