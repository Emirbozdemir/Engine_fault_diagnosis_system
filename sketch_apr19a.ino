#include <LiquidCrystal.h>

// LCD Pinleri: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int micPin = A0;

// LED Pinleri
const int ledYesil = 9;   
const int ledSari = 8;    
const int ledKirmizi = 7; 

void setup() {
  Serial.begin(9600);
  
  // LCD ve Pin kurulumları
  lcd.begin(16, 2);
  pinMode(ledYesil, OUTPUT);
  pinMode(ledSari, OUTPUT);
  pinMode(ledKirmizi, OUTPUT);
  
  // Başlangıçta tüm LED'ler sönük
  digitalWrite(ledYesil, LOW);
  digitalWrite(ledSari, LOW);
  digitalWrite(ledKirmizi, LOW);
  
  // Açılış Ekranı
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
  
  // 1. AŞAMA: 100 milisaniye boyunca sesin merkezini ve 'şiddetini' bul
  while(millis() - referansZaman < 100) {
    int okunan = analogRead(micPin);
    if(okunan > maxDeger) maxDeger = okunan;
    if(okunan < minDeger) minDeger = okunan;
  }
  
  int merkezNokta = (maxDeger + minDeger) / 2;
  int sesSiddeti = maxDeger - minDeger; // Motorun gerçekten çalışıp çalışmadığını anlamak için
  
  int frekansHz = 0;

  // --- GÜRÜLTÜ KAPISI (NOISE GATE) ---
  // Sadece ses şiddeti 30'dan büyükse (yani ortamda belirgin bir ses/motor varsa) dalgaları say.
  if (sesSiddeti > 30) {
    int dalgaSayaci = 0;
    boolean dalgaUstte = false;
    long sayimZamani = millis();
    
    // 2. AŞAMA: 1 saniye (1000 ms) boyunca dalgaları sayarak Hz'i bul
    while(millis() - sayimZamani < 1000) {
      int anlikDeger = analogRead(micPin);
      
      // Hassasiyeti (+15) yaptık ki ufak parazitleri dalga sanmasın
      if(anlikDeger > merkezNokta + 15 && !dalgaUstte) {
        dalgaUstte = true;
      }
      else if (anlikDeger < merkezNokta - 15 && dalgaUstte) {
        dalgaUstte = false;
        dalgaSayaci++; 
      }
    }
    frekansHz = dalgaSayaci; // 1 saniyedeki dalga sayısı = Frekans
  }

  // --- SERİ PORT KONTROLÜ (Bilgisayardan izlemek için) ---
  Serial.print("Ses Siddeti: ");
  Serial.print(sesSiddeti);
  Serial.print(" | Frekans: ");
  Serial.println(frekansHz);

  // --- LCD EKRAN YAZDIRMA (ÜST SATIR) ---
  lcd.setCursor(0, 0);
  lcd.print("FREKANS: "); 
  lcd.print(frekansHz);
  lcd.print(" Hz   "); // Eski sayıların izini silmek için boşluklar

  // --- LCD EKRAN VE LED ARIZA TESPİT MANTIĞI (ALT SATIR) ---
  lcd.setCursor(0, 1);
  
  if (frekansHz == 0) {
    // 0 Hz (Motor çalışmıyor veya ses sensörü sessiz ortamda)
    lcd.print("MOTOR KAPALI    ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
  }
  else if (frekansHz > 160) {
    // 160 Hz Üstü (Aşırı devir durumu veya dışarıdan aşırı parazit)
    lcd.print("ASIRI FREKANS!  ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW); 
  }
  else if (frekansHz >= 80 && frekansHz <= 160) { 
    // 80 - 160 Hz (Yeşil / Sağlıklı Çalışma Aralığı)
    lcd.print("DURUM: NORMAL   ");
    digitalWrite(ledYesil, HIGH);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
  } 
  else if (frekansHz >= 70 && frekansHz < 80) { 
    // 70 - 79 Hz (Sarı / Uyarı Aralığı - Titreşim bozulmaya başlamış)
    lcd.print("ARIZA RISKI!    ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, HIGH);
    digitalWrite(ledKirmizi, LOW);
  }
  else if (frekansHz >= 50 && frekansHz < 70) { 
    // SADECE 50 - 69 Hz (Kırmızı / Kesin Arıza veya Ağır Sürtünme)
    lcd.print("ARIZA VAR!      ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, HIGH);
  }
  else {
    // 1 ile 49 Hz Arası (Motor yeni kalkış yapıyor veya frekans çok düşük)
    lcd.print("DUSUK DEVIR     ");
    digitalWrite(ledYesil, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
  }
}