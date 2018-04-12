#include <TimeLib.h>
#include <MsTimer2.h>

int hhmmss = 0;

void printTime() {

    char buffer [10];

    /*
     * sprintf()
     * 
     * Write formatted data to string
     * Composes a string with the same text that would be printed if format was used on printf, but instead of being printed, the content is stored as a C string in the buffer
     * http://www.cplusplus.com/reference/cstdio/sprintf/
    */
    hhmmss = sprintf (buffer, "%02d:%02d:%02d", hour(), minute(), second());

    for(int i = 0; i <= hhmmss; i++) {
      Serial.print(buffer[i]);
      }
      Serial.println();
}

void setup() {
  Serial.begin(9600);

  // tunnit, minuutit, sekunnit, päivä, kuukausi, vuosi
  setTime(23, 30, 00, 12, 04, 2018);
    
  MsTimer2::set(1000, printTime);
  MsTimer2::start();
}

void loop() {
  // Silence is Golden :)
}
