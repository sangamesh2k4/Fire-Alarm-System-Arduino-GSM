#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
SoftwareSerial mySerial(9, 10);
const int red = 3;
const int green = 4;
const int buzzer = 13;
const int flame = 6;
const int smoke = A0;
const int flasher1 = 11;
const int flasher2 = 12;
int flash_rate=100;
int thresh= 200;
int status = true;
String alertMsg;
String mob1="+919676177016";   //first mobile number
String mob2="+917993291858";   //second mobile number

void setup()
{
  pinMode(red, OUTPUT);
  pinMode(flasher1,OUTPUT);
  pinMode(flasher2,OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(smoke,INPUT);
  pinMode(flame,INPUT);
  pinMode(buzzer, OUTPUT);
  
  lcd.init();                      // initialize the lcd 
  lcd.clear();  
  lcd.backlight();

  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
}

void siren(int buzzer){
  for(int hz = 440; hz < 1000; hz++){
    tone(buzzer, hz, 50);
    delay(5);
  }

  for(int hz = 1000; hz > 440; hz--){
    tone(buzzer, hz, 50);
    delay(5);
  }
}

void loop()
{
  Serial.println("Gas Val: "+String(analogRead(smoke))+", Flame state: "+String(!digitalRead(flame)));
  if (digitalRead(flame)== LOW || analogRead(smoke)>thresh)  //Flame or Smoke detected
  {
    digitalWrite(red, HIGH);
    siren(buzzer);
    digitalWrite(buzzer, HIGH);
    digitalWrite(green, LOW);

    digitalWrite(flasher1, HIGH);
    digitalWrite(flasher2, LOW);
    delay(flash_rate);
    digitalWrite(flasher1, LOW);
    digitalWrite(flasher2, HIGH);
    
    if(digitalRead(flame)== LOW){
      lcd.setCursor(2, 1);
      lcd.write(1);
      lcd.setCursor(4,1); 
      alertMsg= "FIRE HIGH";
      lcd.print(alertMsg);
      lcd.setCursor(4,0);   
      lcd.print("SMOKE:"+String(analogRead(smoke)));
    }
    if(analogRead(smoke)>thresh){
      lcd.setCursor(2, 0);
      lcd.write(1);
      lcd.setCursor(4,0); 
      alertMsg= "SMOKE HIGH";
      lcd.print(alertMsg);
      lcd.setCursor(4,1);   
      lcd.print("FIRE:"+String(digitalRead(flame)==LOW?"HIGH":"LOW"));
    }
    Serial.println(alertMsg);
    if(status){   // Run once when fire/smoke is detected
      status = false;
      String msg= "Alert Type: "+alertMsg;
      SendMessage(msg,mob1);  
      delay(8000); 
      SendMessage(msg,mob2);
    }
  }
  else{
    status = true;
    lcd.setCursor(4,0);   
    lcd.print("SMOKE:"+String(analogRead(smoke)));
    lcd.setCursor(4,1);   
    lcd.print("FIRE:"+String(digitalRead(flame)==LOW?"HIGH":"LOW"));
    digitalWrite(flasher1, LOW);
    digitalWrite(flasher2, LOW);
    digitalWrite(red, LOW);
    digitalWrite(buzzer, LOW);
    noTone(buzzer);
    digitalWrite(green, HIGH);
  }
  delay(500);
  lcd.clear();
}

void SendMessage(String msg, String mob)
{
  Serial.println(msg);
  digitalWrite(flasher1, HIGH);
  digitalWrite(flasher2, HIGH);
  
  mySerial.println("AT+CMGF=1");    // Set GSM Module to Text Mode
  delay(1000);
  mySerial.println("AT+CMGS=\""+mob+"\"\r");
  delay(1000);
  mySerial.println(msg);
  delay(100);
  mySerial.println((char)26);  // ASCII code for CTRL+Z
  delay(1000);
}
