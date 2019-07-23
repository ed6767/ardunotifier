/*
(c) 2019 Ed.E
*/
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <LowPower.h>

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

#define casualFade  0
#define fastFade     1
#define fadeyBlink   2
#define flashOnce   3
#define flashTwice 4
#define flashThreeTimes   5

void animationIn(int animation) {
  
  /* Casual Fade */
  if (animation == casualFade) {
    for (int i = 0; i <= 255; i++) {
    analogWrite(10, i);
    delay(10);
    }
  }
  /* Fast Fade */
  if (animation == fastFade) {
    for (int i = 0; i <= 255; i++) {
    analogWrite(10, i);
    delay(1);
    }
  }
  /* Fadey Blink */
  if (animation == fadeyBlink) {
    animationIn(fastFade);
    animationOut(fastFade);
    animationIn(fastFade);
    animationOut(fastFade);
    animationIn(fastFade);
  }
  /* Flash */
  if (animation > 2) {
    for (int i = 0; i <= animation - 2; i++) {
      analogWrite(10, 255);
      delay(100);
      analogWrite(10, 0);
      delay(100);
    }
    analogWrite(10, 255);
    
  }
  

  
}

void animationOut(int animation) {
  if (animation = fastFade) {
    for (int i = 0; i <= 255; i++) {
    analogWrite(10, 255-i);
    delay(1);
    }
  } else {
    for (int i = 0; i <= 255; i++) {
    analogWrite(10, 255-i);
    delay(10);
  }
  }
}

void scrollText(int speed, String title, String message) {
  // Calculate the amount of "-" needed to center title
  int titleLength = title.length();
  if (titleLength > 16) {
    Serial.println("error during scroll");
  }
  int bothSides = (16 - titleLength) / 2;
  // Construct Begining -
  String appendBegin = "-";
  for (int i = 2; i <= bothSides; i++) {
    appendBegin += "-";
  }
  // Construct End -
  String appendEnd = "-";
  for (int i = 1; i <= bothSides; i++) {
    appendEnd += "-";
  }
  
  // Assemble the title
  String finalTitle = appendBegin;
  finalTitle += title;
  finalTitle += appendEnd;
  
  // Now on to the actual scrolling
  
  if (message.length() < 17) {
    //Display message and do not scroll as it can fit on screen, we wait after calculating the time it would take to scroll
    lcd.setCursor(0,0);
    lcd.print(finalTitle);
    lcd.setCursor(0,1);
    lcd.print(message);
    delay(speed * message.length());
    
    goto exitScrollText;
  }
  
  // Before we need to scroll, add spaces until string is factor of 16
  String finalMessage = message;
  // if the remainder of dividing of 16 is not 0
  if (!(finalMessage.length() % 16 == 0)) {
    while (!(finalMessage.length() % 16 == 0)) {
      finalMessage += " "; // Add space for every time it's not a factor
    }
  }
  for (int i = 0; i <= finalMessage.length() - 16; i++) {
  lcd.setCursor(0,0);
  lcd.print(finalTitle);
  lcd.setCursor(0,1);
  lcd.print(finalMessage.substring(i,16 + i));
  delay(speed);
  }
  
  
  // Keep at bottom to exit
  exitScrollText:
  lcd.setCursor(0,0);
}

void notify(String title, String message, int animation, int duration, bool scroll) {
  lcd.setCursor(0,0);
  lcd.clear();
  analogWrite(10, 0);
  if (!scroll) {
  // No scroll
    lcd.print(title);
  lcd.setCursor(0,1);
  lcd.print(message);
  animationIn(animation);
  delay(duration);
  animationOut(animation);
  
  } else {
    // Scroll at speed based on duration
    animationIn(animation);
    scrollText(duration / message.length(), title, message);
    animationOut(animation);
  }
  
  lcd.clear();
}

String serialWait() {
  String returnVal = "";
  while (returnVal.length() < 2) {
    returnVal = Serial.readString();
    delay(50);
  }
  return(returnVal);
}


void setup() {
  lcd.begin(16,2);
  Serial.begin(115200);
  if (EEPROM.read(3) == 42) {
    // read eeprom 0x3 - check if value is 42, if so device is disabled
    Serial.println("brick");
 notify("System Error", "Error code: 0x3 Check -- please contact support.", flashThreeTimes, 10000, false);
    Serial.println("want");
  }
}

void loop() {
  Serial.println("disconnect");
 notify("Ed. E's notifier", "Version compiled...", fastFade, 2000, false);
  notify(__TIME__, __DATE__, fastFade, 2000, false);
  if (Serial) {
 
  } else {
    notify("Serial is not", "detected!!", flashThreeTimes, 10000, false);
  }
  
/* DISCONNECTED */
waitForConnect:
lcd.clear();
lcd.setCursor(0,0);
analogWrite(10, 0);
waitConnectLoop:
delay(1000);
Serial.println("waiting");
if (Serial.readString() == "connect") {
  goto connected;
}

// Insert connect code
if (read_LCD_buttons() == btnLEFT) {
  notify("Waiting for", "connection", fadeyBlink, 5000, false);
}
goto waitConnectLoop;

/* CONNECTED */
connected:
Serial.println("accepted");
notify("Ed. E's notifier", "Connected!", fastFade, 2000, false);
connectedLoop:
if (serialWait() == "notify") {
  // Entire process should take 800ms   
  Serial.println("title");
  String title = serialWait();
  Serial.println("message");
  String message = serialWait();
  Serial.println("animation");
  int animation = serialWait().toInt();
  Serial.println("duration");

  int duration = serialWait().toInt();
  Serial.println("complete");
  notify(title, message, animation, duration, true);
}
goto connectedLoop;
}
