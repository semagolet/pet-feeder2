/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <OneWire.h>

#include <Servo.h>
/************************* WiFi Access Point *********************************/
Servo myservo;

const char* ssid = "sema22";
const char* password = "sema1234";
const char* mqtt_server = "192.168.139.136";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg =0;
int value=0;
#define trig D6
#define echo D5
#define relay D7
long duration, inches, cm;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char msg1[MSG_BUFFER_SIZE];
void setup() 
{
  Serial.begin(115200);
  myservo.attach(2);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //Serial.println(F("Adafruit MQTT demo"));


 
}
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }  
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String string;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");

  for (int i=0; i<length; i++)
  {
    string+=((char)payload[i]);
  }

  Serial.print(string);


  if ( (String)topic == "servo")
  {
    Serial.print(" ");
    int resultado = string.toInt();
    //int pos = map(resultado,1,100,0,180);
    
    myservo.write(resultado);
    delay(100);
  }
    
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    if(client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      client.subscribe("servo");
    } 
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {

   if (!client.connected())
   {
    reconnect();
   }

  //client.publish("servo1", "merhba");

   client.loop();

   delay(100);
    
  digitalWrite(trig, LOW);
  delayMicroseconds(4);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long t = pulseIn(echo, HIGH);
  long cm = t / 29 / 2;
  Serial.println(cm);
  snprintf (msg, MSG_BUFFER_SIZE, "%ld", cm);
  client.publish("uzaklik", msg);
  long level= 25-cm; // 25 is changing according to depth
  if (level<0)
  level=0;
  Serial.println("Mama seviyesi");
  Serial.println(level); 
  snprintf (msg1, MSG_BUFFER_SIZE, "%ld", level);
  client.publish("level", msg1);
}
