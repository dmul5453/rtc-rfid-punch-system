 /*
 *
 * ------------------------------------
 *             MFRC522      Arduino  
 *             Reader/PCD   Uno      
 * Signal      Pin          Pin       
 * -------------------------------------
 * RST/Reset   RST          9             
 * SPI SS      SDA(SS)      10            
 * SPI MOSI    MOSI         11 / ICSP-4   
 * SPI MISO    MISO         12 / ICSP-1   
 * SPI SCK     SCK          13 / ICSP-3 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   9     // Configurable, see typical pin layout above
#define SS_PIN    10    // Configurable, see typical pin layout above

 int red_light_pin = 7;
 int green_light_pin = 6;
 int blue_light_pin = 5;
  
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card

  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  
}


void loop() {
  int uid_sum = 0;
  int allowed_uid_sum [] = {262};
  String uid_user [] = {"Josh Laurente"};

  
  // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  
  // Now a card is selected. The UID is in mfrc522.uid.
  
  // Make UID into string and print it
  
  Serial.print("Card UID :");
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i]);
    uid_sum += mfrc522.uid.uidByte[i];
  }

  Serial.println();
  
  // Check if UID is valid
  for (int i = 0; i < sizeof(allowed_uid_sum); i++) {
    if (uid_sum == allowed_uid_sum[i]) {
      Serial.print("Access granted: ");
      Serial.println(uid_user[i]);
      RGB_color(0, 255, 0);
      delay(2000);
      RGB_color(0, 0, 0);
      break;
    }
    else {
      Serial.println("Access denied.");
      RGB_color(255, 0, 0);
      delay(2000);
      RGB_color(0, 0, 0);
      break;
    }
  }

  delay(2000);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
