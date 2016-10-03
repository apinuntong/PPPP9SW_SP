/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>
//#include <DHT.h>

#include "CMMC_Blink.hpp"
CMMC_Blink blinker;

const char* ssid     = "ESPERT-3020";
const char* password = "espertap";

#define APPID       "P9"
#define KEY         "JrJt942kkk8znlT"
#define SECRET      "6lA3DROOW6Esx20cNapV6fHSV"
#define ALIAS       "SP_P9"

WiFiClient client;
AuthClient *authclient;

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

  if (msg2 == "1") {
    microgear.chat("SW_P9", "ON");
    digitalWrite(relayPin, HIGH);
    digitalWrite(16, LOW);
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

  pinMode(relayPin, OUTPUT);
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
uint32_t num_x = 0;
void loop() {
  /* To check if the microgear is still connected */
  if (digitalRead(relayPin) == 1) {
    num_x++;
    Serial.println(num_x);
  }
  if (microgear.connected()) {
    // Serial.println("connected");

    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();

    if (timer >= 100) {
      if (digitalRead(2) == 0) {
        microgear.chat("SW_P9", "OFF");
        Serial.print("Publish... ");
        digitalWrite(16, HIGH);
        digitalWrite(relayPin, LOW);
        num_x = 0;
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
  if (digitalRead(relayPin) == 1) {
    if (num_x > 3000) {
      num_x = 0;
     digitalWrite(relayPin, LOW);
    }
  }

}
