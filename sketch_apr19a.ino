#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int micPin = A0;


const int ledYesil = 9;   
const int ledSari = 8;    
const int ledKirmizi = 7; 

void setup() {
  Serial.begin(9600);
  
  
  lcd.begin(16, 2);
  pinMode(ledYesil, OUTPUT);
  pinMode(ledSari, OUTPUT);
  pinMode(ledKirmizi, OUTPUT);
  
  
  digitalWrite(ledYesil, LOW);
  digitalWrite(ledSari, LOW);
  digitalWrite(ledKirmizi, LOW);
  
  
  lcd.print("Motor Analiz");
  lcd.setCursor(0, 1);
  lcd.print("Sistemi Hazir");
  delay(2000);
  lcd.clear();
}

void loop() {
  long maxDeger = 0;
  long minDeger = 1023;
  long referansZaman = millis();
  
  
  while(millis() - referansZaman < 100) {
    int okunan = analogRead(micPin);
    if(okunan > maxDeger) maxDeger = okunan;
    if(okunan < minDeger) minDeger = okunan;
  }
  
  int merkezNokta = (maxDeger + minDeger) / 2;
  int sesSiddeti = maxDeger - minDeger; 
  
  int frekansHz = 0;

  
  if (sesSiddeti > 30) {
    int dalgaSayaci = 0;
    boolean dalgaUstte = false;
    long sayimZamani = millis();
    
    
    while(millis() - sayimZamani < 1000) {
      int anlikDeger = analogRead(micPin);
      
      
      if(anlikDeger > merkezNokta + 15 && !dalgaUstte) {
        dalgaUstte = true;
      }
      else if (anlikDeger < merkezNokta - 15 && dalgaUstte) {
        dalgaUstte = false;
        dalgaSayaci++; 
      }
    }
    frekansHz = dalgaSayaci; 
  }

  
  Serial.print("Ses Siddeti: ");
  Serial.print(sesSiddeti);
  Serial.print(" | Frekans: ");
  Serial.println(frekansHz);

  
  lcd.setCursor(0, 0);
  lcd.print("FREKANS: "); 
  lcd.print(frekansHz);
  lcd.print(" Hz   "); 

  
  lcd.setCursor(0, 1);
  
  if (frekansHz == 0) {
    
    lcd.print("MOTOR KAPALI    ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
  }
  else if (frekansHz > 160) {
    
    lcd.print("ASIRI FREKANS!  ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW); 
  }
  else if (frekansHz >= 80 && frekansHz <= 160) { 
    
    lcd.print("DURUM: NORMAL   ");
    digitalWrite(ledYesil, HIGH);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
  } 
  else if (frekansHz >= 70 && frekansHz < 80) { 
    
    lcd.print("ARIZA RISKI!    ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, HIGH);
    digitalWrite(ledKirmizi, LOW);
  }
  else if (frekansHz >= 50 && frekansHz < 70) { 
    
    lcd.print("ARIZA VAR!      ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, HIGH);
  }
  else {
    
    lcd.print("DUSUK DEVIR     ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
  }
}