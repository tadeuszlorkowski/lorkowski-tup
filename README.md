# lorkowski-tup
Technika mikroprocesorowa projekt, Elektronika i Telekomunikacja, stacjonarne, rok III, sem. 5 - odrabiany przedmiot - Tadeusz Lorkowski 47530 (I rok Elektronika i Telekomunikacja MSU)

Udostępniony program, w postaci kodu źródłowego Arduino C (PinballSoft.ino), stanowi oprogramowanie układowe dla płyty ewaluacyjnej Arduino Mega 2560, będącej sterownikiem dla obwodu elektroniczego automatu flipper.

Więcej na ten temat w pracy dyplomowej, w pliku Praca_dyplomowa2901.pdf.

## Połączenia elektroniczne
Poniżej znajduje sie lista połączeń elektronicznych i ich funkcje w grze. Schemat elektroniczy połączeń znajduje się w pliku caly_uklad_flippera.png.

| Pin Arduino | Podłączony element | Funkcja w automacie |
| -------- | ------- | ------- |
| D0 | Wyświetlacz LCD ze sterownikiem HD44780 - RS | Sterowanie wyświetlaczem |
| D1 | Wyświetlacz LCD ze sterownikiem HD44780 - E | Sterowanie wyświetlaczem |
| D2 | Wyświetlacz LCD ze sterownikiem HD44780 - D4 | Sterowanie wyświetlaczem |
| D3 | Wyświetlacz LCD ze sterownikiem HD44780 - D5 | Sterowanie wyświetlaczem |
| D4 | Wyświetlacz LCD ze sterownikiem HD44780 - D6 | Sterowanie wyświetlaczem |
| D5 | Wyświetlacz LCD ze sterownikiem HD44780 - D7 | Sterowanie wyświetlaczem |
| D6 | LED27 - niebieska | Dioda LED na szkle tylnym |
| D7 | LED26 - biała | Dioda LED na szkle tylnym |
| D8 | S12 | Lewy cel na środku stołu |
| D9 | S11 | Prawy cel na środku stołu |
| D10 | DFPlayer Mini - RX | Sterowanie modułem |
| D11 | DFPlayer Mini - TX | Sterowanie modułem |
| D12 | S10 | Przetoka przy lewym pasie końcowym |
| D13 | S9 | Przetoka przy prawym pasie końcowym |
| D14 | S13 | Przetoka przy lewym pasie flippera |
| D15 | S14 | Przetoka przy prawym pasie flippera |
| D16 | LED28 - czerwona | Lampa “strzelaj ponownie” |
| D17 | LED29 - czerwona | Lampa dodatkowej kulki |
| D18 | S15 | Bank 1, cel 1 |
| D19 | S16 | Bank 1, cel 2 |
| D20 | nieużywany | brak funkcji w grze |
| D21 | nieużywany | brak funkcji w grze |
| D22 | S17 | Bank 1, cel 3 |
| D23 | LED5 - żółta | Lampa przetoki górnej 1 |
| D24 | S18 | Przetoka górna 1 |
| D25 | LED6 - żółta | Lampa przetoki górnej 2 |
| D26 | S19 | Przetoka górna 2 |
| D27 | LED7 - żółta | Lampa przetoki górnej 3 |
| D28 | S20 | Przetoka górna 3 |
| D29 | LED8 - niebieska | LED w grzybku 1 |
| D30 | S21 | Bank 2, cel 1 |
| D31 | LED9 - niebieska | LED w grzybku 2 |
| D32 | S22 | Bank 2, cel 2 |
| D33 | LED10 - biała | LED w grzybku 3 |
| D34 | S23 | Bank 2, cel 3 |
| D35 | LED11 - niebieska | LED w lewej procy |
| D36 | S24 | Bank 3, cel 1 |
| D37 | LED12 - niebieska | LED w prawej procy |
| D38 | S25 | Bank 3, cel 2 |
| D39 | LED13 - żółta | Lampa lewej przetoki |
| D40 | S26 | Bank 3, cel 3 |
| D41 | LED14 - żółta | Lampa prawej przetoki |
| D42 | S27 | (Re)start gry |
| D43 | LED15 - zielona | Lampa bonusu 1 |
| D44 | LED25 - zielona | Lampa bonusu 2 |
| D45 | LED16 - zielona | Lampa bonusu 3 |
| D46 | LED24 - zielona | Lampa bonusu 4 |
| D47 | LED17 - zielona | Lampa bonusu 5 |
| D48 | LED23 - zielona | Lampa bonusu 6 |
| D49 | LED18 - zielona | Lampa bonusu 7 |
| D50 | LED22 - zielona | Lampa bonusu 8 |
| D51 | LED19 - zielona | Lampa bonusu 9 |
| D52 | LED21 - żółta | Lampa bonusu 10 |
| D53 | LED20 - czerwona | Lampa bonusu 20 |
| A0 | L1 | „Grzybek”- elektromagnes ciągnący 1  |
| A1 | L2 | „Grzybek”- elektromagnes ciągnący 2 |
| A2 | L3 | „Grzybek”- elektromagnes ciągnący 3 |
| A3 | L4 | Lewa „proca” - elektromagnes pchający |
| A4 | L5 | Prawa „proca” - elektromagnes pchający |
| A5 | L6 | Wyrzutnia kulek - elektromagnes pchający |
| A6 | S8 | „Spódnica” „grzybka” 1 |
| A7 | S7 | „Spódnica” „grzybka” 2 |
| A8 | S6 | „Spódnica” „grzybka” 3 |
| A9 | S5 | „Spódnica” lewej „procy” |
| A10 | S4 | „Spódnica” prawej „procy” |
| A11 | S3 | Przełącznik końca rundy |
| A12 | LED1 - niebieska | Lampa mnożnika x2 |
| A13 | LED2 - niebieska | Lampa mnożnika x3 |
| A14 | LED3 - niebieska | Lampa mnożnika x4 |
| A15 | LED4 - niebieska | Lampa mnożnika x5 |

gdzie:
- Dn - pin cyfrowy Arduino;
- An - pin analogowy Arduino;
- Ln - baza tranzystora TIP 120, będącego elementem tranzystorowego sterownika solenoidu, połączona szeregowo z rezystorem 1 kΩ;
- LEDn - dioda LED połączona szeregowo z rezystorem 220 Ω;
- Sn - przycisk (o przystosowanym do elementu flippera kształcie) połączony od drugiej strony z pinem 5V oraz zwarty do masy rezystorem 10 kΩ.

## Konfiguracja układu DFPlayer Mini
Przed uruchomieniem urządzenia do slotu należy włożyć kartę microSD, na której znajdują się wyłącznie pliki z katalogu Sounds. Karta musi zostać sformatowana zgodnie z systemem partycjonowania FAT16 lub FAT32. Pliki na karcie należy umieścić luzem.
