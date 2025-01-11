
//Importação de bibliotecas
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <LiquidCrystal_I2C.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//Defines
#define pinBot1 4
#define pinBot2 2
#define pinPot 34
#define enderecoLCD 0x27  //Testar com o código do arduino para identicar o endereco correto

// Variáveis para Setup do Wifi
String WIFI_SSID;
String WIFI_PASSWORD;

//Variáveis para Setup do Firebase
#define API_KEY "AIzaSyAf0ubkUu9w0gJxshU7Lu78LGFlN3_QjcA"
#define DATABASE_URL "https://inventario-c50f8-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

//Variáveis do projeto
bool iniciar = false;

String uid = "";
String wifiNomes[] = { "WIFI-LABORATORIO" };
String wifiSenhas[] = { "labs2022!" } int nwifiSelecionado = 0;
String wifiSelecionado = "";

bool sairLoop = false;

// Variáveis do modo de leitura
bool selecaoBloco = false;
bool selecaoSala = false;
int blocoSelecionado = 0;
int salaSelecionada = 0;

String nomeBloco = "";
String nomeSala = "";
String local = "";

int numeroSala = 0;

bool iniciarLeitura = false;

int valorPot = 0;
int valorBot = 0;
int conversao = 0;

String blocos[] = {"A", "B", "C", "D", "E", "F"};

//Definição do Módulo
#define RST_PIN 17  // Configurable, see typical pin layout above
#define SS_PIN 5    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

LiquidCrystal_I2C lcd(enderecoLCD, 16, 2);

FirebaseJson tagsRegistradas;  //Cria instancia para guardar as uids
FirebaseJson campus;


//*****************************************************************************************//

void wifiSetup() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectando a");

  // while () {
  //   lcd.setCursor(1, 0);
  //   nwifiSelecionado = map(analogRead(pinPot), 0, 4095, 0, nomesWifi.lengh())
  //   lcd.print(wifiNomes[nwifiSelecionado]);
  // }

  WIFI_SSID = wifiNomes[0];
  WIFI_PASSWORD = wifiSenhas[0];

  lcd.setCursor(1, 0);
  lcd.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {}

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wifi conectado!");
  delay(1000);
  lcd.clear();
}
void firebaseSetup() {

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  while (signupOK != true) {
    if (Firebase.signUp(&config, &auth, "inventario.dispositivo01@gmail.com", "labfisica2024!")) {
      signupOK = true;
    } else {
      Serial.printf("%s\n Erro na conexão: ", config.signer.signupError.message.c_str());
    }

    if (digitalRead(pinBot2) == HIGH) {
      return;
    }

    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    if (Firebase.ready()) {
      lcd.setCursor(0, 1);
      lcd.print("Firebase OK!");
    }
  }
}

int selecionarOpcao() {
  int m = analogRead(pinPot);  //Faz a leitura do valor do potenciometro
  int i = map(m, 0, 4095, 1, 3);
  return i;
}

void setup() {

  //Inicializando a serial
  Serial.begin(9600);
  SPI.begin();

  //Inicializando o RFID
  mfrc522.PCD_Init();

  //Iniciando LCD

  lcd.init();
  lcd.clear();
  lcd.backlight();

  //lcd.setCursor(coluna,linha);
  pinMode(pinPot, INPUT);
  pinMode(pinBot1, INPUT);
  pinMode(pinBot2, INPUT);

  iniciar = false;
}

//*****************************************************************************************//

