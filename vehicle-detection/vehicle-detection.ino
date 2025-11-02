#define BLYNK_TEMPLATE_ID "TMPL6X8pJ2-G6"
#define BLYNK_TEMPLATE_NAME "smart parking system"
#define BLYNK_AUTH_TOKEN "Y5sgParZSPUZmPhWkRDWHpEfwTmuZfB4"
#define BLYNK_PRINT Serial

#include <WiFi.h> //Library WiFi
#include <WebServer.h> //Library WiFiClient
#include <WiFiClient.h> //Library WiFiClient
#include <BlynkSimpleEsp32.h> //Library BlynkESP32
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>  

static const int servoPin = 27;

Servo servo1;

int servoAngle = 90;
int detectorPin = 14;   
int val; 
String aiServer = "http://10.67.117.21:8000/detect";
String beServer = "http://10.67.117.90:3000/v1/parking/";


char ssid[] = "Aleknet"; //Nama WiFi yang digunakan
char pass[] = "12345678"; //Password WiFi yang digunakan

WebServer server(80);
bool moveRequested = false;

void handleSet() {
  if (server.hasArg("angle")) {
    int angle = constrain(server.arg("angle").toInt(), 0, 180);

    // servo1.write(angle);
    servoAngle = angle;
    moveRequested = true;
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"error\"}");
  }
}

void handleStatus() {
  String json = "{\"angle\":" + String(servoAngle) + "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "application/json", json);
}

void setup() { 
  Serial.begin(115200); 
  pinMode(LED_BUILTIN, OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(detectorPin, INPUT); 
  
  ESP32PWM::allocateTimer(0);
  servo1.setPeriodHertz(50); 
  servo1.attach(servoPin, 500, 2400);

  servo1.write(servoAngle);

  server.on("/set", handleSet);
  server.on("/status", handleStatus);
  server.begin();
}


void loop() { 

  server.handleClient(); 

  if (moveRequested) {
    servo1.write(servoAngle);
    moveRequested = false;
  }

  static unsigned long last = 0;
  if (millis() - last > 1000) {
    val = digitalRead(detectorPin); 
    Serial.println(val); 
    if (val == LOW) {

      //ada object nyala lampunya
      digitalWrite(LED_BUILTIN, HIGH);  

    if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;
        http.begin(aiServer.c_str());
        http.setTimeout(50000); 
        http.addHeader("Content-Length", String(1000));
        int httpResponseCode = http.POST("");
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode); // -11
        
        if (httpResponseCode>0) {
          if (httpResponseCode == 200) {
            Serial.print("Successfully fetched image, sending it to another server\n");
            DynamicJsonDocument doc(512);
            String response = http.getString();
            DeserializationError error = deserializeJson(doc, response);

            if (error) {
              Serial.print("Failed to parse JSON: ");
              Serial.println(error.c_str());
            }else {
              if (doc.containsKey("plate")) {
                String plate = doc["plate"].as<String>();
                Serial.print("Plate: ");
                Serial.println(plate);
                // servo1.write(0);
                servoAngle= 0;
                moveRequested = true;
                // registerParking(response);
                
              } else {
                Serial.println("Key 'plate' not found in response!");
              }
            }

          }
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
          // Serial.println(http.errorToString());
        }
        http.end();
      }

    }else{  
      //tidak ada object mati lampunya
      digitalWrite(LED_BUILTIN, LOW); 
    } 
    last = millis();
  }

}
