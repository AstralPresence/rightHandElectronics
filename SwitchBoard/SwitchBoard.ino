


#include "CAP1114.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* fanTopic = "college/office/fan";
const char* lightTopic = "college/office/light";

const char* ssid = "Astral Presence";
const char* password = "AstralDev108";


const char* mqtt_server = "192.168.1.101";

int  Speed_temp = 0;


WiFiClient espClient;
PubSubClient client(espClient);




int count=0;

#define FIRE 6
#define ZC 5

#define RELAY1 3
#define RELAY2 4
#define MAXDELAY 7000

int x,timenow,Delay;
Capsense cap;

void setup()
{

delay(3000);
  Serial.begin(115200);
  Serial.println();  
    cap.Init();
  
  
  cap.setSensitivity(4);
  Serial.println(cap.readSensitivity());
  
 cap.writeRegister(0x31,0x20);//2
 cap.writeRegister(0x32,0xFF);//3
 cap.writeRegister(0x33,0xFF);//4
 cap.writeRegister(0x34,0x20);//5
 
 cap.writeRegister(0x38,0xFF);
 cap.writeRegister(0x39,0xFF); 
 
  
  
  cap.setDirection(ZC,INPUT);



  setup_wifi();
  
  client.setServer(mqtt_server , 1883);
  client.setCallback(callback);
 

  
  
   
  
}

void loop()
{

if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  
if(cap.isSet(RELAY2)){
 fan(Delay);
}

 
if( millis()-timenow > 1000){
  
 x  = cap.readCapTouch();
 Serial.println(x);



   if(x==2){

      if(cap.isSet(RELAY1)){
      cap.setState(RELAY1, LOW);
      client.publish(lightTopic, "{\"state\":0}");}
      else{
      cap.setState(RELAY1,HIGH);
      client.publish(lightTopic, "{\"state\":1}");}

      
}

   if(x==5){

      if(cap.isSet(RELAY2)){
      
      cap.setState(RELAY2, LOW);
      client.publish(fanTopic, "{\"state\":0}");}
      
      else{
      cap.setState(RELAY2,HIGH);

       char msg[20];
      sprintf(msg,"%s:%i}","{\"state\"",1);
      Serial.println(msg);
      client.publish(fanTopic, msg);
}

   }

   
  timenow = millis();

  
  }

}

void fan(int Delay){
   // Serial.println(Delay);
  x  = cap.readState(ZC);
  if(Delay==7000){
     cap.setState(RELAY2,LOW);
    cap.setState(FIRE,LOW);
    }
  else if(x){
    
    delayMicroseconds(Delay);
    cap.setState(FIRE,HIGH);
    delayMicroseconds(10);
    cap.setState(FIRE,LOW);
    
    }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {
  
  
  Serial.println(topic);
  char message[length];
  for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
       }
  message[length] = '\0';
  String  Message = String(message);   
  Serial.println(Message);
 String Topic  = topic;


 if (Topic == lightTopic){
  
  if (Message == "{\"state\":0}") {
    cap.setState(RELAY1,LOW);
     Serial.println("L");
         
      }
    else 
      if (Message  == "{\"state\":1}") {
        cap.setState(RELAY1,HIGH);
        Serial.println("H");
      }
    }
  
  if (Topic == fanTopic ) {
  
       String stringSpeed="";

       

        for(int i=0;i<Message.length(); i++){
        if(isDigit(Message[i])){
                stringSpeed+=Message[i];
          }
        }
        Speed_temp  = stringSpeed.toInt();
      if(Speed_temp==0){
          //cap.setState(RELAY2,LOW);
        }
        else cap.setState(RELAY2,HIGH);
        
       Delay  = ((100-Speed_temp)*MAXDELAY)/100 ;
      
 
       
  
  }
  
}


  




boolean reconnect() {
   Serial.println(" MQTT - connecting ");
  if (client.connect("switchboard-Client")) {

    client.subscribe(fanTopic);
    client.subscribe(lightTopic);
  }
  return client.connected();
}


