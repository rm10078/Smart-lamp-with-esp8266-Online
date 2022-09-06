#include <EEPROM.h>

// Change the STORED_WORD is another string
String STORED_WORD = "Rajib manna";
String answer = "";

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);

  Serial.println("Store word " + STORED_WORD + " in EEPROM");
  eeprom_write(STORED_WORD,100);
}

void loop() {
  Serial.print("Read word from EEPROM: ");
  answer = eeprom_read(100);
  Serial.println(answer);

  Serial.print("Word length: ");
  Serial.println(answer.length());

  delay(2000);
}

void eeprom_write(String word,int start_add) {
  delay(10);

  for (int i = 0; i < word.length(); ++i) {
    Serial.println(word[i]);
    EEPROM.write(start_add+i, word[i]);
  }

  EEPROM.write(start_add+word.length(), '\0');
  EEPROM.commit();
}

String eeprom_read(int start_add) {
  String word;
  char readChar='n';
  int i = 0;

  while (readChar != '\0') {
    int tem=start_add+i;
    readChar = char(EEPROM.read(tem));
    delay(10);
    i++;

    if (readChar != '\0') {
      word += readChar;
    }
  }

  return word;
}
