#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button Pins
#define BTN6 13
#define BTN8 12
#define BTN12 14

// Stepper Driver Pins
#define STEP_PIN 26
#define DIR_PIN 27

unsigned long feedInterval = 0;
DateTime nextFeedTime;

bool intervalSet = false;

void setup() {

  Serial.begin(115200);

  Wire.begin();

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
  lcd.print("Cat Feeder");
  delay(2000);
  lcd.clear();
}

void loop() {

  DateTime now = rtc.now();

  // Button Press Detection

  if(digitalRead(BTN6) == LOW){
    setFeedingInterval(6);
    delay(300);
  }

  if(digitalRead(BTN8) == LOW){
    setFeedingInterval(8);
    delay(300);
  }

  if(digitalRead(BTN12) == LOW){
    setFeedingInterval(12);
    delay(300);
  }

  // Auto Feeding

  if(intervalSet && now.unixtime() >= nextFeedTime.unixtime()){

    feedCat();

    nextFeedTime = now + TimeSpan(feedInterval * 3600);
  }

  displayInfo(now);

  delay(500);
}

void setFeedingInterval(int hours){

  feedInterval = hours;

  DateTime now = rtc.now();

  nextFeedTime = now + TimeSpan(feedInterval * 3600);

  intervalSet = true;

  feedCat();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Interval Set");
  lcd.setCursor(0,1);
  lcd.print(String(hours) + " Hours");

  delay(2000);
}

void feedCat(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Feeding Cat");

  // Rotate Stepper

  for(int i=0; i<200; i++){

    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(800);

    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(800);
  }

  delay(1000);
}

void displayInfo(DateTime now){

  lcd.setCursor(0,0);

  char timeBuffer[9];

  sprintf(timeBuffer, "%02d:%02d:%02d",
          now.hour(),
          now.minute(),
          now.second());

  lcd.print(timeBuffer);

  lcd.setCursor(0,1);

  if(intervalSet){

    lcd.print("Next:");

    char nextBuffer[6];

    sprintf(nextBuffer, "%02d:%02d",
            nextFeedTime.hour(),
            nextFeedTime.minute());

    lcd.print(nextBuffer);

  } else {

    lcd.print("Select Time");
  }
}