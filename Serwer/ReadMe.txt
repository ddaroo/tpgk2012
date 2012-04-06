SERWER SCRABBLE


CODING GUIDELINES

Złota reguła
Nie ma gorszego zła niż powielanie kodu. Kto rozpoczyna dodawanie ficzera od skopiowania kawałka istniejącego kodu, ten już przegrał.

Dobre rady
* Unikamy wskaźników w stylu C. Gdzie to możliwe, stosujemy semantykę wartości, w dalszej kolejności sprytne wskaźniki.
* Unikamy przerośniętych funkcji. Wydzielenie kawałka kodu do nazwanej funkcji jest ważnym czynnikiem samodokumentujacym kod.
* Kodowanie znaków dla plików to utf8.
* Polskie komentarze także tych do commit-ów.

Formatowanie
Wcięcia robimy tabami. Tab odpowiada czterem spacjom.

Językowe
Dopuszczamy nowe funkcjonalności ze standardu C++11, które są wspierane zarówno przez VC10 jak i GCC 4.5. Obejmuje to lambda-wyrażenia, referencje do r-wartości, deklarator auto, decltype i parę innych drobiazgów.



