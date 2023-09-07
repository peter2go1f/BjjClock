#include <Arduino.h>

int D0 = 8;   // Binary Coded Decimal input for Decoder
int D1 = 9;   // Binary Coded Decimal input for Decoder
int D2 = 10;   // Binary Coded Decimal input for Decoder
int D3 = 11;   // Binary Coded Decimal input for Decoder

int LT = 2;   // LT LOW turns all segments on
int BL = 3;   // BL LOW while LT HIGH turns all segments off

int digit1Pin = 4;    // Output pin for digit 1
int digit2Pin = 5;    // Output pin for digit 2
int digit3Pin = 6;   // Output pin for digit 3



// Timer 2 has 8 bits = 2^8 - 1 = 255 ticks
// Arduino UNO clock is 16MHz

// Using Timer 2 for cycling through digits
// Prescaler of 1024 gives frequency: 16MHz/1024 = 15,625Hz
// Period is: 1/15,625Hz = 64 microseconds
// ticks = (displayPeriod / Period) = (5 ms / 0.064 ms) = 78.125 ticks

int displayFrequency = 200;              // Hz - how fast the program cycles through each digit
int displayPeriod = 1/displayFrequency;  // 1/200 = 5 milliseconds
long ticksTimer2 = 78;          // 78.125 ticks in one second with 200Hz display frequency



// BJJ clock/timer
// Timer 1 has 16 bits = 2^16 - 1 = 65,535 ticks
// Prescaler of 256 gives frequency: 16MHz/256 = 62,500Hz
// Period is: 1/62,500 = 16 microseconds
// ticks per second = (1s / 16micros) = 62,500 ticks

int digit = 3;               // Each 7-segment display is a digit. Digits 3 to 1
int bjjTimer;                // Timer clock that counts down
int whiteBeltMatch = true;   // White belt matches = 6 minute round
int blueBeltMatch = false;   // Blue belt matches = 7 minute round
int mins;                    // Minutes of the clock          (3rd digit of clock)
int tensSecs = 0;            // Tens of seconds of the clock  (2nd digit of clock)
int secs = 0;                // Seconds of the clock          (1st digit of clock)
long ticksTimer1 = 62500;      // 62,500 ticks per second for Timer 1 with prescaler of 256

bool endOfMatch = false;     // True when bjjTimer reaches 0:00

// Function declarations
int digitPin(int digit);
void activateDigit(int digit);
void turnOffDigit(int digit);
void displaySegmentOff();
void displaySegment0();
void displaySegment1();
void displaySegment2();
void displaySegment3();
void displaySegment4();
void displaySegment5();
void displaySegment6();
void displaySegment7();
void displaySegment8();
void displaySegment9();
void displayDigit(int number);


//////////////////////////////////////////////////////////////////////////////////////
void setup() {

  //cli();  // stop interrupts
  Serial.begin(9600);  // start the arduino hardware serial port at 9600 baud

  // Timer/Counter 2 set up for cycling through digits
  TCCR2A = B00000000; // set Timer 2 to mode 0, Timer 2 is 8-bit
  TCCR2B = B00000111; // set Timer 2 with prescaler of 8
  
  // Timer/Counter 1 set up for counting down timer each second
  TCCR1A = B00000000; // set Timer 1 to mode 0, Timer 1 is 16-bit
  TCCR1B = B00000100; // set Timer 1 with prescaler of 256

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(digit1Pin, OUTPUT);
  pinMode(digit2Pin, OUTPUT);
  pinMode(digit3Pin, OUTPUT);

  pinMode(LT, OUTPUT);
  pinMode(BL, OUTPUT);

  // Initialise bjjClock with a 6 or 7 minute timer
  if (whiteBeltMatch) {
    mins = 6;
  } else if (blueBeltMatch) {
    mins = 7;
  }
  
  TCNT1 = 0;
}


