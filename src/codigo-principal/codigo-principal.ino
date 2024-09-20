
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
#define pinBot2 0
#define pinPot 39
#define enderecoLCD 0x27 //Testar com o código do arduino para identicar o endereco correto

// Variáveis para Setup do Wifi
String WIFI_SSID;
String WIFI_PASSWORD;

//Variáveis para Setup do Firebase
#define API_KEY "AIzaSyAf0ubkUu9w0gJxshU7Lu78LGFlN3_QjcA"
#define DATABASE_URL "https://inventario-c50f8-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
int count = 0;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

//Variáveis do projeto
String uid = "";

// Variáveis do modo de leitura
bool selecaoBloco = false;
bool selecaoSala = false;
int blocoSelecionado = 0;
int salaSelecionada = 0;

String nomeBloco = "";
String nomeSala = "";
String local = "";
String path = "";

int numeroSala = 0;

bool iniciarLeitura = false;


//Definição do Módulo
#define RST_PIN 17   // Configurable, see typical pin layout above
#define SS_PIN 5    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

LiquidCrystal_I2C lcd(enderecoLCD, 16, 2);

//*****************************************************************************************//

void wifiSetup() {

  Serial.println("- Iniciando conexão com o wifi...");
  Serial.print("- Qual o nome da rede wifi? ");

  while (Serial.available() <= 0) {}
  WIFI_SSID = Serial.readString();
  Serial.print("Wifi selecionado foi "); Serial.println(WIFI_SSID);


  Serial.println("- Agora, qual é a senha da rede? ");
  delay(500);

  while (Serial.available() <= 0) {}
  WIFI_PASSWORD = Serial.readString();
  Serial.print("Senha fornecida foi "); Serial.println(WIFI_PASSWORD);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wifi conectado!");
  /*
  Serial.println();
  Serial.println("- Wifi conectado com sucesso!");
  Serial.println("----------------------------------------------");
  Serial.println();
  */

}
void firebaseSetup() {

  Serial.println("Iniciando conexão com o banco de dados...");
  Serial.println("");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Conectado com sucesso!");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n Erro na conexão: ", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    
    lcd.setCursor(0,1);
    lcd.print("Firebase OK!");
    /*
    Serial.println("Banco de dados pronto.");
    Serial.println("");
    Serial.println("----------------------------------------------");
    Serial.println("");
    */
  }
}

int selecionarOpcao() {
  int valorPot = analogRead(pinPot); //Faz a leitura do valor do potenciometro
  int conversao = map(valorPot, 0, 4095, 1, 3);
  return conversao;
}

void setup() {

  //Inicializando a serial
  Serial.begin(9600);
  SPI.begin();

  //Inicializando o RFID
  mfrc522.PCD_Init();

  //Iniciando LCD
  
  lcd.begin(12,14);
  lcd.clear();
  lcd.backlight();

  //lcd.setCursor(coluna,linha);

  pinMode(pinBot1, INPUT);
  pinMode(pinBot2, INPUT);

}

//*****************************************************************************************//

