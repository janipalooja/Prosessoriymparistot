    /*
     * RYHMÄ 3
     * Timer-tehtävä keskeytyksillä, ilman valmista kirjastoa. Tehtävässä käytetty 16-bittistä timeriä (Timer1).
     * Keskeytys tapa tässä esimerkissä: Compare Match Interrupt (kts. ATMEGA -datalehti s. 57)
     * Muita mahdollisia keskeytys tapoja ovat Overflow Interrupt ja Input Capture Interrupt.
     * 
     * Koodi on tarkoituksella HIEMAN "yli kommentoitu", jotta saadaan tuotua esille tehtävän annossa vaadittu ymmärrys timerin toiminnallisuudesta :)
     */
    /*
     * Kirjastot jotka mahdollistavat ajan esittämisen hour(), minute() ja second() funktioiden avulla.
     */
    #include < Time.h >
    #include < TimeLib.h >

        /*
         * Funktio ajan tulostamiseksi sarjamonitorille.
         * Funktio ei palauta mitään, eikä ota vastaan mitää parametreja, tulostaa vain ajan halutussa muodossa sarjamonitorille.
         */
        void printTime() {


            /*
             * Merkkijonotaulukko päättyy nollatavuun ('\0'), minkä vuoksi taulukon vaatima tila muistissa on yhtä suurempi kuin siihen mahtuvan merkkijonon pituus.
             * Ajan näyttämiseen vaadittavan merkkijonon (hh:mm:ss) pituus on 8 merkkiä, eli sen tallentamiseen tarvitaan siis 9-alkioinen taulukko.
             */
            char timeFormat[9];


            /*
             * Sprintf -funktio muodostaa saman tyylisen merkkijonon, kuten esim. c-kielen printf -funktio, mutta ei kuitenkaan itsessään tulosta mitään, vaan haluttu sisältö tallennetaan merkkijonona.
             * Funktion ensimmäinen parametri (timeFormat) on pontteri, joka osoittaa merkkijonotaulukkoon johon haluttu sisältö/merkkijono tallennetaan.
             * Toinen parametri ("%02d:%02d:%02d") on merkkijonon haluttu esitysmuoto.
             * hour(), minute() ja second() funktiot ovat peräisin TimeLib kirjastosta.
             */
            sprintf(timeFormat, "%02d:%02d:%02d", hour(), minute(), second());

            // Tulostetaan aika sarjamonitorille
            Serial.println(timeFormat);
        }

    void setup() {

        Serial.begin(9600);

        /*
         * cli -funktiolla poistetaan keskeytykset käytöstä.
         * Funkiolla manipuloidaan AVR status rekisterin (SREG) bittiä numero 7, eli I bitti (Bit 7 – I: Global Interrupt Enable).
         * SREG Bit 7 = 0
         */
        cli();


        /*
         * Register Description
         * 
         * TCCR1A – Timer/Counter1 Control Register A
         * 
         *    Bit 7:6 – COM1A1:0: Compare Output Mode for Channel A
         *    Bit 5:4 – COM1B1:0: Compare Output Mode for Channel B
         *    Bit 1:0 – WGM11:0: Waveform Generation Mode
         * 
         * TCCR1B – Timer/Counter1 Control Register B
         * 
         *    Bit 7 – ICNC1: Input Capture Noise Canceler
         *    Bit 6 – ICES1: Input Capture Edge Select
         *    Bit 5 – Reserved Bit
         *    Bit 4:3 – WGM13:2: Waveform Generation Mode
         *    Bit 2:0 – CS12:0: Clock Select
         *    
         * TCCR1C – Timer/Counter1 Control Register C
         * 
         *    Bit 7 – FOC1A: Force Output Compare for Channel A
         *    Bit 6 – FOC1B: Force Output Compare for Channel B
         */
        TCCR1A = TCCR1B = TCCR1C = 0; // Alustetaan TCCR1A, TCCR1B ja TCCR1C arvoon 0 (kaikki bitit 0). A ja C rekistereitä ei käytetä tässä esimerkissä.


        /*
         * TCNT1H and TCNT1L – Timer/Counter1
         * Timer Counterin arvo kasvaa (inkrementoituu) joka kellojaksolla (pulssi suoraan System Clock tai Prescaler).
         * System Clock kellotaajuus on 16MHz, ja Prescaler toimii ikään kuin jakajana (clk/prescaler). Lisää tästä kohdassa: Output Compare Register...
         */
        TCNT1 = 0; // Alustetaan timer counter arvoon 0


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


        /*
         * Asetetaan prescalerin arvoksi 1024, laittamalla Timer Counter Control (TCCR1B) rekisterin bitit 2 ja 0 päälle (CS12 ja CS10).
         * Clock Select Bit Description
         * 
         * CS12  |  CS11  |  CS10  |  Description
         *----------------------------------------------------------------------------------
         *   0   |    0   |    0   |  No clock source (Timer/Counter stopped).
         *----------------------------------------------------------------------------------
         *   0   |    0   |    1   |  clk/1 (No prescaling)
         *----------------------------------------------------------------------------------
         *   0   |    1   |    0   |  clk/8 (From prescaler)
         *----------------------------------------------------------------------------------
         *   0   |    1   |    1   |  clk/64 (From prescaler)
         *----------------------------------------------------------------------------------
         *   1   |    0   |    0   |  clk/256 (From prescaler)
         *----------------------------------------------------------------------------------
         *   1   |    0   |    1   |  clk/1024 (From prescaler)
         *----------------------------------------------------------------------------------
         *
         * 
         */
        TCCR1B |= (1 << CS12) | (1 << CS10); // TCCR1B Bit 2 (CS12) = 1, TCCR1B Bit 0 (CS10) = 1


        /*
         * Laitetaan CTC mode päälle (Clear Timer On Compare Match).
         * Kun keskeytys tapahtuu, niin timer counterin (TCNT1) arvo resetoituu takaisiin nollaan.
         */
        TCCR1B |= (1 << WGM12); // TCCR1B Bit 3 (WGM12) = 1


        /*
         * TIMSK1 – Timer/Counter1 Interrupt Mask Register
         * Kun tämä bitti (TIMSK1 Bit 1 – OCIE1A) on 1 ja Status rekisterin bitti numero 7 (SREG Bit 7 – I) on 1, niin Timer Counterin Output Compare A Match Interrupt on sallittu.
         * Vastaava keskeytysvektori (kts. ATMEGA -datalehti s. 57) suoritetaan silloin kun Timer Counter Interrupt Flag (TIFR1) Rekisterin bitti nro. 1 (Bit 1 – OCIE1A) on "päällä", eli 1.
         * 
         * TIFR1 Bit 1 – OCF1A: (Timer/Counter1, Output Compare A Match Flag) menee päälle, kun timer counterin (TCNT1) arvo vastaa Output Compare rekisterin (OCR1A) arvoa.
         * 
         */
        TIMSK1 |= (1 << OCIE1A); // TIMSK1 Bit 1 (OCIE1A) = 1

        /*
         * sei -funktiolla otetaan keskeytykset käyttöön.
         * Funkiolla manipuloidaan AVR status rekisterin (SREG) bittiä numero 7, eli I bitti (Bit 7 – I: Global Interrupt Enable).
         * SREG Bit 7 = 1
         */
        sei();
    }


    /*
     * ISR funktio = keskeytyspalvelu (Interrupt Service Routine)
     * 
     * Interrupt Vector - TIMER1_COMPA_vect
     * 
     * VectorNo.  |  Program Address  |  Source        |  Interrupt Definition
     *----------------------------------------------------------------------------------
     * 12         |  0x0016           |  TIMER1 COMPA  |  Timer/Counter1 Compare Match A
     */
    ISR(TIMER1_COMPA_vect) {
        // Kun keskeytys tapahtuu -> kutsutaan funktio, joka tulostaa ajan sarjamonitorille
        printTime();
    }

    void loop() {
        // Nothing to see here... :)
    }
