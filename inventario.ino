
//Importação de bibliotecas
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Variáveis para Setup do Wifi
String WIFI_SSID ;
//#define WIFI_SSID "WIFI-LABORATORIO"
String WIFI_PASSWORD;
//#define WIFI_PASSWORD "labs2022!"

//Variáveis para Setup do Firebase
#define API_KEY "AIzaSyC9VYdBhgajTQylTf5Acuco4P_bdH5AlWo"
#define DATABASE_URL "https://inventario-6601b-default-rtdb.firebaseio.com/rfids"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
int count = 0;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

//Variáveis do projeto
String uid = "";
String nome = "";
String codigo = "";
String bloco = "";
String sala = "";
String endereco;

//Definição do Módulo
#define RST_PIN 21  // Configurable, see typical pin layout above
#define SS_PIN 5    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

//*****************************************************************************************//

void wifiSetup() {

  Serial.println("Iniciando conexão com o wifi...");
  Serial.println("Qual o nome da rede wifi? ");

  while (Serial.available() <= 0) {}
  WIFI_SSID = Serial.readString();
  Serial.println(WIFI_SSID);

  Serial.println("Agora, qual é a senha da rede? ");
  delay(500);

  while (Serial.available() <= 0) {}
  WIFI_PASSWORD = Serial.readString();
  Serial.println(WIFI_PASSWORD);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("conectado!");

}
void firebaseSetup() {

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void setup() {

  //Inicializando a serial
  Serial.begin(9600);
  SPI.begin();

  //Inicializando o RFID
  mfrc522.PCD_Init();

}

//*****************************************************************************************//

void loop() {

  if (signupOK != true) {
    //wifi
    wifiSetup();

    //firebase
    firebaseSetup();
  }

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  FirebaseJson tagRFID;

  uid = "";
  for (byte i = 0; i < mfrc522.uid.uidByte[i]; i++) {
    uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  uid.toUpperCase(); //Coloca os caracteres em caixa alta

  //tagRFID.add("uid", uid);
  //Interacao com o usuário

  Serial.print("A uid lida foi "); Serial.println(uid);

  //Nome do dispositivo ------------------------------------------------------------------
  Serial.println("Qual o nome do dispositivo pertencente a essa uid? ");

  while (Serial.available() <= 0) {
    Serial.print(".");
    delay(300);
  }

  nome = Serial.readString();
  tagRFID.add("nome", nome);

  Serial.println("");
  Serial.print("O nome dado para a uid "); Serial.print(uid); Serial.print(" foi "); Serial.println(nome);

  //Número de patrimônimo -----------------------------------------------------------------------
  Serial.println("");
  Serial.println("Qual o código de patrimônio do dispositivo? ");

  while (Serial.available() <= 0) {
    Serial.print(".");
    delay(300);
  }

  codigo = Serial.readString();
  tagRFID.add("codigo-patrimonio", codigo);

  Serial.println("");
  Serial.print("O código fornecido é "); Serial.println(codigo);

  //Nome do Bloco -----------------------------------------------------------------------
  Serial.println("");
  Serial.println("Este dispositivo está em qual bloco do campus? ");

  while (Serial.available() <= 0) {
    Serial.print(".");
    delay(300);
  }

  bloco = Serial.readString();
  tagRFID.add("bloco", bloco);

  Serial.println("");
  Serial.print("O dispositivo está no bloco "); Serial.println(bloco);

  //Nome da Sala -----------------------------------------------------------------------
  Serial.println("");
  Serial.println("Este dispositivo está em qual sala? ");

  while (Serial.available() <= 0) {
    Serial.print(".");
    delay(300);
  }

  sala = Serial.readString();
  tagRFID.add("sala", sala);

  Serial.println("");
  Serial.print("O dispositivo está na sala "); Serial.println(sala);
  
  //Registro no Firebase ---------------------------------------------------------------

  Serial.println("");
  Serial.println("Deseja registrar os dados dessa tag o banco de dados? (1 - SIM, 0 - Não)");
  Serial.print("Resposta: ");

  while (Serial.available() <= 0 ) {}

  String resposta = Serial.readString();

  if (resposta == "1") {
    //endereco = enderecoRegistro(uid, nome);
    Serial.println(resposta);

    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      FirebaseJson conteudo;
      conteudo = tagRFID;
      conteudo.toString(Serial, true);

      if (Firebase.RTDB.setJSON(&fbdo, uid, &tagRFID)) {
          Serial.println("Os dados foram registrados");
          } else {
        Serial.println("FALHA AO REGISTRAR" + fbdo.errorReason());
        Serial.println("");
      }
    }

  } else {
    Serial.println("Dados não registrados");
    return;
  }
}



/*
  String enderecoRegistro(String dad, String son) {
  String path = dad;
  path.concat(String("/"));
  endereco.concat(String(son));

  return path;
  }*/
