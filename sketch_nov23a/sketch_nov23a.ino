#include "pitches.h"
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>   

#define BUZZER_PIN 5
#define BT_TX 8
#define BT_RX 7
#define RST_PIN 9
#define SS_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bluetooth(BT_RX, BT_TX);
MFRC522 rc522(SS_PIN, RST_PIN);

Servo SG90;

/*
  0 - locked
  1 - opened
  2 - intruder!!!!
*/
int mode = 0;

bool isAuthor = false;
bool isDoorOpened = false;

int sg90 = 6;

void show_status(){
  lcd.clear();
  if(isDoorOpened){
    lcd.setCursor(0,0);
    lcd.print("Door is opened");
  }
  else {
    lcd.setCursor(0,0);
    lcd.print("Door is locked");
  }

  if(isAuthor){
    lcd.setCursor(0,1);
    lcd.print("User OK");
  }
  else {
    lcd.setCursor(0,1);
    lcd.print("User Unknown");
  }

}

void open(){
  for(int i = 180; i > 0; i--){  //180부터 0까지 1씩 감소
    SG90.write(i);
    delay(5);
  }
  delay(500);
  mode = 1;
  isDoorOpened = true;

}

void close(){
  for(int i = 0; i <= 180; i++){        //0부터 180까지 1씩 증가
    SG90.write(i);
    delay(5);
  }
  delay(500);
  mode = 0;
  isDoorOpened = false;

}

void intruder_alert(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Intruder!");
  lcd.setCursor(0,1);
  lcd.print("Who Are You!!");

  isAuthor = false;
  mode = 2;

  while(1){
     if ( rc522.PICC_IsNewCardPresent() && rc522.PICC_ReadCardSerial() ) { 
      //카드 또는 ID 가 읽히지 않으면 return을 통해 다시 시작하게 됩니다.
      if(rc522.uid.uidByte[0]==0xA3 && rc522.uid.uidByte[1]==0x88 && rc522.uid.uidByte[2]==0x1A 
      && rc522.uid.uidByte[3]==0x0F) {
        isAuthor = true;
        if(!isDoorOpened){
          open();
        }
        break;
      }
    }
    tone(BUZZER_PIN, 40, 1000);
    delay(500);
  }
}

void waiting(){
  show_status();

  while(1){
    if ( rc522.PICC_IsNewCardPresent() && rc522.PICC_ReadCardSerial() ) { 
      //카드 또는 ID 가 읽히지 않으면 return을 통해 다시 시작하게 됩니다.
      if(rc522.uid.uidByte[0]==0xA3 && rc522.uid.uidByte[1]==0x88 && rc522.uid.uidByte[2]==0x1A 
      && rc522.uid.uidByte[3]==0x0F) {  // 여기에 CARD UID 를 자신의 카드에 맞는 값으로 변경해주세요
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Authenticated");

        isAuthor = true;

        if(mode == 0){
          open();
        }
        else {
          close();
        }
        break;
      }
      else {
        if(isDoorOpened){
          close();
        }
        intruder_alert();
        show_status();
      }
    }
  }
}

void setup()
{
  Serial.begin(9600);
  bluetooth.begin(9600);

  SPI.begin();
  rc522.PCD_Init();
  SG90.attach(sg90);  

  pinMode(BUZZER_PIN, OUTPUT);

  // LCD를 초기화합니다.
  lcd.init();
  // I2C LCD의 백라이트를 켜줍니다.
  lcd.backlight();
}

void loop()
{
  waiting();
    // if(bluetooth.available()){
    //   Serial.write(bluetooth.read());
    //   mode = 1;
    // }
    // if(Serial.available()){
    //   bluetooth.write(Serial.read());
    // }
    // if (mode == 1){
    //   tone(BUZZER_PIN, melody[0], 2000);
    //   noTone(3000);
    //   mode = 0;
    // }
}