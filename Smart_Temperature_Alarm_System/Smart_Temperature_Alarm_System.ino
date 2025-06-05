#include <IRremote.h>
#include <EEPROM.h>

const int tempPin = A3;
const int buzzer = 9;
const int greenLED = 11;
const int yellowLED = 12;
const int redLED = 13;
const int RECV_PIN = 7;
int tempThreshold = 30;  // Default Celsius
const int EEPROM_ADDR = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;

void playAlarmMelody() {
  int melody[] = {440, 494, 523, 0, 523, 494, 440};
  int duration = 250;
  for (int i = 0; i < 7; i++) {
    if (melody[i] > 0)
      tone(buzzer, melody[i], duration);
    delay(duration + 50);
  }
  noTone(buzzer);
}

float readTempSensor() {
  int val = analogRead(tempPin);
  // Adjust formula for your sensor if known
  float voltage = val * (5.0 / 1023.0);
  float tempC = (voltage - 0.5) * 100.0;
  return tempC;
}

void setup() {
  Serial.begin(9600);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  irrecv.enableIRIn();
  tempThreshold = EEPROM.read(EEPROM_ADDR);
  if (tempThreshold < 10 || tempThreshold > 100) tempThreshold = 30;
  Serial.print("Initial Threshold: ");
  Serial.println(tempThreshold);
}

void loop() {
  float temp = readTempSensor();
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C | Threshold: ");
  Serial.println(tempThreshold);

  if (temp < tempThreshold - 5) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
  } else if (temp >= tempThreshold - 5 && temp < tempThreshold) {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
    playAlarmMelody();
  }

  if (irrecv.decode()) {
    uint8_t cmd = irrecv.decodedIRData.command;
    if (cmd == 0x46) tempThreshold += 1;
    else if (cmd == 0x47) tempThreshold -= 1;
    EEPROM.write(EEPROM_ADDR, tempThreshold);
    Serial.print("New Threshold: ");
    Serial.println(tempThreshold);
    irrecv.resume();
  }

  delay(1000);
}
