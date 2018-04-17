    /*
     * RYHMÄ 3
     * Timer-tehtävä keskeytyksillä, ilman valmista kirjastoa. Tehtävässä käytetty 16-bittistä timeriä (Timer1).
     * Keskeytys tapa tässä esimerkissä: Compare Match Interrupt (kts. ATMEGA -datalehti s. 57)
     * Muita mahdollisia keskeytys tapoja ovat Overflow Interrupt ja Input Capture Interrupt.
     */

    #include < Time.h >
    #include < TimeLib.h >

        // Funktio ajan tulostamiseksi sarjamonitorille.
        void printTime() {

            char timeFormat[9];

         // Sprintf -funktio muodostaa saman tyylisen merkkijonon, kuten esim. c-kielen printf -funktio, mutta ei kuitenkaan itsessään tulosta mitään, vaan haluttu sisältö tallennetaan merkkijonona.
            sprintf(timeFormat, "%02d:%02d:%02d", hour(), minute(), second());

         // Tulostetaan aika sarjamonitorille
            Serial.println(timeFormat);
        }

    void setup() {

            Serial.begin(9600);

        // Poistetaan keskeytykset käytöstä
            cli();

        // Alustetaan TCCR1A, TCCR1B ja TCCR1C arvoon 0 (kaikki bitit 0).
            TCCR1A = TCCR1B = TCCR1C = 0;

        // Alustetaan timer counter arvoon 0
            TCNT1 = 0;


        /*
         * Asetetaan keskeytykset tapahtumaan 1s välein.
         * Laskutoimituksia:
         * 
         * Keskeytys taajuus (Hz) = 16MHz / (prescaler * (OCR1A + 1)), +1 koska compare match rekisteri alkaa indeksistä 0
         * 16MHz / 1024 * (15624 + 1) = 1Hz, eli keskeytys tapahtuu sekunnin välein.
         * 
         * OCR1A = 16MHz/ (prescaler * keskeytys taajuus) - 1
         * [16MHz / (1024 * 1Hz)] - 1 = 15624
         * 
         * 1/(16MHz/1024) = 0.000064s, eli kun prescalerin arvoksi on asetettu 1024, niin timer counterin arvo inkrementoituu +1 aina 64 mikrosekunnin välein.
         * 1/0.000064 = 15625 (1 sekunti jaettuna 0.000064 sekunnilla on 15625, eli yhden sekunnin aikana timer counter inkrementoituu 15625 kertaa (0-15624))
         */

        /*
         * OCR1AH and OCR1AL – Output Compare Register 1 A
         * Output Compare Rekisteri (OCR1A) on 16 bittinen rekisteri, jonka sisältämää arvoa verrataan jatkuvasti timer counterin (TCNT1) arvoon.
         * Kun timer counterin (TCNT1) arvo vastaa Output Compare rekisteriin (OCR1A) alustettua arvoa, tapahtuu keskeytys (jos Timer Counter Interrupt Mask (TIMSK1) rekisterin bitti 1, eli OCIE1A = 1).
         */
            OCR1A = 15624; // Arvon pitää olla aina pienempi kuin 65536, koska 16:ta bitillä voidaan esittää vain luvut 0-65535 (max. 1111 1111 1111 1111)


        // Asetetaan prescalerin arvoksi 1024, laittamalla Timer Counter Control (TCCR1B) rekisterin bitit 2 ja 0 päälle (CS12 ja CS10).
            TCCR1B |= (1 << CS12) | (1 << CS10); // TCCR1B Bit 2 (CS12) = 1, TCCR1B Bit 0 (CS10) = 1


        // Laitetaan CTC mode päälle (Clear Timer On Compare Match).
            TCCR1B |= (1 << WGM12); // TCCR1B Bit 3 (WGM12) = 1


        // Kun tämä bitti (TIMSK1 Bit 1 – OCIE1A) on 1 ja Status rekisterin bitti numero 7 (SREG Bit 7 – I) on 1, niin Timer Counterin Output Compare A Match Interrupt on sallittu.
            TIMSK1 |= (1 << OCIE1A); // TIMSK1 Bit 1 (OCIE1A) = 1

        // Otetaan keskeytykset käyttöön
            sei();
    }

        // ISR = Interrupt Service Routine, TIMER1_COMPA_vect = keskeytysvektori
    ISR(TIMER1_COMPA_vect) {
        // Kun keskeytys tapahtuu -> kutsutaan funktio, joka tulostaa ajan sarjamonitorille
            printTime();
    }

    void loop() {
        // Nothing to see here... :)
    }
