#include <LiquidCrystal.h>
#include "SoftwareSerial.h"

LiquidCrystal lcd(0,1,2,3,4,5);
SoftwareSerial Ss(10, 11);

unsigned long wynik = 0; // Wynik gry (punktacja)
int kulka = 0; // Numer rundy
bool koniec_gry = false; // Czy minęły trzy rundy i gra się skończyła?
bool gra_rozpoczeta = false; // Czy gra w ogóle jest rozpoczęta?
bool dodatkowa_kulka = false; // Czy wygrano dodatkową rundę?
int bonus = 0; // Bonusowe punkty na końcu rundy/1000
int wspolczynnik_bonus = 1; // Mnożnik bonusowych punktów

bool aktywna_lewa = false; // false - punktacja zwiększona po prawej stronie; true - po lewej
bool dodatkowa_kulka_gotowa = false; // Czy można wygrać dodatkową rundę, trafiając w odpowiedni cel?
bool trzy_grzybki[3] = {false, false, false}; // Czy aktywowano zwiększoną punktację grzybków?
bool trzy_przetoki[3] = {false, false, false}; // Czy kulka przeszła przez wszystkie trzy przyciski przetok przynajmniej raz?
int ukonczone_cele = 0; // Liczba trafionych celów
int uderzenia_w_grzybki = 0; // Liczba trafień w "grzybki"
unsigned long czas_konca_gry = 0; // Czas od zakończenia gry do przejścia w tryb zachęcania do gry (attract mode)

