#define BLYNK_TEMPLATE_ID "blynk template id"
#define BLYNK_TEMPLATE_NAME "blynk template name"
#define BLYNK_AUTH_TOKEN "blynk auth token"
#define BLYNK_PRINT Serial

#include <WiFi.h> //Library WiFi
#include <WiFiClient.h> //Library WiFiClient
#include <BlynkSimpleEsp32.h> //Library BlynkESP32
#include <HTTPClient.h>

int detectorPin = 19;   
int val, confirm; 
String serverName = "";
BlynkTimer timer;

char ssid[] = ""; //Nama WiFi yang digunakan
char pass[] = ""; //Password WiFi yang digunakan

void setup() { 
  Serial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(detectorPin, INPUT); 
}

void loop() { 
  Blynk.run();
  val = digitalRead(detectorPin); 

  Serial.println(val); 
  Blynk.virtualWrite(V0, val);
  if (val == LOW) {

    //ada object nyala lampunya
    digitalWrite(LED_BUILTIN, HIGH);  
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      http.begin(serverName.c_str());
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.printf("Fetching image from ESP32-cam result: %d\n", httpResponseCode);
        if (httpResponseCode == 200) {
          Serial.print("Successfully fetched image, sending it to another server\n");
        }
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }

  }else{  
    //tidak ada object mati lampunya
    digitalWrite(LED_BUILTIN, LOW); 
  }  
  delay(1000); 
}
