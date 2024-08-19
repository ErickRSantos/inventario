
//Importação de bibliotecas
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <LiquidCrystal_I2C.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//Defines
//#define enderecoLCD 0x20; //Testar com o código do arduino para identicar o endereco correto

// Variáveis para Setup do Wifi
String WIFI_SSID ;
String WIFI_PASSWORD;

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

bool leituraNome = false;
bool leituraCodigo = false;
bool leituraBloco = false;
bool leituraSala = false;

int blocoSelecionado = 0;
int salaSelecionada = 0;

//Definição do Módulo
#define RST_PIN 17   // Configurable, see typical pin layout above
#define SS_PIN 5    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

LiquidCrystal_I2C lcd(ende,16,2);

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
  Serial.println();
  Serial.println("- Wifi conectado com sucesso!");
  Serial.println("----------------------------------------------");
  Serial.println();


}
void firebaseSetup() {

  Serial.println("Iniciando conexão com o banco de dados...");
  Serial.println("");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Conectado com sucesso!");
    signupOK = true;
  } else {
    Serial.printf("%s\n Erro na conexão: ", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Banco de dados pronto.");
    Serial.println("");
    Serial.println("----------------------------------------------");
    Serial.println("");
  }
}

int selecionarOpcao(){
  int valorPot = analogRead(A0); //Faz a leitura do valor do potenciometro
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
  lcd.init();
  lcd.clear(); 
  lcd.backlight(); 

  //lcd.setCursor(coluna,linha);

  pinMode(/*Entrada Botão*/, INPUT_PULLUP);

}

//*****************************************************************************************//

