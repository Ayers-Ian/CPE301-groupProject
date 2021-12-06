/*
 * THESE ADDRESS THINGS ARE CALLED REGISTERS
 * AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 */


#include <dht.h>
#include <LiquidCrystal.h>

#define RDA 0x80
#define TBE 0x20

/* crap that I stole from lab 9 */
// UART Pointers
volatile unsigned char *myUCSR0A = (unsigned char*)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char*)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char*)0x00C2;
volatile unsigned int *myUBRR0 = (unsigned int* )0x00C4;
volatile unsigned char *myUDR0 = (unsigned char*)0x00C6;
// GPIO Pointers
volatile unsigned char *portB = (unsigned char*) 0x25;
volatile unsigned char *portDDRB = (unsigned char*) 0x24;
// Timer Pointers
volatile unsigned char *myTCCR1A = (unsigned char*) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char*) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char*) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char*) 0x6F;
volatile unsigned char *myTIFR1 = (unsigned char*) 0x36;
volatile unsigned int *myTCNT1 = (unsigned int* ) 0x84;
/* end crap from lab 9 */

//fill in this as needed i assume
volatile unsigned char */*name*/    = (unsigned char*) 0x/*address*/;
volatile unsigned char */*name*/    = (unsigned char*) 0x/*address*/;
volatile unsigned char */*name*/    = (unsigned char*) 0x/*address*/;
volatile unsigned char */*name*/    = (unsigned char*) 0x/*address*/;

/**
 *   NEW CODE
 *   I FOUND THE PORT A STUFF
 *   PAGE 96 of the guide on canvas
 */
//Port A register pointers
volatile unsigned char* port_a  = (unsigned char*) 0x22; //data register
volatile unsigned char* ddr_a   = (unsigned char*) 0x21; //direction register
volatile unsigned char* pin_a   = (unsigned char*) 0x20; //address register

dht DHT;

const int RS = 2, EN = 3, D4 = 4, D5 = 5, D6 = 6, D7 = 7;
LiquidCrystal lcd(RS,EN,D4,D5,D6,D7);   //set Uno pins that are connected to LCD, 4-bit mode
//D4 through D7 are output pins for the LCD: A4 through A7 for us
//RS is register select pin: A0 for us
//EN is some kind of enable pin: A1 for us

const int Enable12 = 5;  // PWM pin to L293D's EN12 (pin 1) 
const int Driver1A = 4;  // To L293D's 1A (pin 2)
const int Driver2A = 3;  // To L293D's 2A (pin 7)

void setup() {
  *ddr_a |= 0x80; //set PA7 to OUTPUT  /* These lines are also         */
  *ddr_a |= 0x40; //set PA6 to OUTPUT  /* references, we can just copy */
  *ddr_a |= 0x20; //set PA5 to OUTPUT  /* these lines to anywhere they */
  *ddr_a |= 0x10; //set PA4 to OUTPUT  /* are needed in the program,   */
  *ddr_a |= 0x08; //set PA3 to OUTPUT  /* if they are needed.          */
  *ddr_a |= 0x04; //set PA2 to OUTPUT  /* A lot of these lines are     */
  *ddr_a |= 0x02; //set PA1 to OUTPUT  /* probably never going to be   */
  *ddr_a |= 0x01; //set PA0 to OUTPUT  /* used but I'm almost certain  */
  *ddr_a &= 0x7F; //set PA7 to INPUT   /* we need at least 1 or 2 of   */
  *ddr_a &= 0xBF; //set PA6 to INPUT   /* these.                       */
  *ddr_a &= 0xDF; //set PA5 to INPUT
  *ddr_a &= 0xEF; //set PA4 to INPUT
  *ddr_a &= 0xF7; //set PA3 to INPUT
  *ddr_a &= 0xFB; //set PA2 to INPUT
  *ddr_a &= 0xFD; //set PA1 to INPUT
  *ddr_a &= 0xFE; //set PA0 to INPUT
  
  
  lcd.begin(16,2);    //set 16 columns and 2 rows of 16x2 LCD
  //---set pin direction
  pinMode(Enable12,OUTPUT);
  pinMode(Driver1A,OUTPUT);
  pinMode(Driver2A,OUTPUT);
  Serial.begin(9600);
}

void motorCTRL(byte speed, bool D1A, bool D2A){
    /*  motorCTRL controls the DC motor
     *    speed: any value between 0-255, used as PWM
     *             0 - off
     *           255 - maximum
     *      D1A: Input 1 or 1A, boolean value of HIGH or LOW          
     *      D2A: Input 2 or 2A, boolean value of HIGH or LOW
     */
  analogWrite(Enable12,speed);  // PWM
  digitalWrite(Driver1A,D1A);   // Boolean
  digitalWrite(Driver2A,D2A);   // Boolean 
}

void loop() {
  Serial.println("One way, then reverse");
    //---back and forth example
  for (int i = 0; i < 5; ++i){
        motorCTRL(255,HIGH,LOW);  // one way
    delay(500);
    motorCTRL(255,LOW,HIGH);  // reverse
    delay(500);
  }
  motorCTRL(0,LOW,HIGH);        // Stop
  delay(2000);

  Serial.println("fast Slow example");
  //---fast/slow stop example
  motorCTRL(255,HIGH,LOW);      // one way
  delay(3000);
  motorCTRL(0,HIGH,LOW);        // slow stop
  delay(2000);
  motorCTRL(255,LOW,HIGH);      // reverse
  delay(3000);
  motorCTRL(255,LOW,LOW);       // fast stop
  delay(2000);

  Serial.println("PWM full then slow");
  //---PWM example, full speed then slow
  for(int x = 255; x >= 55; x -= 25){
        motorCTRL(x,HIGH,LOW);
        delay(1000);
    }
  motorCTRL(0,HIGH,LOW);        // Stop
    delay(2000);

  Serial.println("PWM slow then full");
  //---PWM example, slow then full speed
    for(int x = 55; x <= 255; x += 25){
        motorCTRL(x,HIGH,LOW);
        delay(1000);
    }
  motorCTRL(0,HIGH,LOW);        // Stop
  delay(10000);
  int readDHT = DHT.read11(8);    //grab 40-bit data packet from DHT sensor
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  //lcd.print((char)223);         //used to display degree symbol on display
  //lcd.write(0xdf);              //another way to display degree symbol
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
  delay(3000);
}
