#include <LiquidCrystal.h>

// These are the ports that the liquid crystal library connects to.
// We also need to set vss=GND, vdd=5V, a=5V, k=GND, rw=GND, and v0
// connected to a potentiometer (to control the contrast). d0-d3
// remain unused since we never need to read from the LCD display.
int const rs = 1,
          en = 2,
          d4 = 3,
          d5 = 4,
          d6 = 5,
          d7 = 6;

// Initialize the liquid crystal library
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.print(69.420, 1);
  lcd.print("%");
}
