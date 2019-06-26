#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>

const char* DOORTOPIC = "college/office/doorlock" ;


#define WIFI_SSID  "Astral Presence"
#define WIFI_PASSWORD  "AstralDev108"

//#define WIFI_SSID "crazybee"
//#define WIFI_PASSWORD "12345678"


#define MQTT_HOST IPAddress(192,168,1,110)

//#define MQTT_HOST "www.mqtt-dashboard.com"
#define MQTT_PORT 1883



#define H 13//Hall sensor 
#define B 12 //Button Input
#define Bz 14 //Buzzer 
#define L 4 //Lock
#define LED 5

enum State_enum {POWER_ON, S0 , S1 , S2, S3, BB};

void state_machine_run(int);
int read_input();
uint8_t state = S0;
boolean BREAKINFLAG = false;
int valB;
int valH;
int valH1;
int valM=0;
int valI=0;
int msgValue;
int l;
int bz;


unsigned long tstateBB = 0;
unsigned long tstate2 = 0;
unsigned long tstate3 = 0;
unsigned long now;




AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;



void connectToWifi() {                        
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  digitalWrite(LED,HIGH);
  connectToMqtt();  
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  digitalWrite(LED,LOW);
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
} 

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason){
  Serial.println("Disconnected from MQTT.");
 if (WiFi.isConnected()){
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {




const size_t capacity = JSON_OBJECT_SIZE(1) + 15;
DynamicJsonBuffer jsonBuffer(capacity);
JsonObject& root = jsonBuffer.parseObject(payload);
String value = root["door"].as<String>();
Serial.println(value);
 if (value=="open")  {
Serial.println("openrequest");
      if(BREAKINFLAG){
        state = POWER_ON;
        powerOn();
        BREAKINFLAG = false;
        }
    
    valM = 1;
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


  Serial.begin(115200);
  delay(10);

  pinMode(H, INPUT);
  pinMode(B, INPUT);
  pinMode(Bz, OUTPUT);
  pinMode(L, OUTPUT);
  pinMode(LED, OUTPUT);
state = POWER_ON;
powerOn();
now = millis();
 
}

void loop() 
{

  if(!BREAKINFLAG){
  if(millis()-now>1000){
    Serial.println("status sent");
    now = millis();
     sendStatus();
    }



    
    }
    
// read_input();
 
 state_machine_run(read_input());
  delay(100);




}

void state_machine_run(int input_state)
{
  switch (state)
  {



    case POWER_ON:
    
       Serial.println("power on state");

        if(input_state == 2){
          state_Szero(); 
          state = S0;
          
          }
        

    break; 

      
    case S0:
   Serial.println("state  0");
      if (input_state == 0 || input_state == 1) { 
        tstate2 = millis();
        state_Stwo();

        state = S2;
      }
      else if (input_state == 2) {              
        state_Szero();                          
        state = S0;
      }
      else if (input_state == 3) {             
        state_Sone();                           
        state = S1;
      }
      break;

    case S1:
     Serial.println("state  1");
      if(!BREAKINFLAG){
       mqttClient.publish(DOORTOPIC, 0, false, "{\"door\":\"break-In\"}");
      
      Serial.println("BREAK-IN");

      
      BREAKINFLAG = true;
    }  
      break;

    case S2:

      Serial.println("state 2");


      if (input_state == 0 || input_state == 1) { 
        state_Stwo();
        state = S2;
      } else if (millis() > tstate2 + 2000) {
        state_Sthree();
        state = S3;
        tstate3 = millis();
      }

      break;

    case S3:

     Serial.println(" state 3 ");


      if (input_state == 0 || input_state == 1) { 
        state_Stwo();
        state = S2;
        }
      else if (input_state == 2) { 
        state_Szero();
        state = S0;
        }
        else if (millis() > tstate3 + 4000) {
        buzzerOn();
        tstateBB = millis();
        state = BB;
      }
      
      break;

    case BB:
      Serial.println("State BB");
      if (input_state == 2) { 
        state_Szero();
        state = S0;
      }
      else if (millis() > tstateBB + 2000) {
        buzzOff();
      }
      break;
  }
}


void state_Szero()
{
  digitalWrite(L, HIGH); 
  digitalWrite(Bz, LOW);
  
}

void state_Sone()
{
  digitalWrite(L, HIGH); 
  digitalWrite(Bz, HIGH);
  
}

void state_Stwo()
{
digitalWrite(L, LOW);
digitalWrite(Bz, LOW);
}

void state_Sthree()
{
  digitalWrite(L,HIGH);
  digitalWrite(Bz, LOW);
  
}


void powerOn(){
  
  digitalWrite(Bz, LOW);
   digitalWrite(L,  HIGH);
  }
void buzzerOn()
{
digitalWrite(Bz, HIGH);
}

void buzzOff() {
digitalWrite(Bz, LOW);
}


int read_input()
{
  valB = digitalRead(B);
  valH = digitalRead(H);
  
 if (valB == 1 || valM == 1 )
  {
    valM = valM ? 0 : 1;
    valI = 1;
  }
  else
    valI = 0;


  if ( valI == 1 && valH == 0) {
    return 0; 
  }
  else if ( valI == 1 && valH == 1) {
    return 1; 
  }
  else if ( valI == 0 && valH == 0) {
    return 2;
  }
  else if ( valI == 0 && valH == 1) {
    return 3; 
  }

}









void sendStatus(){
  
int h = digitalRead(H);
int l = digitalRead(L);


if(h || !l){  
    mqttClient.publish(DOORTOPIC, 0, false, "{\"door\":\"opened\"}");                
  
  
  }
  else if(!h && l ){
  
    mqttClient.publish(DOORTOPIC, 0, false, "{\"door\":\"closed\"}");
   
    } 
}