////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  while (!endOfMatch){
    // Cycle through digits 3 to 1
    if (TCNT2 > ticksTimer2)    // triggers every 200Hz or 5 milliseconds
    { 
      TCNT2 = 0;
      if (digit == 0) {   // digits go from 3 to 1
        digit = 3;   
        activateDigit(digit);           // Turn on digit
      } else {
          displaySegmentOff();            // Make sure segments are all off
          turnOffDigit(digit);            // Turn digit off
          digit --;                       // go to next digit down;
          if (digit != 0){
          activateDigit(digit);           // Turn on digit
          }
        }
      }
      
    // Count down BjjTimer every second
    if (TCNT1 > ticksTimer1) {       // Triggers every 1 second
      TCNT1 = 0;
      if (secs == 0) {
        if (tensSecs == 0) {
          if (mins == 0) {           // BjjTimer has reached 0:00
            endOfMatch = true;       // End of match
          } else {
            mins--;                  // Decrement mins (3nd digit) and set seconds to X:59
            tensSecs = 5;
            secs = 9;
          }
        } else {
          tensSecs--;               // Decrement tensSecs (2nd digit) and set seconds to X:X9
          secs = 9;
        }
      } else {
        secs--;           // count down every second
      }
    }

    // Debugging
    //  Serial.print(mins);
    //  Serial.print(":");
    //  Serial.print(tensSecs);
    //  Serial.print(secs);
    //  Serial.println();

    // Turn on segments for the appropriate digit
    switch (digit) {
      case 3:
        displayDigit(mins);
        break;
      case 2:
        displayDigit(tensSecs);
        break;
      case 1:
        displayDigit(secs);
        break;
    }
  }
 
  // End of match, display 0 on all 3 digits
}

    
  
  


////////////////////////////////////////////////////////////////////////////////////////

// Functions

void activateDigit(int digit) {
  switch (digit) {
    case 3:
      digitalWrite(digit3Pin, HIGH);
      break;
    case 2:
      digitalWrite(digit2Pin, HIGH);
      break;
    case 1:
      digitalWrite(digit1Pin, HIGH);
      break;
  }
}

void turnOffDigit(int digit) {
  switch (digit) {
    case 3:
      digitalWrite(digit3Pin, LOW);
      break;
    case 2:
      digitalWrite(digit2Pin, LOW);
      break;
    case 1:
      digitalWrite(digit1Pin, LOW);
      break;
  }
}

int digitPin(int digit) {    // takes argument "digit" and returns the correct pin
  int pin;
  switch (digit) {
    case 3:
      pin = digit3Pin;
      break;
    case 2:
      pin = digit2Pin;
      break;
    case 1:
      pin = digit1Pin;
      break;
  }
  return pin;
}

void displaySegmentOff() {
  digitalWrite(LT, HIGH);  // Turn all segments off
  digitalWrite(BL, LOW);   // Turn all segments off
}

void displaySegment0() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, LOW);   // 0
  digitalWrite(D1, LOW);  
  digitalWrite(D2, LOW);  
  digitalWrite(D3, LOW);  
}

void displaySegment1() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, HIGH);   // 1
  digitalWrite(D1, LOW);  
  digitalWrite(D2, LOW);  
  digitalWrite(D3, LOW);  
}

void displaySegment2() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, LOW);   // 2
  digitalWrite(D1, HIGH);  
  digitalWrite(D2, LOW);  
  digitalWrite(D3, LOW);  
}

void displaySegment3() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, HIGH);   // 3
  digitalWrite(D1, HIGH);  
  digitalWrite(D2, LOW);  
  digitalWrite(D3, LOW);  
}

void displaySegment4() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, LOW);   // 4
  digitalWrite(D1, LOW);  
  digitalWrite(D2, HIGH);  
  digitalWrite(D3, LOW);  
}

void displaySegment5() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, HIGH);   // 5
  digitalWrite(D1, LOW);  
  digitalWrite(D2, HIGH);  
  digitalWrite(D3, LOW);  
}

void displaySegment6() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, LOW);   // 6
  digitalWrite(D1, HIGH);  
  digitalWrite(D2, HIGH);  
  digitalWrite(D3, LOW);  
}

void displaySegment7() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, HIGH);   // 7
  digitalWrite(D1, HIGH);  
  digitalWrite(D2, HIGH);  
  digitalWrite(D3, LOW);  
}

void displaySegment8() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, LOW);   // 8
  digitalWrite(D1, LOW);  
  digitalWrite(D2, LOW);  
  digitalWrite(D3, HIGH);  
}

void displaySegment9() {
  digitalWrite(LT, HIGH);  // Allow segments to activate
  digitalWrite(BL, HIGH);
  
  digitalWrite(D0, HIGH);   // 9
  digitalWrite(D1, LOW);  
  digitalWrite(D2, LOW);  
  digitalWrite(D3, HIGH);  
}

void displayDigit(int number) {
  switch (number) {
    case 0:
      displaySegment0();
      break;
    case 1:
      displaySegment1();
      break;
    case 2:
      displaySegment2();
      break;
    case 3:
      displaySegment3();
      break;
    case 4:
      displaySegment4();
      break;
    case 5:
      displaySegment5();
      break;
    case 6:
      displaySegment6();
      break;
    case 7:
      displaySegment7();
      break;
    case 8:
      displaySegment8();
      break;
    case 9:
      displaySegment9();
      break;
  }
}
