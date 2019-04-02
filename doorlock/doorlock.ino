#include <ESP8266WiFi.h>
#include <PubSubClient.h>





const char* ssid = "Astral Presence";
const char* password = "AstralDev108";



const char* mqtt_server = "192.168.1.101";

const char*  doorTopic = "college/office/doorlock" ;



WiFiClient espClient;
PubSubClient client(espClient);






enum State_enum {POWER_ON, S0 , S1 , S2, S3, BB};


void state_machine_run(int);
int read_input();
uint8_t state = S0;
boolean FLAG = false;
int valB;
int valH;
int valH1;
int valM=0;
int valI=1;
int msgValue;
int l;
int bz;


unsigned long tstateBB = 0;
unsigned long tstate2 = 0;
unsigned long tstate3 = 0;
unsigned long now;


#define H 13//Hall sensor 
#define B 12 //Button Input

#define Bz 14 //Buzzer 
#define L 4 //Lock



void setup() {
 
  Serial.begin(115200);
  delay(10);

  pinMode(H, INPUT);
  pinMode(B, INPUT_PULLUP);
  pinMode(Bz, OUTPUT);
  pinMode(L, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 state = POWER_ON;

now = millis();
 
}

void loop() 
{

  if(!FLAG){
  if(millis()-now>1000){
    Serial.println("status sent");
    now = millis();
     sendStatus();
    }



    
    }
  
 
 if( !client.connected())
 {

reconnect();
  }
 
  
  
  client.loop();

  state_machine_run(read_input());
  delay(10);




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
      if(!FLAG){
      
      client.publish(doorTopic, "{\"door\":\"break-In\"}"  );
      Serial.println("BREAK-IN");

      
      FLAG = true;
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
  digitalWrite(L, LOW); 
  digitalWrite(Bz, LOW);
  
}

void state_Sone()
{
  digitalWrite(L, LOW); 
  digitalWrite(Bz, HIGH);
  
}

void state_Stwo()
{
digitalWrite(L, HIGH);
digitalWrite(Bz, LOW);
}

void state_Sthree()
{
  digitalWrite(L, LOW);
  digitalWrite(Bz, LOW);
  
}


void powerOn(){
  
  digitalWrite(Bz, LOW);
   digitalWrite(L, LOW);
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
  
 
 if (valB == 0 || valM == 1 )
  {
    valM = valM ? 0 : 1;
    valI = 0;
  }
  else
    valI = 1;


  if ( valI == 0 && valH == 0) {
    return 0; 
  }
  else if ( valI == 0 && valH == 1) {
    return 1; 
  }
  else if ( valI == 1 && valH == 0) {
    return 2;
  }
  else if ( valI == 1 && valH == 1) {
    return 3; 
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
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());



}


void callback(char* topic, byte* payload, unsigned int length) {
  
  
  
  char message[length];
  for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
       }
  message[length] = '\0';
  String  Message = String(message);   
  
 String Topic  = topic;

Serial.println(Message);

  if (Message == "{\"door\":\"open\"}") {

      if(FLAG){
        state = POWER_ON;
        powerOn();
        FLAG = false;
        }
    
    valM = 1;
  }

  

}



void  reconnect() {
   Serial.println(" MQTT - connecting ");
  if (client.connect("doorClient")) {

    client.subscribe(doorTopic);
  }
 
}




void sendStatus(){
  
int h = digitalRead(H);
int l = digitalRead(L);


if(h || l){                   
  client.publish(doorTopic, "{\"door\":\"opened\"}" );
  
  }
  else if(!h && !l ){
  
    
    client.publish(doorTopic, "{\"door\":\"closed\"}" );
    } 
}




