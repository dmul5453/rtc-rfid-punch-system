/*
   Prototype for full rtc rfid punch system
   @author Dustin Kurtz & Jonathan Lafontaine
   Date created: 11/28/2022
   Last updated: 11/30/2022
*/

// libraries
#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS3231.h>
#include <SPI.h>
#include <MFRC522.h>

// pin definitions
#define rs 2
#define en 3
#define d4 4
#define d5 5
#define d6 6
#define d7 7
#define RST_PIN 9
#define SS_PIN 10

// object initialization
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
MFRC522 mfrc522(SS_PIN, RST_PIN);
DS3231 clock;

// converts date to julian date for use in calculations
long convertToJD(int year, int month, int day) {
  return day - 32075 + 1461L * (year + 4800 + (month - 14) / 12) / 4 + 367 * 
    (month - 2 - (month - 14) / 12 * 12) / 12 - 3 * ((year + 4900 + (month - 14) / 12) / 100) / 4;
}

// calculates difference in days between two dates by using their julian dates
long daysDiff(int year1, int mon1, int day1, int year2, int mon2, int day2) {
  return convertToJD(year2, mon2, day2) - convertToJD(year1, mon1, day1);
}

// calculates difference in minutes between two dates by using their difference in days
long minutesDiff(int year1, int mon1, int day1, int hour1, int min1,
                 int year2, int mon2, int day2, int hour2, int min2) {
  int mDiff = (hour2 * 60 + min2) - (hour1 * 60 + min1);
  return daysDiff(year1, mon1, day1, year2, mon2, day2) * 1440 + mDiff;
}

// displays given dateTime
void displayTime(RTCDateTime t) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(t.month);  lcd.print("-");
  lcd.print(t.day);    lcd.print(" ");
  lcd.print(t.hour);   lcd.print(":");
  lcd.print(t.minute); lcd.print(":");
  lcd.print(t.second);
}

// displays the difference in hours between two given dateTimes
void displayTimeWorked(RTCDateTime tIn, RTCDateTime tOut) {
  double mins = minutesDiff(tIn.year, tIn.month, tIn.day, tIn.hour, 
    tIn.minute, tOut.year, tOut.month, tOut.day, tOut.hour, tOut.minute);
  double hours = mins / 60;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Worked for: ");
  lcd.setCursor(0, 1);
  lcd.print(hours);
  lcd.print(" hours.");
}

// print the provided UID in hex to the serial port.
// this does NOT add a newline.
void printUID(byte uid[10]) {
  for (int i = 0; i < 9; ++i) {
    Serial.print(uid[i], HEX);
    Serial.print(" ");
  }
  Serial.print(uid[9], HEX);
}

void setup() {
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  lcd.begin(16, 2);    // Init LCD
  clock.begin();       // Init clock
  clock.setDateTime(__DATE__, __TIME__);
  Serial.begin(9600);
}

struct UserID {
  char const* name;
  byte        uid[10];
  bool        logged_in;
  RTCDateTime login_time;
};

// an array of registered users
// TODO: make this dynamic at runtime?
UserID users[2] = {
  { "Jono (CARD)",   { 0x43, 0x81, 0xAF, 0x05 }, false },
  { "Tao Xie (TAG)", { 0x43, 0x08, 0x1E, 0x11 }, false }
};

void loop() {
  // if we have no card to scan, just return
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    return;
  
  // check to see if one of the registered users punched in
  for (auto& user : users) {
    // memcmp will return 0 if the UID matches
    if (memcmp(mfrc522.uid.uidByte, user.uid, 10) != 0)
      continue;
    
    // if the user was logged in, then log them out.
    // otherwise, log them in.
    user.logged_in = !user.logged_in;

    if (user.logged_in) {
      Serial.print(user.name);
      Serial.println(" clocked in.");
      displayTime(clock.getDateTime());
    } else {
      Serial.print(user.name);
      Serial.println(" clocked out.");
      displayTimeWorked(user.login_time, clock.getDateTime());
    }

    // update the login time for this user
    user.login_time = clock.getDateTime();

    delay(2000);
    return;
  }

  // if we reach here, then an unregistered user tried to log in.
  Serial.print("Unrecognized user: ");
  printUID(mfrc522.uid.uidByte);
  Serial.println();
}















