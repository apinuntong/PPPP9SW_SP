/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MicroGear.h>
//#include <DHT.h>

#include "CMMC_Blink.hpp"
CMMC_Blink blinker;

const char* ssid     = "ESPERT-3020";
const char* password = "espertap";

String smartphone_key = "5866476670025728";   //  Change your 
String message = "มีคนกดปุ่มที่บ้านย่า"; //  Change your message

#define APPID       "P9"
#define KEY         "JrJt942kkk8znlT"
#define SECRET      "6lA3DROOW6Esx20cNapV6fHSV"
#define ALIAS       "SW_P9"

WiFiClient client;
AuthClient *authclient;
void doHttpGet(String);
//#define DHTPIN 12
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//DHT dht(DHTPIN, DHTTYPE);

int timer = 0;
int relayPin = 15; //control relay pin

MicroGear microgear(client);

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  String msg2 = String((char*)msg);
  if (msg2 == "OFF") {
    digitalWrite(16, HIGH);
    digitalWrite(15, 0);
  }
  if (msg2 == "ON") {
    digitalWrite(16, LOW);
    digitalWrite(15, 1);
  }
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  //on led when Connected to NETPIE
  analogWrite(LED_BUILTIN, 0); //LED_BUILTIN use avctive Low to On
  /* Set the alias of this microgear ALIAS */
  microgear.setName(ALIAS);
}


void setup() {
  /* Add Event listeners */
  /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear);

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  pinMode(16, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //dht.begin();
  blinker.init();
  Serial.begin(115200);
  Serial.println("Starting...");

  blinker.blink(50, LED_BUILTIN);
  delay(200);

  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }




  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  blinker.blink(200, LED_BUILTIN);
  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

  // connected to netpie so turn off the led
  blinker.detach();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  /* To check if the microgear is still connected */
  if (microgear.connected()) {
    // Serial.println("connected");

    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();

    if (timer >= 100) {
      if (digitalRead(2) == 0) {
        microgear.chat("SP_P9", "1");
        Serial.print("Publish... ");
        String msg5 = "http://www.espert.io/MySmartphone/send?key=" + smartphone_key + "&message=" + message;
        doHttpGet(msg5);
      }


      timer = 0;
    }
    else timer += 100;
  }
  else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(100);
}

void doHttpGet(String msg) {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");

  http.begin(msg); // GET HTTP
  Serial.println(msg);

  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    Serial.print("[CONTENT]\n");

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
