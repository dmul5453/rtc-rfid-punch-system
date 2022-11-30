/*
   Prototype for full rtc rfid punch system
   @author Dustin Kurtz
   Date created: 11/28/2022
   Last updated: 11/28/2022
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
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
DS3231 clock;
RTCDateTime in;
RTCDateTime out;

// helper methods

// converts date to julian date for use in calculations
long convertToJD(int year, int month, int day)
{
  return day - 32075 + 1461L * (year + 4800 + (month - 14) / 12) / 4 + 367 * (month - 2 - (month - 14) / 12 * 12) / 12 - 3 * ((year + 4900 + (month - 14) / 12) / 100) / 4;
}

// calculates difference in days between two dates by using their julian dates
long daysDiff(int year1, int mon1, int day1, int year2, int mon2, int day2)
{
  return convertToJD(year2, mon2, day2) - convertToJD(year1, mon1, day1);
}

// calculates difference in minutes between two dates by using their difference in days
long minutesDiff(int year1, int mon1, int day1, int hour1, int min1, int year2, int mon2, int day2, int hour2, int min2 )
{
  int mDiff = (hour2 * 60 + min2) - (hour1 * 60 + min1);
  return daysDiff(year1, mon1, day1, year2, mon2, day2) * 1440 + mDiff;
}


void setup() {
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  lcd.begin(16, 2);    // Init LCD
  clock.begin();       // Init clock
  clock.setDateTime(__DATE__, __TIME__);
}

// displays given dateTime
void displayTime(RTCDateTime t) {
  lcd.setCursor(0, 1);
  lcd.print(t.month);  lcd.print("-");
  lcd.print(t.day);    lcd.print(" ");
  lcd.print(t.hour);   lcd.print(":");
  lcd.print(t.minute); lcd.print(":");
  lcd.print(t.second);
  delay(2000);
}

// displays the difference in hours between two given dateTimes
void displayTimeWorked(RTCDateTime tIn, RTCDateTime tOut) {
  double mins = minutesDiff(tIn.year, tIn.month, tIn.day, tIn.hour, tIn.minute, tOut.year, tOut.month, tOut.day, tOut.hour, tOut.minute);
  double hours = mins / 60;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Worked for: ");
  lcd.setCursor(0, 1);
  lcd.print(hours);
  lcd.print(" hours.");
}


void loop() {
  int uid_sum = 0;
  int allowed_uid_sum [] = {262};
  String uid_user [] = {"Josh Laurente"};
  // Look for new cards, and select one if present
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {
    in = clock.getDateTime();
    lcd.setCursor(0, 0);
    lcd.print("clocked in at:");
    displayTime(in);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Employee ID:");
    lcd.setCursor(0, 1);
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      lcd.print(mfrc522.uid.uidByte[i]);
      uid_sum += mfrc522.uid.uidByte[i];
    }
  }
}
