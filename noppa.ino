#define NOPAN_SUURIN_LUKU 6 // Nopassa on numerot 1-6
#define NAPIN_PINNI 3
#define EFEKTIN_KIERROSTEN_LKM 2
#define EFEKTIN_NOPEUS 50 // Millisekuntia 

// Määritellään logiikat joilla ohjataan 7-segmenttinäyttöä näyttämään nopan numerot 1-6
// PORTD neljänneksi vähiten merkitsevän bitin arvo on 1, koska se ohjaa napin pinniä 3
byte segmentit[6][2] = {
  {0x0,  0x68}, // Numero 1 (0x0  = B00000000, 0x68 = B01101000)
  {0x5,  0xB8}, // Numero 2 (0x5  = B00000101, 0xB8 = B10111000)
  {0x4,  0xF8}, // Numero 3 (0x4  = B00000100, 0xF8 = B11111000)
  {0x6,  0x68}, // Numero 4 (0x6  = B00000110, 0x68 = B01101000)
  {0x6,  0xD8}, // Numero 5 (0x6  = B00000110, 0xD8 = B11011000)
  {0x17, 0xD8}  // Numero 6 (0x17 = B00010111, 0xD8 = B11011000)
};

// Palauttaa boolean arvon (true tai false) sen mukaan onko pinniin 3 kytketty painonappi alhaalla vai ylhäällä. Funktio ei ota vastaa parametreja.
boolean nappiOnPainettu(void) {
    // Palautetaan invertoitu arvo, koska käytetään pinModea INPUT_PULLUP
    return !digitalRead(NAPIN_PINNI);
}

// Palauttaa kokonaisluku satunnaisluvun (random integer) väliltä 1 - 6. Funktio ei ota vastaa parametreja.
int satunnaisluku(void) {
    return random(NOPAN_SUURIN_LUKU);
}

// Kirjoittaa annetun integer luvun mukaisen luvun 7-segmenttinäytölle, joka on kytketty Arduinon pinneihin 4-10
void kirjoitaLukuSevenSegmentille(int luku) {
    PORTB = segmentit[luku][0]; // PORTB = 8-10
    PORTD = segmentit[luku][1]; // PORTD = 4-7
}

// Sammuta kaikki segmentin ledit, eli muuttaa kaikki bitit arvoon 0. Funktio ei ota vastaa parametreja.
void sammuta(void) {
    PORTD,
    PORTB = 0x0; // 0x0 = B00000000
}

// Kiertoefekti, joka sytyttää vuoron perään segmentit a,b,c,d,e,f, eli muuttaa ne bitit arvoon 1, jotka ohjaavat pinnejä 4,5,6,7,8,9
// Funktiolle annetaan seuraavat parametrit: efektin kierrosten lukumäärä ja toistonopeus
void efekti(int kierrokset, int nopeus) {
    int laskuri = 0;
    // Pyöritään while -loopissa niin kauan, kuin toistolauseke (laskuri < kierrokset) on tosi
    while (laskuri < kierrokset) {
        // Kutsutaan sammuta -funktio
        sammuta();
        for (int i = 0; i < 6; i++) {
            // Siirretään bittiä jonka arvo on 1 joka kierroksella yhden kerran vasemmalle
            PORTD = 0x10 << i; // 0x10 = B00010000
            // Jos for -loopin kierros on > 3
            if (i > 3) {
                // Siirretään bittiä jonka arvo on 1 joka kierroksella yhden kerran vasemmalle
                PORTB = 0x1 << i - 4; // 0x1 = B00000001
            }
            // Viive ennen koodin suorittamisen jatkamista
            delay(nopeus);
            // Kutsutaan sammuta -funktio
            sammuta();
        }
        // Lisätään joka while -loopin kierroksella laskurin arvoa yhdellä
        laskuri++;
    }
}

// Arduinon oma setup -funktio, jossa määritellään arduinon asetukset. Funktio ei ota vastaa parametreja.
void setup(void) {
    // Asetetaan pinni johon nappi on kytketty INPUT_PULLUP tilaan
    // INPUT_PULLUP invertoi input tilan toiminnan
    // https://www.arduino.cc/en/Tutorial/DigitalPins -> Properties of Pins Configured as INPUT_PULLUP
    pinMode(NAPIN_PINNI, INPUT_PULLUP);
    // Tässä määritellään, että numeroiden arpominen alkaa joka kerta satunnaisesta numerosta
    // Arvonnan alkuarvo saadaan analogisesta pinnistä 0, jonka arvo kelluu, joten sen arvo ei ole koskaan sama
    randomSeed(analogRead(0));
    // 7-segmenttinäyttö on kytketty pinneihin 4-10, joten asetetaan ne output tilaan
    DDRB = 0x7; // Aseta PORTB pinnit 8-10 (0x7 = B00000111) output tilaan
    DDRD = 0xF0; // Aseta PORTD pinnit 4-7 (0xF0 = B11110000) output tilaan
}

// Arduinon oma loop -funktio, jossa suoritetaan ohjelmakoodia niin kauan kunnes virta katkaistaan. Funktio ei ota vastaa parametreja.
void loop(void) {
    // Jos nappiOnPainettu -funktio palauttaa boolean arvon TRUE, eli nappia on painettu, niin suoritetaan satunnaisluvun arvonta ja kirjoitetaan se 7-segmenttinäytölle
    // Arvottu numero säilyy näytöllä niin kauan, että nappia painetaan uudelleen
    if (nappiOnPainettu() == true) {
        // Määritellään kokonaislukumuuttuja (nopanLuku), johon tallennetaan satunnaisluku -funktion palauttama satunninen kokonaisluku
        int nopanLuku = satunnaisluku();
        // Kutsutaan efekti -funktio
        efekti(EFEKTIN_KIERROSTEN_LKM, EFEKTIN_NOPEUS);
        // Kutsutaan kirjoitaLukuSevenSegmentille -funktio ja syötetään sille parametriksi aiemmin arvottu satunnailuku
        kirjoitaLukuSevenSegmentille(nopanLuku);
    }
}
