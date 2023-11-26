#include "BLEDevice.h";


static BLEUUID UUID_SER("71096367-0e6a-4b33-bbca-9ef40f71c103"); //servico
static BLEUUID UUID_CAR("e771dfb6-d703-47e2-a866-1df4a46e5a92"); //caracteristica

// variaveis de conrole - verifica se tem conexao ativa e se o servidor foi conectado
static boolean encontrado = false;
static boolean conectado = false ;

static BLERemoteCharacteristic* caracteristica;
static BLEAdvertisedDevice* servidor;

//const int pinoSensor = A5;
//const int pinoSensor = 2;
const int pinoSensor = 14;

//#define LED_RED "D12"
const int LED_RED = 12;

void alerta0 () {
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(500);
}

void alerta1 () {
  digitalWrite(LED_RED, HIGH);
  delay(1000);
  digitalWrite(LED_RED, LOW);
  delay(2000);
}

void alerta2 () {
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
}

void alerta3 () {
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
}


class CallbacksDeConexao: public BLEClientCallbacks {
  
  void onConnect (BLEClient *cliente) {
    conectado = true;
    Serial.println("Conectado.");
  }

  void onDisconnect (BLEClient* cliente) {
    conectado = false;
    Serial.println("Desconectado.");
  }
  
};

class CallbackDeBusca: public BLEAdvertisedDeviceCallbacks {

  void onResult (BLEAdvertisedDevice dispositivo) {
    if ( dispositivo.haveServiceUUID() && dispositivo.isAdvertisingService(UUID_SER) ) {
      BLEDevice::getScan()->stop();

      servidor = new BLEAdvertisedDevice(dispositivo);

      encontrado = true;
    }
  }
  
};

void conectar () {
  BLEClient* cliente = BLEDevice::createClient();

  cliente->setClientCallbacks( new CallbacksDeConexao() );

  cliente->connect( servidor );

  while (!cliente->isConnected()) {
    alerta0();
    cliente->connect(servidor);
    delay(10);
  }

  BLERemoteService* servico = cliente->getService( UUID_SER );

  caracteristica = servico->getCharacteristic(UUID_CAR);
}

void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(pinoSensor, INPUT);

  alerta3();
  
  Serial.begin(115200);
  Serial.println("iniciado");
  pinMode(pinoSensor, INPUT);

  BLEDevice::init(""); // não precisa de nome pois ete dispositivo não sera buscado, ele é apenas um cliente

  BLEScan* scanner = BLEDevice::getScan();

  scanner->setAdvertisedDeviceCallbacks( new CallbackDeBusca () );
  scanner->setInterval(1349); // controlam alguns processor de busca do scanner
  scanner->setWindow(449); // podem ser valores sempre fixos
  scanner->setActiveScan(true);
  scanner->start(30, false); // por quanto tempo o processo de scanner vai ser executado ate que desista de encontrar o dispositivo EM SEGUNDOS. o callback de busca tbm é capaz de encerrar o processo
  
}

void loop() {
  // so pode enviar o valor se: o dispositivo que queremos nos conectar for encontrado e a conexao foi bem sucedida
  if (encontrado) {
    if (conectado) {
      int valor = 0;
      // if( digitalRead(pinoSensor) == LOW ) {
      //   valor = 0;
      //   Serial.print( valor );
      //   Serial.println(" - Tudo certo.");
      // } else {
      //   valor = 1;
      //   Serial.print( valor );
      //   Serial.println(" - SENSOR ATIVADO!!");
      // }

      int valorAnalogico = analogRead(pinoSensor);
      Serial.println("Value: ");
      Serial.println(valorAnalogico);
      
      if (valorAnalogico > 1000) {
        valor = 0;
      } else {
        valor = 1;
      }
      
      
      String nome = "presenca";

      String payload = "{\"d\":{\"" + nome + "\": " + valor + "}}"; //convencionado para trechos de informacoes em servicos na internet // {d: {"luminosidade": valor}}

      caracteristica->writeValue(payload.c_str(), payload.length()); // c_str forca a ser uma string caso algo tenha ocorrido no processo - o segundo parametro eh o comprimento da mensagem em caracteres
      
      if (valor == 1 ) {
//        alerta2();
      }
      
    } else {
      Serial.println("Dispositivo encontrado, porém não conectado");
      alerta0();
      conectar();
      
    }
    
  } else {
    alerta2();
    Serial.println("Nenhum dispositivo encontrado para se conectar");
    Serial.println("ativar varredura - 10 segundos");
    BLEDevice::getScan()->start(10);
    Serial.println("nao encontrado - este ponto");
  }

  delay(500);

}
