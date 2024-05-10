#include <LiquidCrystal.h>
#include "SoftwareSerial.h"

LiquidCrystal lcd(0,1,2,3,4,5);
SoftwareSerial Ss(10, 11);

unsigned long wynik = 0;
int kulka = 0;
bool koniec_gry = false;
bool gra_rozpoczeta = false;
bool dodatkowa_kulka = false;
int bonus = 0;
int wspolczynnik_bonus = 1;

bool aktywna_lewa = false;
bool dodatkowa_kulka_gotowa = false;
bool trzy_grzybki[3] = {false, false, false};
bool trzy_przetoki[3] = {false, false, false};
int ukonczone_cele = 0;
int uderzenia_w_grzybki = 0;
unsigned long czas_konca_gry = 0;

const int piny_wejsciowe[25] = {8, 9, 12, 13, 14, 15, 18, 19, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, A6, A7, A8, A9, A10, A11};
int odczyt[25] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
int ostatni_odczyt[25] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
unsigned long czas_od_wcisniecia[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const int piny_wyjsciowe[35] = {6, 7, 16, 17, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, A0, A1, A2, A3, A4, A5, A12, A13, A14, A15};
const int ledy_przetok[3] = {23,25,27};
const int ledy_wsp[4] = {A12, A13, A14, A15};

int dzwiek = 1;
bool czy_petla = false;
const unsigned long dlugosci_dzwiekow[10] = {3000,2000,2000,4000,3500,4000,5000,3000,4000,3000};
unsigned long moment_odtworzenia = 0;

void setup() {
  Ss.begin(9600);

  delay(500);
  modul_mp3(0x3F, 0, 0);
  delay(500);
  modul_mp3(0x06,0,15);
  delay(500);
  modul_mp3(0x08,0,1);
  delay(500);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("     SZTORM     ");
  lcd.setCursor(0, 1);
  lcd.print("  PRESS  START  ");


  for(int i=0; i<25; i++)
    pinMode(piny_wejsciowe[i], INPUT);
  for(int i=0; i<35; i++)
    pinMode(piny_wyjsciowe[i], OUTPUT);
  
  zapal_leda(6);
  zapal_leda(7);
  zapal_leda(35);
  zapal_leda(37);
}

void loop() {
  
  for(int i=0; i<25; i++) {
    int nowy_odczyt = digitalRead(piny_wejsciowe[i]);

    if (nowy_odczyt != ostatni_odczyt[i]) {
      czas_od_wcisniecia[i] = millis();
    }

    if (millis() - czas_od_wcisniecia[i] > 50) {
      if (nowy_odczyt != odczyt[i]) {
        odczyt[i] = nowy_odczyt;
        if (odczyt[i] == HIGH) {
          switch(piny_wejsciowe[i]) {
            case 8: //Lewy cel na środku stołu
              odtworz_dzwiek(4,false);
              ustaw_aktywna_strone(true);
              dodaj_punkty(1000);
              break;
            case 9: //Prawy cel na środku stołu
              odtworz_dzwiek(4,false);
              ustaw_aktywna_strone(false);
              dodaj_punkty(1000);
              break;
            case 12: //Przetoka przy lewym pasie końcowym
              odtworz_dzwiek(6,false);
              if(aktywna_lewa)
                dodaj_punkty(15000);
              else
                dodaj_punkty(5000);
              break;
            case 13: //Przetoka przy prawym pasie końcowym
              odtworz_dzwiek(6,false);
              if(!aktywna_lewa)
                dodaj_punkty(15000);
              else
                dodaj_punkty(5000);
              break;
            case 14: //Przetoka przy lewym pasie flippera
              odtworz_dzwiek(6,false);
              if(aktywna_lewa)
                dodaj_punkty(1500);
              else
                dodaj_punkty(500);
              break;
            case 15: //Przetoka przy prawym pasie flippera
              odtworz_dzwiek(6,false);
              if(!aktywna_lewa)
                dodaj_punkty(1500);
              else
                dodaj_punkty(500);
              break;
            case 18: //Bank 1, cel 1
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 19: //Bank 1, cel 2
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 22: //Bank 1, cel 3
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 24: //Przetoka górna 1
              sprawdz_trzy_przetoki(0);
              break;
            case 26: //Przetoka górna 2
              sprawdz_trzy_przetoki(1);
              break;
            case 28: //Przetoka górna 3
              sprawdz_trzy_przetoki(2);
              break;
            case 30: //Bank 2, cel 1
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 32: //Bank 2, cel 2
              odtworz_dzwiek(5,false);
              sprawdz_cele(true);
              break;
            case 34: //Bank 2, cel 3
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 36: //Bank 3, cel 1
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 38: //Bank 3, cel 2
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 40: //Bank 3, cel 3
              odtworz_dzwiek(5,false);
              sprawdz_cele(false);
              break;
            case 42: //(Re)start gry
              restart_gry();
              break;
            case A11: //Przełącznik wejścia kulki do fartucha
              kulka_stracona();
              break;
            case A6: //Spódnica grzybka 1
              if(uderzenia_w_grzybki>=30)
                dodaj_punkty(200);
              else
                dodaj_punkty(50);
              aktywuj_grzybek(A0);
              break;
            case A7: //Spódnica grzybka 2
              if(uderzenia_w_grzybki>=50)
                dodaj_punkty(200);
              else
                dodaj_punkty(50);
              aktywuj_grzybek(A1);
              break;
            case A8: //Spódnica grzybka 3
              if(uderzenia_w_grzybki>=80)
                dodaj_punkty(400);
              else
                dodaj_punkty(100);
              aktywuj_grzybek(A2);
              break;
            case A9: //Spódnica lewej procy
              zamien_aktywna_strone();
              aktywuj_proce(A3);
              break;
            case A10: //Spódnica prawej procy
              zamien_aktywna_strone();
              aktywuj_proce(A4);
              break;
          }
        }
      }
    }

    ostatni_odczyt[i] = nowy_odczyt;
  }

  if(millis() - moment_odtworzenia > dlugosci_dzwiekow[dzwiek] && gra_rozpoczeta && !czy_petla)
    odtworz_dzwiek(2,true);  
  
}

void dodaj_punkty(int punkty) {
  wynik += punkty;
  lcd.setCursor(0, 0);
  if(gra_rozpoczeta)
    lcd.print(wynik);
}

void zapal_leda(int pinLeda) {
  digitalWrite(pinLeda,HIGH);
}

void zgas_leda(int pinLeda) {
  digitalWrite(pinLeda,LOW);
}

void restart_gry() {
  wynik = 0;
  kulka = 0;
  dodatkowa_kulka = false;
  bonus = 0;
  wspolczynnik_bonus = 1;

  dodatkowa_kulka_gotowa = false;
  for(int i=0; i<3; i++) {
    trzy_grzybki[i] = false;
    trzy_przetoki[i] = false;
  }
  ukonczone_cele = 0;
  uderzenia_w_grzybki = 0;

  koniec_gry = false;
  gra_rozpoczeta = true;
  odtworz_dzwiek(2,true);
  delay(500);


  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.print(wynik);
  dodaj_punkty(0);
  lcd.setCursor(0, 1);
  lcd.print("     BALL      ");
  lcd.setCursor(10, 1);
  lcd.print(kulka+1);

  for(int i=0; i<35; i++)
    zgas_leda(piny_wyjsciowe[i]);
  zapal_leda(6);
  zapal_leda(7);
  zapal_leda(35);
  zapal_leda(37);
  ustaw_aktywna_strone(false);

  aktywuj_wyrzutnie();
}

void kulka_stracona() {
  odtworz_dzwiek(10,false);
  int obliczony_bonus = 1000*bonus*wspolczynnik_bonus;
  dodaj_punkty(obliczony_bonus);

  lcd.setCursor(0, 0);
  lcd.print("  BONUS    X    ");
  lcd.setCursor(13, 0);
  lcd.print(wspolczynnik_bonus);
  lcd.setCursor(0, 1);
  lcd.print("+               ");
  lcd.setCursor(1, 1);
  lcd.print(obliczony_bonus);
  delay(2000);

  if(dodatkowa_kulka) {
    lcd.setCursor(0, 0);
    lcd.print("  SHOOT  AGAIN  ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(1000);
    dodatkowa_kulka = false;
  }
  else
    kulka++;

  
  bonus = 0;
  wspolczynnik_bonus = 1;
  dodatkowa_kulka_gotowa = false;
  ukonczone_cele = 0;
  for(int i=0; i<3; i++) {
    trzy_przetoki[i] = false;
    zgas_leda(ledy_przetok[i]);
  }
  
  zgas_leda(16);
  zgas_leda(17);
  ledy_bonus();

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(wynik);

  if(kulka<3) {
    lcd.setCursor(0, 1);
    lcd.print("     BALL      ");
    lcd.setCursor(10, 1);
    lcd.print(kulka+1);
    aktywuj_wyrzutnie();
    odtworz_dzwiek(2,true);
  }
  else {
    koniec_gry = true;
    for(int i=0; i<3; i++)
    trzy_grzybki[i] = false;
    zgas_leda(29);
    zgas_leda(31);
    zgas_leda(33);
    lcd.setCursor(0, 0);
    lcd.print("   GAME  OVER   ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Score:          ");
    lcd.setCursor(7, 1);
    lcd.print(wynik);
    odtworz_dzwiek(3,false);
    czas_konca_gry = millis();
    gra_rozpoczeta = false;

    delay(5000);    

    lcd.setCursor(0, 0);
    lcd.print("     SZTORM     ");
    lcd.setCursor(0, 1);
    lcd.print("  PRESS  START  ");
    odtworz_dzwiek(1,true);
  }
}

void zwieksz_wspolczynnik() {
  if(wspolczynnik_bonus<5)
    wspolczynnik_bonus++;
  else
    dodaj_punkty(10000);
  
  ledy_bonus();
}

void zwieksz_bonus(int ile) {
  if(bonus<39)
    bonus += ile;
  
  ledy_bonus();
}

void ledy_bonus() {
  for(int i=1; i<10; i++) {
    if(bonus%10>=i)
      zapal_leda(42+i);
    else
      zgas_leda(42+i);      
  }

  if(bonus>=30 || (bonus>=10 && bonus<20))
    zapal_leda(52);
  else
    zgas_leda(52);
  
  if(bonus>=20)
    zapal_leda(53);
  else
    zgas_leda(53);
  
  for(int i=0; i<4; i++) {
    if(wspolczynnik_bonus>=i+2)
      zapal_leda(ledy_wsp[i]);
    else
      zgas_leda(ledy_wsp[i]);      
  }  
}

void ustaw_aktywna_strone(bool lewa) {
  aktywna_lewa = lewa;

  if(aktywna_lewa) {
    zapal_leda(39);
    zgas_leda(41);
  }
  else {
    zapal_leda(41);
    zgas_leda(39);
  }
}

void modul_mp3(byte CMD, byte para1, byte para2) {
  word checksum = -(0xFF + 0x06 + CMD + 0x00 + para1 + para2);
  byte komenda[10] = { 0x7E, 0xFF, 0x06, CMD, 0x00, para1, para2, highByte(checksum), lowByte(checksum), 0xEF};
  for (byte i=0; i<10; i++) {
    Ss.write(komenda[i]);
  }
}

// 1 - attract mode, 2 - gra, 3 - koniec gry, 4 - cel 1, 5 - cel 2, 6 - przetoka 1, 7 - przetoka 2,
// 8 - przetoka 3, 9 - dodatkowa kulka, 10 - koniec rundy
void odtworz_dzwiek(int dzw, bool petla) {
  dzwiek = dzw;
  czy_petla = petla;
  moment_odtworzenia = millis();
  if(petla)
    modul_mp3(0x08,0,dzw);
  else
    modul_mp3(0x03,0,dzw);
  delay(10);
}

void aktywuj_grzybek(int pinGrzybka) {
  uderzenia_w_grzybki++;
  if(uderzenia_w_grzybki>=30)
    zapal_leda(29);
  if(uderzenia_w_grzybki>=50)
    zapal_leda(31);
  if(uderzenia_w_grzybki>=80)
    zapal_leda(33);
  
  digitalWrite(pinGrzybka,HIGH);
  delay(100);
  digitalWrite(pinGrzybka,LOW);
}

void aktywuj_proce(int pinProcy) {
  dodaj_punkty(200);
  
  digitalWrite(pinProcy,HIGH);
  delay(100);
  digitalWrite(pinProcy,LOW);
}

void aktywuj_wyrzutnie() {
  digitalWrite(A5,HIGH);
  delay(1000);
  digitalWrite(A5,LOW);
}

void sprawdz_trzy_przetoki(int przetoka) {
  if(trzy_przetoki[przetoka])
    dodaj_punkty(500);
  else
    dodaj_punkty(2000);
  
  trzy_przetoki[przetoka] = true;
  zapal_leda(ledy_przetok[przetoka]);

  bool ukonczone = trzy_przetoki[0] && trzy_przetoki[1] && trzy_przetoki[2];
  if(ukonczone) {
    odtworz_dzwiek(8,false);
    zwieksz_wspolczynnik();
    for(int i=0; i<3; i++) {
      trzy_przetoki[i] = false;
      zgas_leda(ledy_przetok[i]);
    }
  }
  else
    odtworz_dzwiek(7,false);
}

void sprawdz_cele(bool dod_kulka) {
  dodaj_punkty(1000);
  ukonczone_cele++;

  if(dod_kulka && dodatkowa_kulka_gotowa) {
    odtworz_dzwiek(9,false);
    dodatkowa_kulka = true;
    dodatkowa_kulka_gotowa = false;
    zapal_leda(16);
    zgas_leda(17);
  }

  if(ukonczone_cele==15) {
    dodatkowa_kulka_gotowa = true;
    zapal_leda(17);
  }
  zwieksz_bonus(1);
  if(ukonczone_cele%5==0)
    zwieksz_bonus(1);
  if(ukonczone_cele%3==0)
    zwieksz_bonus(1);
}

void zamien_aktywna_strone() {
  aktywna_lewa = !aktywna_lewa;
  Serial.println(aktywna_lewa);

  if(aktywna_lewa) {
    zapal_leda(39);
    zgas_leda(41);
  }
  else {
    zapal_leda(41);
    zgas_leda(39);
  }
}