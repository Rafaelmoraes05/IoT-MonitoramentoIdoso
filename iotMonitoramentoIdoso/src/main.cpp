#include <pgmspace.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h> 
#include "WiFi.h"

// MQTT topics for the device
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

//mod
#define THINGNAME "ESP32-IoT-MonitoramentoIdoso"
const char WIFI_SSID[] = "iPhone de Rafael";
const char WIFI_PASSWORD[] = "rafael8765";
const char AWS_IOT_ENDPOINT[] = "a34ypir054ngjb-ats.iot.us-east-2.amazonaws.com";

WiFiClientSecure wifi_client = WiFiClientSecure();
MQTTClient mqtt_client = MQTTClient(256); //256 indicates the maximum size for packets being published and received.

//mod
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAOxh8KClZBqmQtlfEGTadto9mPQ5MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzExMjcxNzM3
MzNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDej2wSQbtNBS2grQhk
chZfr978RMkQmQSMoIWvCq3xxNBMdj2w4snO+EqzMHvl2F0KzQDtmILAB/d2M8I1
J9XcR0QJNAenP3TWVhDQWqGJvvWftjqNlW1eem3IXAISbwoIG3LfhsZBfrTT/r/0
9hSHzrb78reKlp402oIfGLLlC7e61KMT6AzBGiyHGn6t4oypz1fbmTZCx6rhjn0d
VRzHnK99/+IxEGbnrfnQ0CW+53M8ga/7NPelJvWI0rYloJf8a2OoWEf8e85rUBkg
2SglgUgNX1y7bmAAD1eMXbeb5FCJY1LNK9Caa/gn4kJ/upIjq44seEyVSob38dH2
pwxzAgMBAAGjYDBeMB8GA1UdIwQYMBaAFMk6parZB/ITAtrApqOcLopXvojvMB0G
A1UdDgQWBBSLZ3k8P7gBQOg9egWXbo+MYDaeBjAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAnh9W1SaUb+l3bK9S8Jy8k2Ir
31zKBlkUefxLa7ptFuyQmYgMGwk6ttLE/c1xg4GHeiybakiUAD4JxpHel0MBpq+/
C7yJVhtnCxWIMWaiIf6vPin/0oUaAH2PqO9RU9wIUFp0GgjSlpSoVQ67H1DiEIUM
7aCUNvfjmyYx+46AUpRA/rVIe0JWqh1QcrhTVsJ/7SricEgAH9lJTbwPp6D+K4ml
ZktFN1m4AqlNL4nul4Qflydp+2LjmON+SrbXzysI2uM5WeZvCDZJFa6Jx2a4wwb2
zJNjYSkx/lSm/8A1REI/U2ffxC+9nYL7n+skKbE9spb0IIYYJLZnJ0armirRPA==
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA3o9sEkG7TQUtoK0IZHIWX6/e/ETJEJkEjKCFrwqt8cTQTHY9
sOLJzvhKszB75dhdCs0A7ZiCwAf3djPCNSfV3EdECTQHpz901lYQ0Fqhib71n7Y6
jZVtXnptyFwCEm8KCBty34bGQX600/6/9PYUh862+/K3ipaeNNqCHxiy5Qu3utSj
E+gMwRoshxp+reKMqc9X25k2Qseq4Y59HVUcx5yvff/iMRBm56350NAlvudzPIGv
+zT3pSb1iNK2JaCX/GtjqFhH/HvOa1AZINkoJYFIDV9cu25gAA9XjF23m+RQiWNS
zSvQmmv4J+JCf7qSI6uOLHhMlUqG9/HR9qcMcwIDAQABAoIBAQC7ouV1Wu+jlssK
HPfnX3xXffoca42sraV1PlyW4AUYwVrwK/wE5p9IHHM2MZkzhb+jvISczCjn54+N
m0OP9x7J0Ved/ry5F7nUfsk8MDzi2hlRZpxlA+cqcUOfWVrx2aX2gdXOeF0rmLyx
wqRpzRIwEWqabd6qPCPz4Yk02GjyyuPPWh/4h13U9he7oTxvuO4qJV5yjpbmP78t
Pb7qCvXttM9ljbI5cfps8qBYP9H9ctwxLuOGIuTOCnjoGQm/FamAbglZm+leQVrD
0iDfx7rI5GfWCUiN/EU/fynuN4SJvKLLhNuT7NtVtZk+JppUaXHCgJHE16b7SCUo
zX3Yk4yRAoGBAP/RhxemuCNCXApe5P74RDQMgp1ieXJdQMYAzzhNGm8G74fAotX3
E5tAi8S1lL/oZgZX6ze3IsxVRgD64JEze6/w86xJcmC0Hd/ClSlm60TQcJxrHET9
BTHGabFwyaBoYUsPXoUVRhNRmy6fSZ/HVdGZimqschcj2hYMv6fYkfdbAoGBAN63
2kvN+BzGMS/xNUDEZZgd/8p0GMtzEuYrr3bQ89wiAga8ScK5aOKCSjf788pDEUn1
SRr9Uv9mB2gfiURme5UMnTuLaIc9WILjqSot+Gve6rPOCBvk2CZo9bDLeiG7O3aZ
1jdKzSK+to8JGYLq7xTDQXRHUiAGJGmOiG18AWLJAoGBAO4J08CoO7Njbi2gO0kd
Tql38pnsJcHLEBiubCOhazVgqMYS8/JQ5AeUufmiVPV1lGnWqWLAoDquJmOHDitO
UotWlAb+mNVvBFVPxGgwAj+YDREfgcCYB5tXWsXng1YlilLDzFndf8U+LD3nriRk
4XAVhPzZ1Ze+npHh7vyJwtj9AoGBAJwQ8WNVPZvMNgOGXu63sQlbXDceC5HlXUT1
L8tuuLN1ET5vCrDtA9ZVvnRklX0SajcNLuE+yr41XNNV5faMaeUUJcnn8qNyFWgC
6DOqav4MAyNkKMKDJgQkYKY2r6qbbRu6ySWNwRmr0nmAwuO+hTJjcKZuo9nU9AcT
3f36e24hAoGAGnBZ2s1iuxFHgNo+NrkN7WQH46zx71DyIuJM2N+W2BzvONcC19Ke
gxXoSvHsTmpF7gjcSqO2l+0SlPwnrLa+QagQIbOq+Io0yjgISzzUXUvWVimPQCR4
yo0B7osMH0f9ffJigbiwleDglsbnlGdBmyIswFzdRN1AdlA2NKdHF0k=
-----END RSA PRIVATE KEY-----
)KEY";
//endmod

