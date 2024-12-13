
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

//Definição do Módulo
#define RST_PIN 17  // Configurable, see typical pin layout above
#define SS_PIN 5    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

LiquidCrystal_I2C lcd(enderecoLCD, 16, 2);

FirebaseJson leituraSala;  //Cria instancia para guardar as uids


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
      leituraSala = fbdo.jsonObject();
      Serial.println("Banco de dados lido");
      leituraSala.toString(Serial, true);
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


      if (conversao == 1) {
        nomeBloco = "A";
        lcd.print(nomeBloco);
      } else if (conversao == 2) {
        nomeBloco = "B";
        lcd.print(nomeBloco);
      } else if (conversao == 3) {
        nomeBloco = "C";
        lcd.print(nomeBloco);
      } else if (conversao == 4) {
        nomeBloco = "D";
        lcd.print(nomeBloco);
      } else if (conversao == 5) {
        nomeBloco = "E";
        lcd.print(nomeBloco);
      } else if (conversao == 6) {
        nomeBloco = "F";
        lcd.print(nomeBloco);
      }

      valorBot = digitalRead(pinBot1);
      if (valorBot == 1) {
        selecaoBloco = true;
        blocoSelecionado = conversao;
        lcd.clear();
      }
    }

    while (!selecaoSala) {
      delay(300);

      lcd.setCursor(0, 0);
      lcd.print(nomeBloco);

      valorPot = analogRead(pinPot);

      if (blocoSelecionado == 1) {
        numeroSala = 20;
      } else if (blocoSelecionado == 2) {
        numeroSala = 20;
      } else if (blocoSelecionado == 3) {
        numeroSala = 20;
      } else if (blocoSelecionado == 4) {
        numeroSala = 20;
      } else if (blocoSelecionado == 5) {
        numeroSala = 20;
      } else if (blocoSelecionado == 6) {
        numeroSala = 20;
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

      int valorBotSim = digitalRead(pinBot1);
      int valorBotNao = digitalRead(pinBot2);

      if (valorBotSim == HIGH && valorBotNao == LOW) {
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
              int valorBotSim = digitalRead(pinBot1);
              int valorBotNao = digitalRead(pinBot2);

              if (valorBotSim == 1 && valorBotNao == 0)  //Caso em que o botão de confirmar é pressionado
              {
                leituraSala.set(uid + "/local", local);
                leituraSala.toString(Serial, true);
                lcd.setCursor(0, 1);
                lcd.print("            ");
                delay(500);
                lcd.setCursor(0, 1);
                lcd.print("Registrada");
                delay(500);
                lcd.setCursor(0, 1);
                lcd.print("            ");
                confirma = 1;

                Serial.print(Firebase.RTDB.set(&fbdo, F("/tagsLidas/"), &leituraSala) ? "ok" : fbdo.errorReason().c_str());
              } else if (valorBotSim == 0 && valorBotNao == 1)  //Caso em que o botão de cancelar é pressionado
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
              } else if (valorBotSim == 1 && valorBotNao == 1)  //Caso em que os dois botões são pressionados
              {
                /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
              }
              delay(100);
            }

            int confirmaRep = 0;
            while (confirmaRep == 0) {
              int valorBotSim = digitalRead(pinBot1);
              int valorBotNao = digitalRead(pinBot2);

              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Finalizar?");

              if (valorBotSim == LOW && valorBotNao == HIGH) {
                repeat = true;
                lcd.clear();
                confirmaRep = 1;
              } else if (valorBotSim == HIGH && valorBotNao == LOW) {
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
              } else if (valorBotSim == HIGH && valorBotNao == HIGH) {
                /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
              }
              delay(100);
            }
            run = 0;
            selecaoBloco = false;
            selecaoSala = false;
          }
        }  //finalizou as leituras
      } else if (valorBotSim == LOW && valorBotNao == HIGH) {
        //iniciarLeitura = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Retornando");
        delay(1000);
        acaoBot = 1;
        return;
      } else if (valorBotSim == HIGH && valorBotNao == HIGH) {
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
