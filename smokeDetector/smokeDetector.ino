#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "DHT.h"
#include <ArduinoJson.h>
const char* TOPIC = "smokeDetector";

#define BUZZER 14
#define LED 12
#define ADC A0

#define DHTPIN 2 
#define DHTTYPE DHT22 

#define load_Res 10    
#define air_factor 9.83  

#define WIFI_SSID "RightHand Network"
#define WIFI_PASSWORD "astralDev108"

#define MQTT_HOST IPAddress(192, 168, 1, 100) 
#define MQTT_PORT 1883

float SmokeCurve[3] ={2.3,0.53,-0.44};
float COCurve[3]    ={2.3,0.72,-0.34}; 
float LPGCurve[3]   ={2.3,0.21,-0.47}; 

float Res=0;  
int tim=0;
DHT dht(DHTPIN, DHTTYPE);

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker dataTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {                        
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
 Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
 dataTimer.attach(2,sendValues);
  mqttClient.subscribe(TOPIC, 0);
 

} 

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason){
  Serial.println("Disconnected from MQTT.");
  dataTimer.detach();
 if (WiFi.isConnected()){
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}




void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
    Serial.print("  message: ");
  Serial.println(payload);
  
}



void setup() {
  
  Serial.begin(115200);
  Serial.println(); 
  Serial.println("calibrating");
  Res = SensorCalibration();
  Serial.println("caliberation done");
  Serial.print("Res : ");
  Serial.print(Res);
  
  pinMode(BUZZER , OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
   dht.begin();
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
}

void loop() {
   tim=millis();
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  delay(500);  Serial.println(millis()-tim);                     // wait for a second
   tim=millis(); 
   digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(500);  
 Serial.println(millis()-tim);   
 
}


void sendValues(){
  char Buffer[150];
   StaticJsonBuffer<200> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();

  root["smoke"] = getPPM(SmokeCurve);
 root["CO"] = getPPM(COCurve);
  root["LPG"]  = getPPM(LPGCurve);
  root["temparature"] = dht.readTemperature();
  root["humidity"] = dht.readHumidity();
   root.prettyPrintTo(Serial);
   root.printTo(Buffer, sizeof(Buffer));
  mqttClient.publish(TOPIC, 0, false, Buffer);
  }

int getPPM(float *pcurve){
   float res=resistance(5,50);
   res/=Res;
  int result=pow(10,(((log(res)-pcurve[1])/pcurve[2]) + pcurve[0]));
   return result;
    }

float resistance(int samples, int interval)
{
   int i;
  float res=0; 
  for (i=0;i<samples;i++) 
  {
     int adc_value=analogRead(ADC);
     res+=((float)load_Res*(1023-adc_value)/adc_value);
    delay(interval);
  }
   res/=samples;
   return res;
}

float SensorCalibration()
{
  int i;
  float val=0;    
  val=resistance(50,500);                
  val = val/air_factor;  
  return val; 
}
    