void loop() {

  if (iniciar = false) {
    wifiSetup();
    delay(300);
    firebaseSetup();
    delay(300);

    if (Firebase.RTDB.getJSON(&fbdo, "/tagsLidas")) {
      tagsRegistradas = fbdo.jsonObject();
      Serial.println("Leitura das tags registradas realizada com sucesso!");
      tagsRegistradas.toString(Serial, true);
    }
    delay(10);
    if (Firebase.RTDB.getJSON(&fbdo, "/campus")) {
      campus = fbdo.jsonObject();
      Serial.println("Mapeamento do campus realizado com sucesso");
      campus.toString(Serial, true);
    }
    iniciar = true;
  }

  if (modo == 1 && botao == HIGH) {  // Modo setup
    if (signupOK != true) {
    }
    modo = 0;
  } else if (modo == 2 && botao == HIGH) {
    delay(300);
    lcd.clear();

    nomeBloco = "";
    nomeSala = "";
    local = "";

    while (!selecaoBloco) {
      delay(300);

      valorPot = analogRead(pinPot);
      conversao = map(valorPot, 0, 4095, 1, 6);

      lcd.setCursor(0, 0);
      lcd.print("Bloco: ");
      lcd.setCursor(7, 0);

      valorBot = digitalRead(pinBot1);
      if (valorBot == 1) {
        selecaoBloco = true;
        blocoSelecionado = blocos[conversao-1];
        lcd.clear();
      }
    }

    while (!selecaoSala) {
      delay(300);

      lcd.setCursor(0, 0);
      lcd.print(nomeBloco);

      valorPot = analogRead(pinPot);

      if (blocoSelecionado == 1) {
        numeroSala = Firebase.RTDB.getInt(campus, "/A/numeroSala");
      } else if (blocoSelecionado == 2) {
        numeroSala = Firebase.RTDB.getInt(campus, "/B/numeroSala");
      } else if (blocoSelecionado == 3) {
        numeroSala = Firebase.RTDB.getInt(campus, "/C/numeroSala");
      } else if (blocoSelecionado == 4) {
        numeroSala = Firebase.RTDB.getInt(campus, "/D/numeroSala");
      } else if (blocoSelecionado == 5) {
        numeroSala = Firebase.RTDB.getInt(campus, "/E/numeroSala");
      } else if (blocoSelecionado == 6) {
        numeroSala = Firebase.RTDB.getInt(campus, "/F/numeroSala");
      }

      int conversao = map(valorPot, 0, 4095, 1, numeroSala);

      String nomeSala = String(conversao);
      lcd.setCursor(0, 1);
      lcd.print("Sala:");
      lcd.setCursor(6, 1);
      lcd.print(nomeSala + " ");


      valorBot = digitalRead(pinBot1);
      if (valorBot == 1) {
        selecaoSala = true;
        local.concat(nomeBloco);
        local.concat(nomeSala);
        lcd.clear();
      }
    }
    delay(300);

    Serial.println(local);
    lcd.setCursor(0, 0);
    lcd.print(local);

    lcd.setCursor(0, 1);
    lcd.print("Iniciar leitura?");

    iniciarLeitura = false;

    int acaoBot = 0;
    while (acaoBot != 1)  //Espera ação do botão para iniciar a leitura
    {

      // pinBot1 - Sim pinBot2 - Não

      if (digitalRead(pinBot1) == HIGH && digitalRead(pinBot2) == LOW) {
        //iniciarLeitura = true;
        delay(300);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(local);
        lcd.setCursor(0, 1);
        lcd.print("Aproxime");

        bool repeat = true;
        //Inicio do processo de leitura de todas as tags e registro nas salas dadas pelo usuario

        while (repeat == true) {
          int run = 0;

          if (!mfrc522.PICC_IsNewCardPresent()) {
          } else {
            run = 1;
          }
          if (!mfrc522.PICC_ReadCardSerial()) {
          }

          if (run == 1) {
            uid = "";

            for (byte i = 0; i < mfrc522.uid.uidByte[i]; i++) {  //Organiza os caracteres da uid para a string
              uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
              uid.concat(String(mfrc522.uid.uidByte[i], HEX));
            }
            uid.toUpperCase();  //Coloca os caracteres em caixa alta

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(local);
            lcd.setCursor(4, 0);
            Serial.println(uid);
            lcd.print(uid);
            lcd.setCursor(0, 1);
            lcd.print("Registrar?   ");

            int confirma = 0;
            while (confirma == 0 && uid != " ") {

              if (digitalRead(pinBot1) == HIGH && digitalRead(pinBot2) == LOW)  //Caso em que o botão de confirmar é pressionado
              {
                tagsRegistradas.set(uid + "/local", local);
                tagsRegistradas.toString(Serial, true);
                lcd.setCursor(0, 1);
                lcd.print("            ");
                delay(500);
                lcd.setCursor(0, 1);
                lcd.print("Registrada");
                delay(500);
                lcd.setCursor(0, 1);
                lcd.print("            ");
                confirma = 1;

                Serial.print(Firebase.RTDB.set(&fbdo, F("/tagsLidas/"), &tagsRegistradas) ? "ok" : fbdo.errorReason().c_str());
              } else if (digitalRead(pinBot1) == LOW && digitalRead(pinBot2) == HIGH)  //Caso em que o botão de cancelar é pressionado
              {
                confirma = 1;
                lcd.setCursor(0, 1);
                lcd.print("              ");
                delay(500);
                lcd.setCursor(0, 1);
                lcd.print("Cancelando");
                delay(1000);
                lcd.setCursor(0, 1);
                lcd.print("              ");
                return;
              } else if (digitalRead(pinBot1) == HIGH && digitalRead(pinBot2) == HIGH)  //Caso em que os dois botões são pressionados
              {
                /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
              }
              delay(100);
            }

            int confirmaRep = 0;
            while (confirmaRep == 0) {

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Finalizar?");

              if (digitalRead(pinBot1) == LOW && digitalRead(pinBot2) == HIGH) {
                repeat = true;
                lcd.clear();
                confirmaRep = 1;
              } else if (digitalRead(pinBot1) == HIGH && digitalRead(pinBot2) == LOW) {
                repeat = false;
                lcd.setCursor(0, 0);
                lcd.print("              ");
                delay(500);
                lcd.setCursor(0, 0);
                lcd.print("Voltando");
                delay(1000);
                lcd.setCursor(0, 0);
                lcd.print("              ");
                confirmaRep = 1;
              } else if (digitalRead(pinBot1) == HIGH && digitalRead(pinBot2) == HIGH) {
                /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
              }
              delay(100);
            }
            run = 0;
            selecaoBloco = false;
            selecaoSala = false;
          }
        }  //finalizou as leituras
      } else if (digitalRead(pinBot1) == LOW && digitalRead(pinBot2) == HIGH) {
        //iniciarLeitura = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Retornando");
        delay(1000);
        acaoBot = 1;
        return;
      } else if (digitalRead(pinBot1) == HIGH && digitalRead(pinBot2) == HIGH) {
        /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
      }
      delay(100);
    }
    acaoBot = 0;
    modo = 0;
  } else if (modo == 3 && botao == true) {
  }

  delay(200);
}
