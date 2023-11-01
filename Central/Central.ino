#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define UUID_SER "71096367-0e6a-4b33-bbca-9ef40f71c103"
#define UUID_CAR "e771dfb6-d703-47e2-a866-1df4a46e5a92"

//#define LED_INTERFACE "D12"
const int LED_INTERFACE = 12;
const int LED_ALARME = 27;

bool isConnected = false;


String findJsonValue(const String& json, const String& key) {
    int keyPos = json.indexOf("\"" + key + "\":");

    if (keyPos == -1) {
        return "Chave não encontrada";
    }

    int valueStart = json.indexOf(":", keyPos) + 1;
    int valueEnd = json.indexOf(",", valueStart);

    if (valueEnd == -1) {
        valueEnd = json.indexOf("}", valueStart);
    }

    String value = json.substring(valueStart, valueEnd);
    value.trim();

    if (value.startsWith("\"") && value.endsWith("\"")) {
        // Valor é uma string, remover as aspas
        value = value.substring(1, value.length() - 1);
    }

    return value;
}



void alerta1 () {
  digitalWrite(LED_INTERFACE, HIGH);
  delay(1000);
  digitalWrite(LED_INTERFACE, LOW);
  delay(2000);
}

void alerta2 () {
  digitalWrite(LED_INTERFACE, HIGH);
  delay(500);
  digitalWrite(LED_INTERFACE, LOW);
  delay(500);
  digitalWrite(LED_INTERFACE, HIGH);
  delay(500);
  digitalWrite(LED_INTERFACE, LOW);
}

void alerta3 () {
  digitalWrite(LED_INTERFACE, HIGH);
  delay(500);
  digitalWrite(LED_INTERFACE, LOW);
  delay(500);
  digitalWrite(LED_INTERFACE, HIGH);
  delay(500);
  digitalWrite(LED_INTERFACE, LOW);
  delay(500);
  digitalWrite(LED_INTERFACE, HIGH);
  delay(500);
  digitalWrite(LED_INTERFACE, LOW);
}

void imprimirPayload(const char* payload) {
  Serial.println(payload);
//  DeserializationError err = deserializeJson(doc, payload);
//
//  if (err) {
//    Serial.print(F("deserializeJson() failed with code "));
//    Serial.println(err.f_str());
//  }


//  const char* name = doc["name"];
//  long stars = doc["stargazers"]["totalCount"];
//  int issues = doc["issues"]["totalCount"];

//  const char* valor = doc["d"]["presenca"];

  String key = "presenca";
  
  String value = findJsonValue(payload, key);
  
  Serial.println(key + ": " + value);

  if (value == "1") {
    digitalWrite(LED_ALARME, HIGH);
  }

  if (value == "0") {
    digitalWrite(LED_ALARME, LOW);
  }
  
  
  
}

class CallbacksDeCaracteristica : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *caracteristica) {
    std::string payload = caracteristica->getValue();

    if (payload.length() > 0) {
      imprimirPayload( payload.c_str() );
    }
  }
  
};


class MyServerCallbacks : public BLEServerCallbacks {

  // Quando um dispositivo se conecta a Central
  void onConnect(BLEServer *servidor) {
    Serial.println("Conectado - BL");
    isConnected = true;
    alerta2();
  }

  void onDisconnect (BLEServer *servidor) {
    Serial.println("Aberto para conexões - BL");
    isConnected = false;
    servidor->getAdvertising()->start();
  }
  
};




void setup() {
  pinMode(LED_INTERFACE, OUTPUT);
  pinMode(LED_ALARME, OUTPUT);

  alerta3();
  
  Serial.begin(115200);

  Serial.println("Iniciando Central");

  BLEDevice::init("ESP32");

  BLEServer *servidor = BLEDevice::createServer();

  servidor->setCallbacks( new MyServerCallbacks() );

  BLEService *servico = servidor->createService(UUID_SER);

  BLECharacteristic *caracteristica = servico->createCharacteristic(UUID_CAR, BLECharacteristic::PROPERTY_WRITE); // permite que dispositivos conectados a esta caracteristica escrevam valores para ela

  caracteristica->setCallbacks( new CallbacksDeCaracteristica() );

  servico->start();

  servidor->getAdvertising()->addServiceUUID(UUID_SER);

  Serial.println("Central pronta para receber conexoes");
  
  servidor->getAdvertising()->start();

}

void loop() {
  delay(10);  

  if (!isConnected) { // emitir sinal luminoso enquanto não houver conexão extabelecida
    alerta1();
  }
}