void loop() {

  int modo = selecionarOpcao(); /* 1 - Setup; 2 - Leitura; 3 - Registro */
  String smodo = String(modo);
  bool botao = digitalRead(pinBot1);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Modo: ");
  lcd.setCursor(6,0);
  lcd.print(smodo);

  if (modo == 1 && botao == HIGH) { // Modo setup
    if (signupOK != true) {
      //wifi
      wifiSetup();

      //firebase
      firebaseSetup();
      delay(1000);
    }
    modo = 0;
  }
  else if (modo == 2 && botao == HIGH) {
    delay(1000);
    lcd.clear();
    
    while (!selecaoBloco)
    {
      
      int valorPot = analogRead(pinPot);
      int conversao = map(valorPot, 0, 4095, 1, 6);

      
      lcd.setCursor(0, 0);
      lcd.print("Bloco: ");
      lcd.setCursor(7, 0);
      

      if (conversao == 1)
      {
        nomeBloco = "A";
        //lcd.print(nomeBLoco);
      }
      else if (conversao == 2)
      {
        nomeBloco = "B";
        //lcd.print(nomeBLoco);
      }
      else if (conversao == 3)
      {
        nomeBloco = "C";
        //lcd.print(nomeBLoco);
      }
      else if (conversao == 4)
      {
        nomeBloco = "D";
        //lcd.print(nomeBLoco);
      }
      else if (conversao == 5)
      {
        nomeBloco = "E";
        //lcd.print(nomeBLoco);
      }
      else if (conversao == 6)
      {
        nomeBloco = "F";
        //lcd.print(nomeBLoco);
      }

      bool valorBot = digitalRead(pinBot1);
      if (valorBot == true)
      {
        selecaoBloco == true;
        blocoSelecionado == conversao;
      }
    }

    while (!selecaoSala)
    {
      lcd.setCursor(0, 0);
      lcd.print("Bloco: ");
      lcd.setCursor(6, 0);
      int valorPot = analogRead(pinPot);
      

      if (blocoSelecionado == 1)
      {
        numeroSala = 20;
      }
      else if (blocoSelecionado == 2)
      {
        numeroSala = 20;
      }
      else if (blocoSelecionado == 3)
      {
        numeroSala = 20;
      }
      else if (blocoSelecionado == 4)
      {
        numeroSala = 20;
      }
      else if (blocoSelecionado == 5)
      {
        numeroSala = 20;
      }
      else if (blocoSelecionado == 6)
      {
        numeroSala = 20;
      }

      int conversao = map(valorPot, 0, 4095, 1, numeroSala);

      String nomeSala = String(numeroSala);
      lcd.setCursor(0, 1);
      lcd.print("Sala: ");
      lcd.setCursor(6, 1);
      lcd.print(nomeSala);

      int valorBot = digitalRead(pinBot1);
      if (valorBot == HIGH)
      {
        selecaoSala == true;
        salaSelecionada == conversao;
        nomeSala = String(numeroSala);
      }
    }

    delay(1000);

    local.concat(nomeBloco);
    local.concat(nomeSala);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Local: ");
    lcd.setCursor(7, 0);
    lcd.print(local);
    
    lcd.setCursor(0, 1);
    lcd.print("Iniciar leitura?");

    iniciarLeitura = false;

    int acaoBot = 0;
    while (acaoBot != 0) //Espera ação do botão para iniciar a leitura
    {
      
      int valorBotSim = digitalRead(pinBot1);
      int valorBotNao = digitalRead(pinBot2);

      if (valorBotSim == HIGH && valorBotNao == LOW)
      {
        iniciarLeitura = true;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Iniciando");
        lcd.setCursor(0,1);
        lcd.print("leitura");
        delay(1000);
        acaoBot = 1;
      }
      else if (valorBotSim == LOW && valorBotNao == HIGH)
      {
        iniciarLeitura = false;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Retornando");
        delay(1000);
        acaoBot = 1;
        return;
      }
      else if (valorBotSim == HIGH && valorBotNao == HIGH)
      {
        /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
      }
      delay(100);
    }

    if (iniciarLeitura == true) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Lendo");
      FirebaseJsonArray leituraSala; //Cria instancia para guardar as uids

      path = "/" + local;
      bool repeat = false;
      //Inicio do processo de leitura de todas as tags e registro nas salas dadas pelo usuario

      while (repeat != true) {
        if (!mfrc522.PICC_IsNewCardPresent()) {
          return;
        }
        if (!mfrc522.PICC_ReadCardSerial()) {
          return;
        }
        
        uid = "";

        for (byte i = 0; i < mfrc522.uid.uidByte[i]; i++) { //Organiza os caracteres da uid para a string
          uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
          uid.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        uid.toUpperCase(); //Coloca os caracteres em caixa alta

        lcd.setCursor(0,1);
        lcd.print("> ");
        lcd.setCursor(2,1);
        Serial.println(uid);

        int confirma = 0;
        while (confirma != 0)
        {
          int valorBotSim = digitalRead(pinBot1);
          int valorBotNao = digitalRead(pinBot2);

          if (valorBotSim == HIGH && valorBotNao == LOW) //Caso em que o botão de confirmar é pressionado
          {
            leituraSala.add(uid);
            lcd.setCursor(2,1);
            lcd.print("              ");
            delay(500);
            lcd.print("Registrada");
            delay(500);
            lcd.print("              ");
            confirma = 1;
          }
          else if (valorBotSim == LOW && valorBotNao == HIGH) //Caso em que o botão de cancelar é pressionado
          {
            confirma = 1;
            lcd.setCursor(2,1);
            lcd.print("              ");
            delay(500);
            lcd.print("Não registrada");
            delay(1000);
            lcd.print("              ");
            return;
          }
          else if (valorBotSim == HIGH && valorBotNao == HIGH) //Caso em que os dois botões são pressionados
          {
            /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
          }
          delay(100);
        }

        int confirmaRep = 0;
        while( confirmaRep != 0)
        {
          int valorBotSim = digitalRead(pinBot1);
          int valorBotNao = digitalRead(pinBot2);

          lcd.setCursor(2,1);
          lcd.print("Continuar?");

          if (valorBotSim == HIGH && valorBotNao == LOW)
          {
            repeat = true;
            lcd.setCursor(2,1);
            lcd.print("              ");
            confirmaRep = 1;
          }
          else if (valorBotSim == LOW && valorBotNao == HIGH)
          {
            repeat = false;
            lcd.setCursor(2,1);
            lcd.print("              ");
            delay(500);
            lcd.print("Voltando");
            delay(1000);
            lcd.print("              ");
            confirmaRep = 1;
          }
          else if (valorBotSim == HIGH && valorBotNao == HIGH)
          {
            /*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
          }
          delay(100);
        }
      } //finalizou as leituras

      Serial.print(Firebase.RTDB.setArray(&fbdo, path, &leituraSala) ? "ok" : fbdo.errorReason().c_str());
    }
    modo = 0;
  }
  else if (modo == 3 && botao == true) {

  }
}
