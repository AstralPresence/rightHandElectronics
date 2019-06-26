#include "Arduino.h"
#include "PCF8574.h"

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>

PCF8574 pcf8574(0x20);

const char* LIGHTTOPIC = "college/office/light";
const char* DOORTOPIC  = "college/office/doorlock";

#define WIFI_SSID  "Astral Presence"
#define WIFI_PASSWORD  "AstralDev108"
//
//#define WIFI_SSID "crazybee"
//#define WIFI_PASSWORD "12345678"

#define MQTT_HOST IPAddress(192, 168, 1, 110) 
//#define MQTT_HOST "WWW.mqtt-dashboard.com"
#define MQTT_PORT 1883

#define LIGHT 14
#define DOORLOCK 13
Ticker flipper;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;


#define ON HIGH
#define OFF LOW

bool lightState = false;
bool doorState = false;

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

void onMqttConnect(bool sessionPresent){
  Serial.println("Connected to MQTT.");
  mqttClient.subscribe(DOORTOPIC, 0);
   mqttClient.subscribe(LIGHTTOPIC, 0);
} 

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason){
  Serial.println("Disconnected from MQTT.");
 if (WiFi.isConnected()){
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}


void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
 
  
if(!strcmp(topic,LIGHTTOPIC)){
  Serial.println(topic);
  Serial.println(payload);
const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
DynamicJsonBuffer jsonBuffer(capacity);
JsonObject& root = jsonBuffer.parseObject(payload);
int value = root["state"].as<int>();

Serial.println(value);
lightState  = value?true:false;
 
}

else if(!strcmp(topic,DOORTOPIC)){
  Serial.println(topic);
  Serial.println(payload);
const size_t capacity = JSON_OBJECT_SIZE(1) + 20;
DynamicJsonBuffer jsonBuffer(capacity);
JsonObject& root = jsonBuffer.parseObject(payload);
String message  = root["door"].as<String>();


if(message=="opened")
{
  doorState=true;
  
  }
  else if(message=="closed"){
    doorState = false;
    
    }
  
}



}


void setup() {

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
    pinMode(LIGHT, OUTPUT);
    pinMode(DOORLOCK, OUTPUT);

  flipper.attach_ms(150, scan);
   Serial.begin(115200);
}

void loop() {

updateState(LIGHT,lightState);
updateState(DOORLOCK,doorState);
}

void scan(){

pinMode(LIGHT, INPUT);

int x = digitalRead(LIGHT);

  if(x){
    Serial.println("light pressed");
    if(lightState==false){
     lightState=true;
    
      mqttClient.publish(LIGHTTOPIC, 0, false, "{\"state\":1}");
      
      }
    else {
      lightState = false;
    
     mqttClient.publish(LIGHTTOPIC, 0, false, "{\"state\":0}");
     }
    
    }
 
  



pinMode(DOORLOCK, INPUT);
int y = digitalRead(DOORLOCK);
if(y){
   Serial.println("door pressed");
   
   mqttClient.publish(DOORTOPIC, 0, false, "{\"door\":\"open\"}"  );
   }
 
 
}


  void updateState(int pin,bool state){
    pinMode(pin, OUTPUT);
    if(pin==LIGHT){
      digitalWrite(pin, state);
      } 
      else if(pin==DOORLOCK){
        digitalWrite(pin, state);
         
        }
          
    
    }
