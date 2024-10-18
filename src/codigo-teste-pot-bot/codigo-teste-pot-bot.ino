#include <LiquidCrystal_I2C.h>

#define pinPot 34
#define enderecoLCD 0x27

LiquidCrystal_I2C lcd(enderecoLCD, 16, 2);

void setup() {
  
  //pinMode(pinPot, INPUT);
  Serial.begin(9600);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  pinMode(4, INPUT);//Confirma 
  pinMode(13, OUTPUT);

}

void loop() {

  int leitura = analogRead(pinPot);
  int conversao = map(leitura, 0, 4095, 1, 3);
  String sconversao = String(conversao);

  int leiturab1 = digitalRead(4);
  digitalWrite(13, leiturab1);

  Serial.println(leiturab1);

  lcd.setCursor(0,0);
  lcd.print("Modo: ");
  lcd.setCursor(6, 0);
  lcd.print(sconversao);

}
