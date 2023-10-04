#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

String readString;
const char* ssid = "Tricolor";
const char* password = "trimundial";

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
String GAS_ID = "AKfycbw8ulo008Rkph4iG8sJb2CnCpWOTiKM8nibQJ_SXt0DzpD5V_eVlAbFZMQbXUBWRhNMIQ";  // Replace by your GAS service id


void setup()
{
           Serial.begin(9600);
              WiFi.mode(WIFI_STA);
              WiFi.begin(ssid, password);
             
                              while (WiFi.status() != WL_CONNECTED) {
                                delay(500);
                                Serial.print(".");
                              }
          
              //sendData(113,125);  // Send test data

              pinMode(trigP, OUTPUT);
              pinMode(echoP, INPUT);
              startMillis = millis();
}

void loop()
{
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    digitalWrite(trigP, LOW);   // Makes trigPin low
    delayMicroseconds(2);       // 2 micro second delay 
    
    digitalWrite(trigP, HIGH);  // tigPin high
    delayMicroseconds(10);      // trigPin high for 10 micro seconds
    digitalWrite(trigP, LOW);   // trigPin low
    
    duration = pulseIn(echoP, HIGH);   //Read echo pin, time in microseconds
    distance= duration*0.034/2;        //Calculating actual/real distance
    
    Serial.print("Distance = " + distance);        //Output distance on arduino serial monitor 
    Serial.println(distance);
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.

  }

  int a,b;                       //a,b are variables to store sensor values
    a=distance;                         
    //b=distance;
    sendData(a);               //This function uploads data to Google Sheets
        
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

  /*
  if (client.verify(fingerprint, host)) {
  Serial.println("certificate matches");
  } else {
  Serial.println("certificate doesn't match");
  }
  */
  String string_x     =  String(x, DEC);
  //String string_y     =  String(y, DEC);
  String url = "/macros/s/" + GAS_ID + "/exec?Distance=" + string_x;
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