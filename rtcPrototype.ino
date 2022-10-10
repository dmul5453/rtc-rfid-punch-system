/*
 * Prototype for RTC component
 * @author: Dustin Mullins
 * date created: 10/10/22
 * last updated: 10/10/22
 */

// make sure to go to Sketch > include library > add .ZIP library > the DS3231 zip
#include <Wire.h>
#include <DS3231.h>

// pin definitions
#define ledPin 13
#define outPin 4
#define inPin 7

// RTC setup
DS3231 clock;
RTCDateTime in;
RTCDateTime out;

// sets up an led pin for output
void ledSetup(int pinNum) {
  pinMode(pinNum, OUTPUT);
}

// sets up a button pin for input
void buttonSetup(int pinNum) {
  pinMode(pinNum, INPUT_PULLUP);
}

void setup() {
  // put your setup code here, to run once:

  // pin setup
  ledSetup(ledPin);
  buttonSetup(outPin);
  buttonSetup(inPin);

  // serial monitor setup
  Serial.begin(9600);
  Serial.println("Prototype for RTC component, testing logic for calculating time worked");

  
  //initializing the clock
  clock.begin();
  //the below command can be commented out once we want the arduino to store the time without connection to computer
  // This can also be set manually via the format (YYYY, MM, DD, HH, II, SS)- for example, clock.setDateTime(2016, 12, 9, 11, 46, 00);
  clock.setDateTime(__DATE__, __TIME__);
}

// returns true when a given button is pressed
bool buttonPress(int pinNum) {
  if (digitalRead(pinNum) == LOW) {
    return true;
  } else {
    return false;
  }
}

// turns on given led
void turnOnLed(int pinNum) {
  digitalWrite(pinNum, HIGH);
}

// turns off given led
void turnOffLed(int pinNum) {
  digitalWrite(pinNum, LOW);
}

// converts date to julian date for use in calculations
long convertToJD(int year, int month, int day)
{
  return day-32075+1461L*(year+4800+(month-14)/12)/4+367*(month-2-(month-14)/12*12)/12-3*((year+4900+(month-14)/12)/100)/4;
}

// calculates difference in days between two dates by using their julian dates
long daysDiff(int year1, int mon1, int day1, int year2, int mon2, int day2)
{
  return convertToJD(year2, mon2, day2) - convertToJD(year1, mon1, day1);
}

// calculates difference in minutes between two dates by using their difference in days
long minutesDiff(int year1, int mon1, int day1, int hour1, int min1, int year2, int mon2, int day2, int hour2, int min2 )
{
  int mDiff = (hour2*60+min2) - (hour1*60+min1);
  return daysDiff(year1, mon1, day1, year2, mon2, day2)*1440 + mDiff;
}

// displays given dateTime
void displayTime(RTCDateTime t) {
    Serial.print(t.year);   Serial.print("-");
    Serial.print(t.month);  Serial.print("-");
    Serial.print(t.day);    Serial.print(" ");
    Serial.print(t.hour);   Serial.print(":");
    Serial.print(t.minute); Serial.print(":");
    Serial.print(t.second); Serial.println("");
}

// displays the difference in hours between two given dateTimes
void displayTimeWorked(RTCDateTime tIn, RTCDateTime tOut) {
  double mins = minutesDiff(tIn.year, tIn.month, tIn.day, tIn.hour, tIn.minute, tOut.year, tOut.month, tOut.day, tOut.hour, tOut.minute);
  double hours = mins / 60;
  Serial.print("Worked a total of ");
  Serial.print(hours);
  Serial.println(" hours.");
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  // when the in button is pressed, record the punch in (this will be swapped to when the rfid tag is tapped)
  if (buttonPress(inPin)) {
    in = clock.getDateTime();
    Serial.print("Clocked in at: ");
    displayTime(in);
    turnOnLed(ledPin);
    delay(200);
  }

  // when the out button is pressed, record the punch out (this will be swapped to when the same user's rfid tag is tapped for a second time)
  if (buttonPress(outPin)) {
    out = clock.getDateTime();
    Serial.print("Clocked out at: ");
    displayTime(out);
    turnOffLed(ledPin);
    displayTimeWorked(in, out);
    delay(200);
  }
}
