/* CPE 301 Final Project
 * Alexander Ram, Ian Ayers
 */

#include <DHT.h>
#include <LiquidCrystal.h>

//Port A register pointers (pins A0 to A7)
volatile unsigned char* port_a  = (unsigned char*) 0x22; //data register
volatile unsigned char* ddr_a   = (unsigned char*) 0x21; //direction register
volatile unsigned char* pin_a   = (unsigned char*) 0x20; //address register
//Port C register pointers (pins A8 to A15)
volatile unsigned char* port_c  = (unsigned char*) 0x28; //data register
volatile unsigned char* ddr_c   = (unsigned char*) 0x27; //direction register
volatile unsigned char* pin_c   = (unsigned char*) 0x26; //address register

//for screen control
const int RS = 78, EN = 77, D4 = 74, D5 = 73, D6 = 72, D7 = 71;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

//for motor control
const int Enable12 = 76;
const int Driver1A = 58;
const int Driver2A = 59;

void setup() {
  //A0 and A1 are for the LCD (outputs)
  *ddr_a |= 0x01 << 0;
  *ddr_a |= 0x01 << 1;
  //A4 to A7 are for the LCD (outputs)
  *ddr_a |= 0x01 << 4;
  *ddr_a |= 0x01 << 5;
  *ddr_a |= 0x01 << 6;
  *ddr_a |= 0x01 << 7;
  //A8 is for the water sensor (input)
  *ddr_c &= ~(0x01 << 8 - 8);  
  //A10 is for the humidity controller (input)
  *ddr_c &= ~(0x01 << 10 - 8);
  //A13 to A15 are for the motor contrl (output)
  *ddr_c |= 0x01 << 13 - 8;
  *ddr_c |= 0x01 << 14 - 8;
  *ddr_c |= 0x01 << 15 - 8;
  
  lcd.begin(16, 2);    //set 16 columns and 2 rows of 16x2 LCD
  
  //set pin direction for pins numbered above
  pinMode(Enable12,OUTPUT);
  pinMode(Driver1A,OUTPUT);
  pinMode(Driver2A,OUTPUT);
  
  Serial.begin(9600);
}

void motorController(byte speed, bool D1A, bool D2A){
  //to control the DC motor
  //speed: 0-255 as PWM, 0 = off, 255 = max
  //D1A: Input 1 or 1A, bool hi/low
  //D2A: Input 2 or 2A, bool hi/low
  analogWrite(Enable12, speed); //PWM
  digitalWrite(Driver1A, D1A); //bool
  digitalWrite(Driver2A, D2A); //bool
}

void loop() {
  Serial.println("forwards; then backwards");
  for (int i = 0; i < 5; ++i){
    //forwards 0.5 seconds
    motorController(255, HIGH, LOW);
    delay(500);
    //backwards 0.5 seconds
    motorController(255, LOW, HIGH);
    delay(500);
  }
  //halt
  motorController(0, LOW, HIGH);
  //2 second wait
  delay(2000);

  Serial.println("stopping slowly then stopping quickly");
  //forwards 3 seconds
  motorController(255, HIGH, LOW); //forwards
  delay(3000);
  //slow stop plus 2 seconds
  motorController(0, HIGH, LOW);
  delay(2000);
  //backwards 3 seconds
  motorController(255, LOW, HIGH);
  delay(3000);
  //quick stop plus 2 seconds
  motorController(255, LOW, LOW);
  delay(2000);

  Serial.println("full speed PWM then slow speed PWM");
  //PWM from full speed to about 21% speed
  //speed changes once per second
  for(int x = 255; x >= 55; x -= 25){
    motorController(x, HIGH, LOW);
    delay(1000);
  }
  //halt
  motorController(0, HIGH, LOW);
  //2 second wait
  delay(2000);

  Serial.println("slow speed PWM then full speed PWM");
  //PWM from about 21% speed to full speed
  //speed changes once per second
  for(int x = 55; x <= 255; x += 25){
    motorController(x, HIGH, LOW);
    delay(1000);
  }
  
  DHT dht(2, DHT11);

  //halt
  motorController(0,HIGH,LOW);
  //10 second wait
  delay(10000);
  
  //grab 40-bit data packet from DHT sensor
  int readDHT = dht.read(8);
  //move cursor to top left corner
  lcd.setCursor(0, 0);
  //print temperature
  lcd.print("Temp: ");
  lcd.print(dht.readTemperature(true, true));
  lcd.write(0xDF); //degree sign, can also use lcd.print((char)223);
  lcd.print("C");
  //move cursor down one line
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(dht.readHumidity(true));
  lcd.print("%");
  //3 second wait
  delay(3000);
}
