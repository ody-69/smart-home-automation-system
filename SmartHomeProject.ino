#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>
//------------------------------------------------------------
LiquidCrystal_I2C LCD(0x27, 16, 2);
SoftwareSerial BT(12, 13);
Servo S;
//------------------------------------------------------------
const int tempSensor = A1;
const int doorIr = 5;
const int irPin = A2;
const int buzzerPin = 2;
const int pirPin = 7;
const int ledPin = 8; //if there is movement
const int motorPin1 = 10;
const int motorPin2 = 11;
const int motorSpeed = 9;
const int LDR_PIN = A0;
const int SWITCH_PIN = 4;
const int LIGHT_PIN = 6;  //for ldr
const int potPin = A3;
//------------------------------------------------------------
const int lightThreshold = 300;
int tempThreshold = 26;
//------------------------------------------------------------
int mSpeed = 60;

bool securityMode = false;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 1000;

double temp;

double lastTemp = -1000;
String lastDoorState = "";
String lastSecurityState = "";
//------------------------------------------------------------
String code = "nowWay";
//------------------------------------------------------------
void setup() {
  setupPins();
}
//------------------------------------------------------------
void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;
    temp = tempCalc(tempSensor);
    lcdSetting(temp);
  }

  Security(securityMode, buzzerPin, irPin, pirPin, ledPin);
  doorLight();
  int potValue = analogRead(potPin);         
  tempThreshold = map(potValue, 0, 1023, 20, 50);
  coolingFan(mSpeed, temp);
  bluetoothControl();
}
//------------------------------------------------------------
//FUNCTIONS
void setupPins() {
  pinMode(irPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(doorIr, INPUT);
  pinMode(tempSensor, INPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  BT.begin(9600);
  BT.setTimeout(500);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  LCD.init();
  LCD.backlight();
  LCD.clear();
}






//------------------------------------------------------------
void doorOpening() {
  if (digitalRead(doorIr) == HIGH) return;
  S.attach(3);
  for (int pos = 0; pos <= 160; pos++) {
    S.write(pos);
    delay(10);
  }
  S.detach();
}
//------------------------------------------------------------
void doorClose() {
  if (digitalRead(doorIr) == LOW) return; //new line 
  S.attach(3);
  S.write(0);
  delay(300);
  S.detach();
}
//------------------------------------------------------------
void working(String read) {
  if (read == code) {
    doorOpening();
    BT.println("Door opened.");
  }
  else {
    doorClose();
    BT.println("Wrong code.");
  }   
}
 




//------------------------------------------------------------
void doorLight() {
  int ldrValue = analogRead(LDR_PIN);
  int switchState = digitalRead(SWITCH_PIN); 
  if (switchState == HIGH || ldrValue < lightThreshold) {
    digitalWrite(LIGHT_PIN, HIGH);
  } else {
    digitalWrite(LIGHT_PIN, LOW);
  }
}





//------------------------------------------------------------
double tempCalc(int tempSensor){
  double voltage;
  double rawValue;
  rawValue = analogRead(tempSensor);
  voltage = (((rawValue*5))/1023)*1000;
  temp = voltage*0.1;
  return temp;
}
//------------------------------------------------------------
void coolingFan(int speed, double temp){
  if (temp >tempThreshold){
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    analogWrite(motorSpeed, speed);
  }
  else{
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
  }
}






//------------------------------------------------------------
void windowIR(int buzzerPin, int irPin){
  int irRead = digitalRead(irPin);
  if ( irRead == LOW ) //if the window is closed
  {
    digitalWrite(buzzerPin, LOW);
  }
  else {
    digitalWrite(buzzerPin, HIGH);
  }
}
//------------------------------------------------------------
void ifMovement(int pirPin, int ledPin){
  digitalWrite(ledPin, HIGH);
  
  // int pirRead = digitalRead(pirPin);
  // if ( pirRead == HIGH ) //if there is movement
  // {
  //   digitalWrite(ledPin, HIGH);
  // }
  // else {
  //   digitalWrite(ledPin, LOW);
  // }
}
//------------------------------------------------------------
void Security(bool mode, int buzzerPin,
                  int irPin, int pirPin, int ledPin){
  
  if (mode){
    windowIR(buzzerPin, irPin);
    ifMovement(pirPin, ledPin);
  }
  else {
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);
  }
}




//------------------------------------------------------------
String doorState(int doorIr){
  int doorRead = digitalRead(doorIr);
  if (doorRead == HIGH){  
    return "Opened"; 
  }
  else {return "Closed";}
}
//------------------------------------------------------------
String securityState(bool securityMode){
  if (securityMode){
    return "ON";
  }
  else {return "OFF";}
}
//------------------------------------------------------------
void lcdSetting(double temp){
  String newDoorState = doorState(doorIr);
  String newSecurityState = securityState(securityMode);

  if (temp != lastTemp || newDoorState != lastDoorState || newSecurityState != lastSecurityState) {
  LCD.clear();
  LCD.setCursor(0,0);
  LCD.print("T:");
  LCD.print(temp,1);
  LCD.print((char)223);
  LCD.print("C, SM:");
  LCD.print(newSecurityState);
  LCD.setCursor(0,1);
  LCD.print("Door: ");
  LCD.print(newDoorState);

  lastTemp = temp;
  lastDoorState = newDoorState;
  lastSecurityState = newSecurityState;
  }
}
//------------------------------------------------------------





//------------------------------------------------------------
void bluetoothControl() {
  if (!BT.available()) return;

  String read = BT.readString();
  read.trim();

  if (read == "security_on") {
    securityMode = true;
    BT.println("Security Mode: ON");
  } 
  else if (read == "security_off") {
    securityMode = false;
    BT.println("Security Mode: OFF");
  }
  else if (read == "door_code") {
    BT.println("Enter the door code:");
    while (!BT.available()); 
    String codeRead = BT.readString();
    codeRead.trim();
    working(codeRead);
    delay(10000); //system is stopped
    doorClose();
    BT.println("Door closed");
  }
  else if (read == "open_door") {
    doorOpening();
    BT.println("Door manually opened");
  }
  else if (read == "close_door") {
    doorClose();
    BT.println("Door closed");
  }
  else if (read == "reset_pass") {
    BT.println("Enter the brevios password: ");
    while (!BT.available());
    String codeRead = BT.readString();
    codeRead.trim();
    if (codeRead == code) {
      BT.println("Enter the new password: ");
      while (!BT.available());
      String newCode = BT.readString();
      newCode.trim();
      code = newCode;
      BT.println("Password Updated.");
    }
    else {
      BT.println("Wrong code.");
    }
  }
  else if (read == "status") {
    BT.print("Temp: ");
    BT.print(temp);
    BT.print("°C | Door: ");
    BT.print(doorState(doorIr));
    BT.print(" | Security Mode: ");
    BT.println(securityState(securityMode));
  }
  else if (read == "temp_limit") {
    BT.println("Enter the tempreature: ");
    while (!BT.available());
    String codeRead = BT.readString();
    codeRead.trim();
    tempThreshold = codeRead.toInt();
  }
  else {
    BT.println("Invalid command.");
  }
}