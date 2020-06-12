/*
(c) March/April 2020 Ed.E
*/
#include <LiquidCrystal.h>
#include <avr/sleep.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


int adc_key_in = 0;
// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(A0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 /*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 // For V1.0 comment the other threshold and use the one below:
*/
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   



 return btnNONE;  // when all others fail, return this...
}



void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  Serial.begin(115200);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Ed. E's notifier");
  lcd.setCursor(0,1);
  lcd.print("Version compiled...");
  delay(2000);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print(__TIME__);
  lcd.setCursor(0,1);
  lcd.print(__DATE__);
  delay(2000);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("notifier v2");
  lcd.setCursor(0,1);
  lcd.print("Ready to connect.");
}

void loop() {
  // put your main code here, to run repeatedly:

  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    
    // read all the available characters
    while (Serial.available() > 0) {
      char toWrite = Serial.read();
      // If it is a ` that means custom command
      if (toWrite == '`') {
        char action = Serial.read();
        if (action == 'b') { // read button
          Serial.println(read_LCD_buttons()); 
       } else if (action == 'c') { // clear lcd
        lcd.clear();
       } else if (action == 'l') {// backlight brightness
        // example `l then BYTE value
        int brightness = Serial.read();
        analogWrite(10, brightness);
       } else if (action == 'p') { // suspend / low power mode
        
        // prep to sleep
        lcd.clear();
        analogWrite(10, 0);
        
        sleep_enable();
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_cpu(); // go to sleep
        
        analogWrite(10, 255); // on wake up, doubt that'd ever happen
       } else if (action == '2') {
        // Set cursor to line 2
        lcd.setCursor(0, 1);
       } else if (action == '3') {
       // When showing time, turns into 9x2, so we go to this on first line
          lcd.setCursor(7, 0);
       } else if (action == '4'){
        // same for line 2
        lcd.setCursor(7, 1);
       }
         
      } else {
        // display each character to the LCD
        lcd.write(toWrite);
      }
    }
   
  }

}
