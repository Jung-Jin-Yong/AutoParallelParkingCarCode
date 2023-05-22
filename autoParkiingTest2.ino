#include <NewPing.h>
#include <Servo.h>
#define speedPin 1
#define MOTA A1 
#define MOTB A2 

Servo myServo;

int leftFrontUltra, leftBackUltra, backLeftUltra, backRightUltra, frontLeftUltra, frontRightUltra;
int angle = 90;
NewPing Uf1(4, 5);  // 오른쪽 전방 센서 Trig, Echo
NewPing Uf2(6, 7);  //오른쪽 후방 센서
NewPing Uf3_L(8, 9);  //후방 좌측 센서
NewPing Uf3_R(12, 13); //후방 우측 센서
NewPing Uf4_L(10, 11); // 전방 좌측 센서
NewPing Uf4_R(2, 3); //전방 우측 센서

void advance(int velocity);
void backUp(int velocity);
void stopCar();
void turnLeft();
void turnRight();
void turnNeutral();

void setup() {
  // put your setup code here, to run once:
  pinMode(speedPin, OUTPUT);// PWM1
  pinMode(MOTA, OUTPUT);//direction motor 1
  pinMode(MOTB, OUTPUT);//direction motor 1

  myServo.attach(A0);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  //1. 전진하면서 주차공간 파악
  advance(255);
  while(1){
    leftFrontUltra = Uf1.ping_cm();
    leftBackUltra = Uf2.ping_cm();
    Serial.print("좌측 전방 거리 : ");
    Serial.print(leftFrontUltra);
    Serial.print("  좌측 후방 거리 : ");
    Serial.println(leftBackUltra);
    
    if (leftFrontUltra > 13 && leftBackUltra > 13){
      stopCar();
      break;
    }
  }
  delay(1000);

  Serial.println("1차 체크");
  //2. 멈추고 조금 더 전진
  advance(255);
  while(1){
    Serial.println("2차 체크");
    leftFrontUltra = Uf1.ping_cm();
    leftBackUltra = Uf2.ping_cm();
    Serial.print("좌측 전방 거리 : ");
    Serial.print(leftFrontUltra);
    Serial.print(" 좌측 후방 거리 : ");
    Serial.println(leftBackUltra);
    if (leftFrontUltra < 13 && leftBackUltra < 13){
      stopCar();
      break;
    }
  }
  //여기까진 잘됨
  
  //3. 멈추고 좌회전 꺾고 후진
  Serial.println("3차 체크");
  turnLeft();
  backUp(220);
  while(1){
    leftFrontUltra = Uf1.ping_cm();
    if(leftFrontUltra >= 18){
      break;
    }
    
  }
  stopCar();
  delay(1000);
  // 4. 휠 중립 후 살짝 뒤로 후진
  Serial.println("4차 체크");
  turnNeutral();
  backUp(240);
  /*
  backUp(180);
  delay(400);
  stopCar();
  */
  
  while(1){
    backLeftUltra = Uf3_L.ping_cm();
    backRightUltra = Uf3_R.ping_cm();
    Serial.print("후방 거리 : ");
    Serial.print(backLeftUltra);
    Serial.print(" ");
    Serial.print(backRightUltra);
    if((backRightUltra + backLeftUltra) / 2<= 12 && backRightUltra * backLeftUltra != 0){ // 0으로 튀는거 우회 방법
      stopCar();
      break;
    }
    
  }
  
  
  // 5. 우회전 꺾고 다시 후진
  Serial.println("5차 체크");
  turnRight();
  backUp(240);
  while(1){
    backLeftUltra = Uf3_L.ping_cm();
    backRightUltra = Uf3_R.ping_cm();
    Serial.print("후방 거리 : ");
    Serial.print(backLeftUltra);
    Serial.print(" ");
    Serial.println(backRightUltra);
    if ((backLeftUltra + backRightUltra) / 2 <= 3 && backLeftUltra * backRightUltra != 0){
      stopCar();
      break;
    }
    
  }
  delay(1000);
  
  // 6. 전방 우측 센서 거리 10 밑으로 될때까지 차량 자세 재조정
  Serial.println("6차 체크");
  turnLeft();
  advance(255);
  while(1){
    frontLeftUltra = Uf4_L.ping_cm();
    frontRightUltra = Uf4_R.ping_cm();
    Serial.print("전방 거리 : ");
    Serial.print(frontLeftUltra);
    Serial.print(" ");
    Serial.println(frontRightUltra);
    if((frontLeftUltra + frontRightUltra) / 2 <= 3 && frontLeftUltra * frontRightUltra != 0){
      stopCar();
      break;
    }
  }
  /*
  turnRight();
  backUp(210);
  while(1){
    backLeftUltra = Uf3_L.ping_cm();
    backRightUltra = Uf3_R.ping_cm();
    if((backLeftUltra + backRightUltra) / 2 <= 3 && backLeftUltra * backRightUltra != 0){
      stopCar();
      break;
    }
  }
  */
  // 7. 휠 중립으로 두고 마무리
  Serial.println("5차 체크");
  turnNeutral();
  
  while(1);
}

// 함수 파트
void advance(int velocity){ // 차량 전진
  Serial.println("전진");
  analogWrite(speedPin, velocity);
  digitalWrite(MOTB, HIGH);
  digitalWrite(MOTA, LOW);  
}

void backUp(int velocity){ //차량 후진
  Serial.println("후진");
  analogWrite(speedPin, velocity);
  digitalWrite(MOTB, LOW);
  digitalWrite(MOTA, HIGH);  
}

void stopCar(){ // 정차
  analogWrite( speedPin , 0 );
  digitalWrite(MOTB, LOW);
  digitalWrite(MOTA, LOW);
}

void turnLeft(){
  while( angle >= 47){
    myServo.write(angle);
    angle -= 1;
    delay(100);
  }
}

void turnRight(){
  while( angle <= 133){
    myServo.write(angle);
    angle += 1;
    delay(100);
  }
}

void turnNeutral(){  //휠 중립으로 두기
  if(angle > 90){
    while(angle >= 90){
      myServo.write(angle);
      angle -= 1;
      delay(100);
    }
  }else if(angle < 90){
    while(angle <= 90){
      myServo.write(angle);
      angle += 1;
      delay(100);
    }
  }
}
