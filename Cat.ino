#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// BUTTONS
#define BTN6   2
#define BTN8   3
#define BTN12  4

// TB6600
#define STEP_PIN 9
#define DIR_PIN  8

int feedIntervalHours = 0;
unsigned long nextFeedEpoch = 0;
bool systemStarted = false;

void setup() {

  Serial.begin(9600);

  Wire.begin(); // A4 SDA, A5 SCL

  rtc.begin();

  lcd.init();
  lcd.backlight();

  pinMode(BTN6, INPUT_PULLUP);
  pinMode(BTN8, INPUT_PULLUP);
  pinMode(BTN12, INPUT_PULLUP);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  digitalWrite(DIR_PIN, HIGH);

  lcd.setCursor(0,0);
  lcd.print("SMART CAT");
  lcd.setCursor(0,1);
  lcd.print("FEEDER");

  delay(2000);
  lcd.clear();
}

void loop() {

  DateTime now = rtc.now();

  if (digitalRead(BTN6) == LOW) setInterval(6);
  if (digitalRead(BTN8) == LOW) setInterval(8);
  if (digitalRead(BTN12) == LOW) setInterval(12);

  if (systemStarted && now.unixtime() >= nextFeedEpoch) {
    feedCat();
    nextFeedEpoch = now.unixtime() + feedIntervalHours * 3600;
  }

  displayData(now);

  delay(500);
}

void setInterval(int hours) {

  feedIntervalHours = hours;
  systemStarted = true;

  feedCat();

  nextFeedEpoch = rtc.now().unixtime() + hours * 3600;

  lcd.clear();
  lcd.print("SET ");
  lcd.print(hours);
  lcd.print(" HRS");

  delay(1500);
  lcd.clear();
}

void feedCat() {

  lcd.clear();
  lcd.print("FEEDING...");

  digitalWrite(DIR_PIN, HIGH);

  for (int i = 0; i < 800; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(800);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(800);
  }

  delay(1000);
  lcd.clear();
}

void displayData(DateTime now) {

  lcd.setCursor(0,0);

  char t[16];
  sprintf(t,"%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.print(t);

  lcd.setCursor(0,1);

  if (systemStarted) {
    unsigned long rem = nextFeedEpoch - now.unixtime();
    lcd.print("NEXT ");
    lcd.print(rem/3600);
    lcd.print("H ");
    lcd.print((rem%3600)/60);
    lcd.print("M");
  } else {
    lcd.print("SET 6/8/12 H");
  }
}