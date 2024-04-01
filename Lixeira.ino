#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

String readString;
const char* ssid = "";
const char* password = "";

const char* host = "script.google.com";
const int httpsPort = 443;

//Parte do sensor
const int trigP = 2;
const int echoP = 0;

int duration;
int distance;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 50;


WiFiClientSecure client;


const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbxXEX7iS29mDDwQF-J3IiuUNC5p-tr7yklIAaf7eqgYUcOcKOkS_QDPhhNCD1eiQV9ZAw";  // Replace by your GAS service id


void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT);
  startMillis = millis();
}

void loop()
{
  currentMillis = millis();  
  if (currentMillis - startMillis >= period)  
  {
    digitalWrite(trigP, LOW);   
    delayMicroseconds(2);      
    
    digitalWrite(trigP, HIGH);  
    delayMicroseconds(10);      
    digitalWrite(trigP, LOW);   
    
    duration = pulseIn(echoP, HIGH);   
    distance= duration*0.034/2;        
    
    Serial.print("Distance = " + distance);        
    Serial.println(distance);
    startMillis = currentMillis; 

  }

  int a,b;                      
  a=distance;
  sendData(a);              
        
}

void sendData(int x)
{
  client.setInsecure();
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String string_x     =  String(x, DEC);
  String url = "/macros/s/" + GAS_ID + "/exec?Distance3=" + string_x;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    Serial.println("headers received");
    break;
  }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
  Serial.println("esp8266/Arduino CI successfull!");
  } else {
  Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
