

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>

const char* TOPIC = "college/office/fan";


#define WIFI_SSID  "Astral Presence"
#define WIFI_PASSWORD  "AstralDev108"

//#define WIFI_SSID "crazybee"
//#define WIFI_PASSWORD "12345678"


#define MQTT_HOST IPAddress(192,168,1,110)

//#define MQTT_HOST "www.mqtt-dashboard.com"
#define MQTT_PORT 1883

#define led1 15
#define led2 2
#define led3 3
#define led4 1
#define plus 5
#define minus 14
#define zcPin 12
#define firePin 4
#define relay 13



#define Step 1125 //microseconds
#define offset 200

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
Ticker sendTimer;

bool countChanged = false; 
int time1,time2;
int count = 0;
int x,y;
int fireTime;
int led[4][9]= {
              {0,150,1024,1024,1024,1024,1024,1024,1024},
               { 0,0,0,150,1024,1024,1024,1024,1024},
               { 1024,1024,1024,1024,1024,876,0,0,0},
                {1024,1024,1024,1024,1024,1024,1024,876,0}
            };




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
  mqttClient.subscribe(TOPIC, 0);
} 

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason){
  Serial.println("Disconnected from MQTT.");
 if (WiFi.isConnected()){
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {


const size_t capacity = JSON_OBJECT_SIZE(1) + 20;
DynamicJsonBuffer jsonBuffer(capacity);
JsonObject& root = jsonBuffer.parseObject(payload);
int speed = root["state"].as<int>();
 


  
if(speed==0){count=0  ;}
else if(speed == 100){count = 8;} 
else
 count = (speed/12.5) + 1;

 setLed(count);
 setspeed(count);
 }


void setup() {
 
 wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();


  pinMode(plus,INPUT);
  pinMode(minus,INPUT);
  pinMode(firePin,OUTPUT);
   pinMode(relay,OUTPUT);
   setLed(0);
   OFF();
   time1 = millis();
   time2 = millis();


}

void loop() {


if(millis()-time1>150){ 
x = digitalRead(plus);
y = digitalRead(minus);

if(x){
  
   if(count<8){
  count = count + 1;
sendTimer.detach();
sendTimer.once_ms(200,sendSpeed);
  
  setLed(count);
  setspeed(count);
  
   }
  }else if(y){
    if(count>0){
    count = count-1;
  sendTimer.detach();
   sendTimer.once_ms(200,sendSpeed); 
    setLed(count);
    setspeed(count);
    
    }
    }
  time1= millis();
 }


  
  
 if(count!=0){ 
if(digitalRead(zcPin)){
pulse();
}}

}


void pulse(){
 
  delayMicroseconds(offset+fireTime);
  digitalWrite(firePin, HIGH);
  delayMicroseconds(80);
  digitalWrite(firePin,LOW);
 
 }


void setLed(int count){
    analogWrite(led1,led[0][count]);
    analogWrite(led2,led[1][count]);
    analogWrite(led3,led[2][count]);
    analogWrite(led4,led[3][count]);
 }

void setspeed(int count){
  if(count==0){
    OFF();
    } else {
      ON();
  fireTime = int((-count*8000+71000)/7);
  Serial.println(fireTime);
  }
  
  }


  
void OFF(){
   digitalWrite(relay,HIGH);
  }

  void ON(){
    
    digitalWrite(relay,LOW);
    
    }

    void sendSpeed(){
      char msg[20];
     sprintf(msg,"%s:%i}","{\"state\"",count*12);
       mqttClient.publish(TOPIC, 0, false, msg);
      
      }