uint32_t t1;

void incomingMessageHandler(String &topic, String &payload) {
  Serial.println("Message received!");
  Serial.println("Topic: " + topic);
  Serial.println("Payload: " + payload);
}

void connectAWS()
{
  //Begin WiFi in station mode
  WiFi.mode(WIFI_STA); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  //Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  // Configure wifi_client with the correct certificates and keys
  wifi_client.setCACert(AWS_CERT_CA);
  wifi_client.setCertificate(AWS_CERT_CRT);
  wifi_client.setPrivateKey(AWS_CERT_PRIVATE);

  //Connect to AWS IOT Broker. 8883 is the port used for MQTT
  mqtt_client.begin(AWS_IOT_ENDPOINT, 8883, wifi_client);

  //Set action to be taken on incoming messages
  mqtt_client.onMessage(incomingMessageHandler);
    // Create a message handler
  // mqtt_client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IOT");

  //Wait for connection to AWS IoT
  while (!mqtt_client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.println(WiFi.localIP());

  if(!mqtt_client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  //Subscribe to a topic
  mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  //Create a JSON document of size 200 bytes, and populate it
  //See https://arduinojson.org/
  StaticJsonDocument<200> doc;
  doc["elapsed_time"] = millis() - t1;
  doc["value"] = random(1000);
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to mqtt_client
  //Publish to the topic
  mqtt_client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("testando");
}

// void MessageHandler(String &topic, String &payload) {
//   Serial.println("Message received!");
//   Serial.println("Topic: " + topic);
//   Serial.println("Payload: " + payload);
// }

void setup() {
  Serial.begin(9600);
  t1 = millis();
  connectAWS();

}

void loop() {
  if (WiFi.status() != WL_CONNECTED){
    connectAWS();
  }
  
  Serial.println("Start");
  Serial.println(WiFi.localIP());
  publishMessage();
  mqtt_client.loop();
  delay(4000);
}