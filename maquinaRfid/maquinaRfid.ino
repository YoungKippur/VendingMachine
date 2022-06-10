#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define SS_PIN 15
#define RST_PIN 16

#define FIREBASE_HOST "https://server-ort1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "iWBQFFHObz1rvfZW4mOqRGuRI06oR8BHCFx2vFuj"
#define WIFI_SSID "CABLEVISION-eec1"
#define WIFI_PASSWORD "1805BMISBQHA"

FirebaseData firebaseData;

MFRC522 rfid(SS_PIN, RST_PIN);

String idCard = "";
int num, data, aux;

void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      readCard();

      String id = idCard;
      if (Firebase.getInt(firebaseData, id)) {
        data = firebaseData.to<int>();
        aux = 1;
      }
      if (aux == 1) {
        if (data >= 10) {
          Firebase.setInt(firebaseData, id, (data - 10));
          Serial.println("1");
          num = 0;
          while (num == 0) {
            num = Serial.parseInt();
          }
          aux = 0;
        }

        else if (data >= 0 && data < 10) {
          Serial.println("2");
          num = 0;
          while (num == 0) {
            num = Serial.parseInt();
          }
          aux = 0;
        }
      }
      if (Firebase.getInt(firebaseData, "aux")) {
        data = firebaseData.to<int>();
      }
      rfid.PICC_HaltA();
      idCard = "";
    }
  }
  else {
    Serial.println("0");
  }
}

void readCard() {
  for (byte i = 0; i < rfid.uid.size; i++) {
    idCard = idCard + rfid.uid.uidByte[i];
  }
}