void loop() {

  int modo = selecionarOpcao(); /* 1 - Setup; 2 - Leitura; 3 - Registro */
  bool botao = digitalRead(/*Entrada digital do botão de confirmação*/);

  if (modo == 1 && botao == true){
    if (signupOK != true) {
    //wifi
    wifiSetup();

    //firebase
    firebaseSetup();
  	}
  } else if (modo == 2 && botao == true){
		bool selecaoBloco = false;
		bool selecaoSala = false;

		while (!selecaoBloco)
		{
			int valorPot = analogRead(A0);
			int conversao = map(valorPot, 0, 4095, 1, 6);

			lcd.setCursor(0, 0);
			lcd.print("Bloco: ");
			lcd.setCursor(6, 0);

			String nomeBloco = "";

			if(conversao == 1)
			{
				nomeBloco = "A";
				lcd.print(nomeBLoco);				

			} else if (conversao == 2)
			{
				nomeBloco = "B";
				lcd.print(nomeBLoco);
			} else if (conversao == 3)
			{
				nomeBloco = "C";
				lcd.print(nomeBLoco);
			} else if (conversao == 4)
			{
				nomeBloco = "D";
				lcd.print(nomeBLoco);
			} else if (conversao == 5)
			{
				nomeBloco = "E";
				lcd.print(nomeBLoco);
			} else if (conversao == 6)
			{
				nomeBloco = "F";
				lcd.print(nomeBLoco);
			}

			bool valorBot = digitalRead(/*Entrada Botão*/);
			if (valorBot == true)
			{
				selecaoBloco == true;
				blocoSelecionado == conversao;
			}
		}

		while (!selecaoSala)
		{
			int valorPot = analogRead(A0);
			int numeroSala = 0;
			
			if(blocoSelecionado == 1)
			{
				numeroSala = /**/ ;
			} else if (blocoSelecionado == 2)
			{
				numeroSala = /**/ ;
			} else if (blocoSelecionado == 3)
			{
				numeroSala = /**/ ;
			} else if (blocoSelecionado == 4)
			{
				numeroSala = /**/ ;
			} else if (blocoSelecionado == 5)
			{
				numeroSala = /**/ ;
			} else if (blocoSelecionado == 6)
			{
				numeroSala = /**/ ;
			}

			int conversao = map(valorPot, 0, 4095, 1, numeroSala);

			String nomeSala = String(numeroSala);
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Sala: ");
			lcd.setCursor(6, 0);
			lcd.print(nomeSala);

			bool valorBot = digitalRead(/*Entrada Botão*/);
			if (valorBot == true)
			{
				selecaoSala == true;
				salaSelecionada == conversao;
			}
		}

		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Local: ");
		lcd.setCursor(7,0);
		lcd.print(nomeBloco);
		lcd.setCursor(8, 0);
		lcd.print(nomeSala);

		lcd.setCursor(0,1);
		lcd.print("Iniciar leitura?")

		bool valorBotSim = digitalRead(/*Entrada Botão*/);
		bool valorBotNao = digitalRead(/*Entrada Botão*/);

		if(valorBotSim == true && valorBotNao == false)
		{

		} else if (valorBotSim == false && valorBotNao == true)
		{
			return;
		} else if (valorBotSim == true && valorBotNao == true)
		{
			/*Não realiza nada se os dois botoes forem pressionados ao mesmo tempo*/
		}

  } else if (modo == 3 && botao == true) {

  }


  
	/*
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

  Serial.print("A uid lida foi "); Serial.println(uid);

  
  //Nome do dispositivo ------------------------------------------------------------------
  while (!leituraNome) {
    Serial.println("");
    Serial.println("Qual o nome do dispositivo pertencente a essa uid? ");

    while (Serial.available() <= 0) {}

    nome = Serial.readString();
    tagRFID.add("nome", nome);

    Serial.println("");
    Serial.print("O nome dado para a uid "); Serial.print(uid); Serial.print(" foi "); Serial.println(nome);

    Serial.println("");
    Serial.println("Correto? (1 - SIM, 0 - Não)");

    while (Serial.available() <= 0 ) {}

    String resposta = Serial.readString();

    if (resposta == "1") {
      leituraNome = true;
    }

  }
  //Número de patrimônimo -----------------------------------------------------------------------
  Serial.println("");

  while (!leituraCodigo) {
    Serial.println("Digite o codigo de patrimônio: ");

    while (Serial.available() <= 0) {}

    codigo = Serial.readString();
    codigo.toUpperCase();
    tagRFID.add("codigo-patrimonio", codigo);

    Serial.println("");
    Serial.print("O código fornecido é "); Serial.println(codigo);

    Serial.println("");
    Serial.println("Correto? (1 - SIM, 0 - Não)");

    while (Serial.available() <= 0 ) {}

    String resposta = Serial.readString();

    if (resposta == "1") {
      leituraCodigo = true;
    }
  }
  //Nome do Bloco -----------------------------------------------------------------------
  Serial.println("");

  while (!leituraBloco) {
    Serial.println("Digite em qual bloco do campus o dispositvo se encontra: ");
    Serial.println("  A   B   C   D   E   F");

    while (Serial.available() <= 0) {}

    bloco = Serial.readString();
    bloco.toUpperCase();
    tagRFID.add("bloco", bloco);

    Serial.println("");
    Serial.print("O dispositivo está no bloco "); Serial.println(bloco);

    Serial.println("");
    Serial.println("Correto? (1 - SIM, 0 - Não)");

    while (Serial.available() <= 0 ) {}

    String resposta = Serial.readString();

    if (resposta == "1") {
      leituraBloco = true;
    }
  }
  //Nome da Sala -----------------------------------------------------------------------
  Serial.println("");

  while (!leituraSala) {
    Serial.print("Digite a sala do bloco "); Serial.print(bloco); Serial.print(" o dispostivo está: ");

    while (Serial.available() <= 0) {}

    sala = Serial.readString();
    sala.toUpperCase();
    tagRFID.add("sala", sala);

    Serial.println("");
    Serial.print("O dispositivo está na sala "); Serial.print(sala); Serial.print(" do bloco "); Serial.println(bloco);

    Serial.println("");
    Serial.println("Correto? (1 - SIM, 0 - Não)");

    while (Serial.available() <= 0 ) {}

    String resposta = Serial.readString();

    if (resposta == "1") {
      leituraSala = true;
    }
  }
	*/
  
  //Registro no Firebase ---------------------------------------------------------------

  Serial.println("");
  Serial.println("Deseja registrar os dados dessa tag o banco de dados? (1 - SIM, 0 - Não)");
  Serial.print("Resposta: ");

  while (Serial.available() <= 0 ) {}

  String resposta = Serial.readString();

  if (resposta == "1") {
    Serial.println(resposta);

    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      FirebaseJson conteudo;
      String jsonString;
      conteudo = tagRFID;
      conteudo.toString(jsonString, true);
      Serial.println(jsonString);

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

