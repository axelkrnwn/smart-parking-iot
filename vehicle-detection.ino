#define BLYNK_TEMPLATE_ID "blynk template id"
#define BLYNK_TEMPLATE_NAME "blynk template name"
#define BLYNK_AUTH_TOKEN "blynk auth token"
#define BLYNK_PRINT Serial

#include <WiFi.h> //Library WiFi
#include <WiFiClient.h> //Library WiFiClient
#include <BlynkSimpleEsp32.h> //Library BlynkESP32

int detectorPin = 19; 
int val; 

BlynkTimer timer;

char ssid[] = ""; //Nama WiFi yang digunakan
char pass[] = ""; //Password WiFi yang digunakan


void myTimerEvent() {
  val = digitalRead(detectorPin); 
  Serial.println(val); 
  Blynk.virtualWrite(V0, val);

  if (val == LOW) {
    Blynk.virtualWrite(V1, 1); 
    digitalWrite(LED_BUILTIN, HIGH);  
  }else{
    Blynk.virtualWrite(V1, 0);   
    digitalWrite(LED_BUILTIN, LOW); 
  }
  delay(1000);
}

void setup() { 
  Serial.begin(9600); 
  pinMode(LED_BUILTIN, OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(detectorPin, INPUT); 
  timer.setInterval(1000L, myTimerEvent);
}

void loop() { 
  Blynk.run();
  timer.run();

}
