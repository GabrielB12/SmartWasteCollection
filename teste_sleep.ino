#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "";
const char* password = "";

const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbxXEX7iS29mDDwQF-J3IiuUNC5p-tr7yklIAaf7eqgYUcOcKOkS_QDPhhNCD1eiQV9ZAw";

const int trigP = 2;
const int echoP = 0;

WiFiClientSecure client;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT);

  // Medição do sensor
  digitalWrite(trigP, LOW);
  delayMicroseconds(2);
  digitalWrite(trigP, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigP, LOW);

  int duration = pulseIn(echoP, HIGH);
  int distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.println(distance);

  sendData(distance);

  // Coloca em deep sleep por 5 minutos (5 * 60 * 1.000.000 µs)
  ESP.deepSleep(5 * 60 * 1000000);
}

void loop() {
  // nunca entra aqui, porque é usado deepSleep no final do setup()
}

void sendData(int x) {
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  String string_x = String(x, DEC);
  String url = "/macros/s/" + GAS_ID + "/exec?Distance3=" + string_x;

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  client.stop();
}
