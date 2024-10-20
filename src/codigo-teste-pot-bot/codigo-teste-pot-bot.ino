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
  pinMode(0, INPUT);

}

void loop() {

  int a = digitalRead(4);
  int b = digitalRead(0);

  Serial.print(a); Serial.println(b);

}
