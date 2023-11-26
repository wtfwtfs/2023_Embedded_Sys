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
#define sg90 6
#define LED_PIN 4

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bluetooth(BT_RX, BT_TX);
MFRC522 rc522(SS_PIN, RST_PIN);
Servo SG90;

/*
  0 - normal
  1 - password_input
  2 - intruder!!!!
  3 - register
*/
int mode = 0;

/*
  사용자 인증 여부, 문 열림 여부                                                                                                                                                                                                
*/
bool isAuthor = false;
bool isDoorOpened = false;
bool isLightOn = false;

/*
  RFID를 저장할 버퍼
*/
byte readDT[4];

void light_handler(){
  if(mode == 2){ // 침입자 경계중일 경우 -> LED 깜박깜박, delay로 인해 버저도 간격을 두고 울리도록 설계
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  else if(isDoorOpened){ // 문이 열릴 경우 집의 불을 켠다.
    digitalWrite(LED_PIN, HIGH);
    isLightOn = true;
  }
  else { // 문이 닫힐 경우 집의 불을 끈다.
    digitalWrite(LED_PIN, LOW);
    isLightOn = false;
  }
}

void register_card(){
  mode = 3; // 모드 변경

  Serial.println("Register your Card : "); //lcd 세팅하기
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Register");
  lcd.setCursor(0,1);
  lcd.print("Your Card...");

  while(1){
    if ( rc522.PICC_IsNewCardPresent() && rc522.PICC_ReadCardSerial() ) { // 읽을 카드가 있을 경우 읽어 저장한다.
      readDT[0] = rc522.uid.uidByte[0];
      readDT[1] = rc522.uid.uidByte[1];
      readDT[2] = rc522.uid.uidByte[2];
      readDT[3] = rc522.uid.uidByte[3];

      // 카드를 저장 후 등록되었다고 lcd에 세팅하기
      Serial.println("Registered!");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Card Registered!");

      delay(3000);
      mode = 0; // 다시 노멀 모드로 전환 후 종료
      break;
    }
  }
}

void send_status(){ 
  if(mode == 2){
    bluetooth.write("I"); // 현재 칩입자 발생
    delay(100);
    //Serial.println("Status : intruder alert!");
  }
  else if(mode == 3){
    bluetooth.write("R"); // 인증 상태인 경우
    delay(100);
  }
  else if(mode == 0 && isAuthor && isDoorOpened){
    bluetooth.write("O"); // 현재 문이 열려 있는 경우
    delay(100);
    //Serial.println("Status : Door is Opened, Be careful!");
  }
  else if(mode == 0 && isAuthor && !isDoorOpened){
    bluetooth.write("C"); // 현재 문이 닫혀 있는 경우
    delay(100);
    //Serial.println("Status : Door is Locked");
  }
  else{
    bluetooth.write("U"); // 그 외에는 인증이 되지 않았거나 
    delay(100);
  }
}

void show_status(){ // lcd에 상태를 띄워줍니다.
  lcd.clear();
  if(isDoorOpened){ // 문이 열려 있는 경우
    lcd.setCursor(0,0);
    lcd.print("Door is opened");
  }
  else { // 문이 닫혀 있는 경우
    lcd.setCursor(0,0);
    lcd.print("Door is locked");
  }

  if(isAuthor){ // 카드로 인증이 된 경우 
    lcd.setCursor(0,1);
    lcd.print("User OK");
  }
  else { // 카드로 인증이 아직 되지 않은 경우
    lcd.setCursor(0,1);
    lcd.print("User Unknown");
  }

}

void open(){
  for(int i = 180; i > 0; i--){ // 180부터 0까지 1씩 감소
    SG90.write(i);
    delay(5);
  }
  delay(500);
  isDoorOpened = true; // 문이 열림
  light_handler();
}

void close(){
  for(int i = 0; i <= 180; i++){ // 0부터 180까지 1씩 증가
    SG90.write(i);
    delay(5);
  }
  delay(500);
  isDoorOpened = false; // 문이 닫힘
  light_handler();
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
    send_status(); // 현재상태를 앱에 보냅니다.

     if ( rc522.PICC_IsNewCardPresent() && rc522.PICC_ReadCardSerial() ) { 
      //카드 또는 ID 가 읽히지 않으면 return을 통해 다시 시작하게 됩니다.
      if(rc522.uid.uidByte[0]==readDT[0] && rc522.uid.uidByte[1]==readDT[1] && rc522.uid.uidByte[2]==readDT[2] 
      && rc522.uid.uidByte[3]==readDT[3]) {
        mode = 0;
        isAuthor = true;
        if(!isDoorOpened){
          open();
        }
        break;
      }
    }
    tone(BUZZER_PIN, 23, 3000);
    light_handler();
  }

  while(bluetooth.available()){ // 혹시 침입자 알림이 뜬 동안의 블루투스로 들어온 요청은 모두 제거합니다.
    bluetooth.read();
  }
}

void waiting(){
  show_status();

  while(1){
    send_status();
    read_ble();

    if ( rc522.PICC_IsNewCardPresent() && rc522.PICC_ReadCardSerial() ) { 
      //카드와 ID가 읽힌다면 
      if(rc522.uid.uidByte[0]==readDT[0] && rc522.uid.uidByte[1]==readDT[1] && rc522.uid.uidByte[2]==readDT[2]
      && rc522.uid.uidByte[3]==readDT[3]) {  //등록된 카드와 대조한다.
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Authenticated");
        mode = 0;
        isAuthor = true;
        if(!isDoorOpened){
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

void read_ble(){ // 블루투스로 전달 받은 글자에 따른 처리
  if(bluetooth.available()){
    char data = bluetooth.read();
    switch(data){ 
      case 'r' : // 블루투스로 받은 데이터가 r일 때
        mode = 3; // 모드를 3으로 하고, 카드 입력을 받는다.
        register_card();
        show_status();
        break;

      case 'o' : // 블루투스로 받은 데이터가 o일 때 
        if (!isDoorOpened){ // 문이 열려있지 않다면 문을 열고 상태 출력
          isDoorOpened = true;
          open();
          show_status();
        }
        break;

      case 'c' :
        if (isDoorOpened){ // 문이 열려있다면 문을 닫고 상태 출력
          light_handler();
          show_status();
        }
        break;

      case 'l' :
        if(!isLightOn){
          isLightOn = true;
          digitalWrite(LED_PIN, HIGH);
        }
        //show_status();
        break;

      case 'd' :
        if(isLightOn){
          isLightOn = false;
          digitalWrite(LED_PIN, LOW);
        }
        //show_status();
        break;
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
  pinMode(LED_PIN, OUTPUT);

  // LCD를 초기화합니다.
  lcd.init();
  // I2C LCD의 백라이트를 켜줍니다.
  lcd.backlight();

  close(); // 문이 잠긴 상태로 시작합니다.
  digitalWrite(LED_PIN, LOW);

  register_card(); // 일단 처음 시작할 시 등록될 카드를 입력받습니다.
}

void loop()
{
  waiting(); // 계속 대기 함수를 부르면서 카드의 입력을 받는 상태가 됩니다.
  send_status(); // 주기적으로 블루투스를 통해 상태를 표시합니다.

    // if(bluetooth.available()){
    //   Serial.write(bluetooth.read());
    // }
    // if(Serial.available()){
    //   bluetooth.write(Serial.read());
    // }
}