// Oznaczenia pinów Arduino do zadeklarowania jako piny wejściowe
const int piny_wejsciowe[25] = {8, 9, 12, 13, 14, 15, 18, 19, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, A6, A7, A8, A9, A10, A11};
// Tablice odczytów z pinów wejściowych
int odczyt[25] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
int ostatni_odczyt[25] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
// Czas od ostatniego wciśnięcia pinów wejściowych
unsigned long czas_od_wcisniecia[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Oznaczenia pinów Arduino do zadeklarowania jako piny wyjściowe
const int piny_wyjsciowe[35] = {6, 7, 16, 17, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, A0, A1, A2, A3, A4, A5, A12, A13, A14, A15};
// Oznaczenia pinów połączonych z diodami LED przetok
const int ledy_przetok[3] = {23,25,27};
// Oznaczenia pinów połączonych z diodami LED wskazującymi na wartość mnożnika bonusowych punktów
const int ledy_wsp[4] = {A12, A13, A14, A15};

// Numer dźwięku do odtworzenia
int dzwiek = 1;
// Czy dźwięk ma grać w pętli?
bool czy_petla = false;
// Długości poszczególnych dźwięków [ms]
const unsigned long dlugosci_dzwiekow[10] = {3000,2000,2000,4000,3500,4000,5000,3000,4000,3000};
// Milisekunda działania Arduino, w której odtworono dźwięk
unsigned long moment_odtworzenia = 0;

void setup() {
  Ss.begin(9600);

  // konfiguracja układu DFPlayer
  delay(500);
  modul_mp3(0x3F, 0, 0);
  delay(500);
  modul_mp3(0x06,0,15);
  delay(500);
  modul_mp3(0x08,0,1);
  delay(500);

  // konfiguracja modułu LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("     SZTORM     ");
  lcd.setCursor(0, 1);
  lcd.print("  PRESS  START  ");

  // deklaracja kierunków pinów Arduino
  for(int i=0; i<25; i++)
    pinMode(piny_wejsciowe[i], INPUT);
  for(int i=0; i<35; i++)
    pinMode(piny_wyjsciowe[i], OUTPUT);

  // zapalenie niektórych diod LED
  zapal_leda(6);
  zapal_leda(7);
  zapal_leda(35);
  zapal_leda(37);
}

void loop() {

  // Sprawdzenie, czy któryś z elementów został aktywowany
  for(int i=0; i<25; i++) {
    int nowy_odczyt = digitalRead(piny_wejsciowe[i]);

    if (nowy_odczyt != ostatni_odczyt[i]) {
      czas_od_wcisniecia[i] = millis();
    }

    // Debouncing programowy - sprawdzenie danego pinu najwcześniej 50 ms od ostatniej zmiany.
    if (millis() - czas_od_wcisniecia[i] > 50) {
      if (nowy_odczyt != odczyt[i]) {
        odczyt[i] = nowy_odczyt;
        // Jako że wszystkie elementy są aktywowane jednym impulsem, sprawdza się jedynie zmianę stanu na wysoki.
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

// Zwiększenie wyniku
void dodaj_punkty(int punkty) {
  wynik += punkty;
  lcd.setCursor(0, 0);
  if(gra_rozpoczeta)
    lcd.print(wynik);
}

// Zapalenie diody LED
void zapal_leda(int pinLeda) {
  digitalWrite(pinLeda,HIGH);
}

// Zgaszenie diody LED
void zgas_leda(int pinLeda) {
  digitalWrite(pinLeda,LOW);
}

// Rozpoczęcie nowej gry
void restart_gry() {
  // Wyzerowanie postępów gry
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

  // Konfiguracja wyświetlacza i diod LED
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

  // Wprowadzenie kuli do gry
  aktywuj_wyrzutnie();
}

// Koniec rundy - kula znajduje się w "fartuchu"
void kulka_stracona() {
  odtworz_dzwiek(10,false);

  // Dodanie bonusu na koniec rundy
  int obliczony_bonus = 1000*bonus*wspolczynnik_bonus;
  dodaj_punkty(obliczony_bonus);

  // Konfuguracja wyświetlacza
  lcd.setCursor(0, 0);
  lcd.print("  BONUS    X    ");
  lcd.setCursor(13, 0);
  lcd.print(wspolczynnik_bonus);
  lcd.setCursor(0, 1);
  lcd.print("+               ");
  lcd.setCursor(1, 1);
  lcd.print(obliczony_bonus);
  delay(2000);

  // Jeżeli przysługuje dodatkowa runda - przypomnij o tym graczowi i nie zwiększaj numeru rundy, w przeciwnym razie zwiększ numer rundy
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

  // Wyzeruj już dodany bonus i postęp trzech przetok
  bonus = 0;
  wspolczynnik_bonus = 1;
  dodatkowa_kulka_gotowa = false;
  ukonczone_cele = 0;
  for(int i=0; i<3; i++) {
    trzy_przetoki[i] = false;
    zgas_leda(ledy_przetok[i]);
  }

  // Ponowna konfiguracja LED-ów i wyświetlacza
  zgas_leda(16);
  zgas_leda(17);
  ledy_bonus();

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(wynik);

  if(kulka<3) {
    // Jeżeli gra się nie skończyła (nie minęły trzy podstawowe oraz wszystkie dodatkowe rundy) - wprowadź nową kulkę
    lcd.setCursor(0, 1);
    lcd.print("     BALL      ");
    lcd.setCursor(10, 1);
    lcd.print(kulka+1);
    aktywuj_wyrzutnie();
    odtworz_dzwiek(2,true);
  }
  else {
    // Koniec gry - poinformuj o tym gracza i pokaż jego końcowy wynik
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

    // Po 5 s od końca gry ponownie przejdź w tryb zachęcania do gry (attract mode)
    delay(5000);
    lcd.setCursor(0, 0);
    lcd.print("     SZTORM     ");
    lcd.setCursor(0, 1);
    lcd.print("  PRESS  START  ");
    odtworz_dzwiek(1,true);
  }
}

// Zwiększ mnożnik bonusu na koniec rundy (max 5 - jeżeli już wynosi 5, dodaj zamiast tego 10000 punktów)
void zwieksz_wspolczynnik() {
  if(wspolczynnik_bonus<5)
    wspolczynnik_bonus++;
  else
    dodaj_punkty(10000);
  
  ledy_bonus();
}

// Zwiększ bonus o ile*1000
void zwieksz_bonus(int ile) {
  if(bonus<39)
    bonus += ile;
  
  ledy_bonus();
}

// Zaktualizuj diody LED informujące o wysokości bonusu
void ledy_bonus() {
  // liczba zapalonych zielonych diod - liczba jedności
  for(int i=1; i<10; i++) {
    if(bonus%10>=i)
      zapal_leda(42+i);
    else
      zgas_leda(42+i);      
  }

  // żółta dioda - 10
  if(bonus>=30 || (bonus>=10 && bonus<20))
    zapal_leda(52);
  else
    zgas_leda(52);

  // czerwona dioda - 20
  if(bonus>=20)
    zapal_leda(53);
  else
    zgas_leda(53);

  // niebieskie diody - mnożnik bonusu
  for(int i=0; i<4; i++) {
    if(wspolczynnik_bonus>=i+2)
      zapal_leda(ledy_wsp[i]);
    else
      zgas_leda(ledy_wsp[i]);      
  }  
}

// zmiana strony stołu, po której można otrzymać więcej punktów
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

// wykonanie polecenia modułu DFPlayer Mini
void modul_mp3(byte CMD, byte para1, byte para2) {
  word checksum = -(0xFF + 0x06 + CMD + 0x00 + para1 + para2);
  byte komenda[10] = { 0x7E, 0xFF, 0x06, CMD, 0x00, para1, para2, highByte(checksum), lowByte(checksum), 0xEF};
  for (byte i=0; i<10; i++) {
    Ss.write(komenda[i]);
  }
}

// odtworzenie dźwięku
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

// aktywacja "grzybka"
void aktywuj_grzybek(int pinGrzybka) {
  // liczenie uderzeń w "grzybki" - duża ich ilość zwiększa punktację za dalsze ich trafianie - dioda LED w danym "grzybku" zostanie zapalona
  uderzenia_w_grzybki++;
  if(uderzenia_w_grzybki>=30)
    zapal_leda(29);
  if(uderzenia_w_grzybki>=50)
    zapal_leda(31);
  if(uderzenia_w_grzybki>=80)
    zapal_leda(33);

  // aktywuj solenoid "grzybka" i odepchnij kulkę
  digitalWrite(pinGrzybka,HIGH);
  delay(100);
  digitalWrite(pinGrzybka,LOW);
}

// aktywacja "procy" - dodanie 200 punktów i odepchnięcie kulki
void aktywuj_proce(int pinProcy) {
  dodaj_punkty(200);
  
  digitalWrite(pinProcy,HIGH);
  delay(100);
  digitalWrite(pinProcy,LOW);
}

// przekazanie kulki z "fartucha" do ręcznej wyrzutni sprężynowej
void aktywuj_wyrzutnie() {
  digitalWrite(A5,HIGH);
  delay(1000);
  digitalWrite(A5,LOW);
}

// po przejściu przez kulę jednej z trzech przetok nagródź gracza
void sprawdz_trzy_przetoki(int przetoka) {
  // jeżeli już trafiono, + 500 punktów, w przeciwnym razie + 2000 punktów
  if(trzy_przetoki[przetoka])
    dodaj_punkty(500);
  else
    dodaj_punkty(2000);

  // zapisz postęp
  trzy_przetoki[przetoka] = true;
  zapal_leda(ledy_przetok[przetoka]);

  // jeżeli trafiono wszystkie trzy - zwiększ współczynnik bonusu
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

// po trafieniu przez przez kulę jednego z celów nagródź gracza
void sprawdz_cele(bool dod_kulka) {
  // dodaj 1000 punktów i zwiększ liczbę trafionych celów
  dodaj_punkty(1000);
  ukonczone_cele++;

  // jeżeli przyznano możliwość przedłużenia gry o dodatkową rundę, rzeczywiście ją przyznaj
  if(dodatkowa_kulka_gotowa) {
    odtworz_dzwiek(9,false);
    dodatkowa_kulka = true;
    dodatkowa_kulka_gotowa = false;
    zapal_leda(16);
    zgas_leda(17);
  }

  // jeżeli trafiono cele 15 razy - daj możliwość przedłużenia gry o dodatkową rundę
  if(ukonczone_cele==15) {
    dodatkowa_kulka_gotowa = true;
    zapal_leda(17);
  }
  // co każdy trafiony cel zwiększ bonus o 1
  zwieksz_bonus(1);
  // co trzeci i co piąty trafiony cel zwiększ bonus jeszcze o 1
  if(ukonczone_cele%5==0)
    zwieksz_bonus(1);
  if(ukonczone_cele%3==0)
    zwieksz_bonus(1);
}

// zamień bardziej punktowaną stronę
void zamien_aktywna_strone() {
  aktywna_lewa = !aktywna_lewa;

  if(aktywna_lewa) {
    zapal_leda(39);
    zgas_leda(41);
  }
  else {
    zapal_leda(41);
    zgas_leda(39);
  }
}
