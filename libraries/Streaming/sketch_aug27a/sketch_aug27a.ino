
#include <Time.h>
#include <DS1302RTC.h>
/********************************************
  Shift register configuration

 */
//Pin connected to ST_CP of 74HC595
int latchPin = 10;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;
/**********************************************/


/************************************************
 DS1302RTC configuration
 ***********************************************/
// Init the DS1302
// Set pins:  CE, IO,CLK
DS1302RTC RTC(7, 6, 5);

// Optional connection for RTC module
#define DS1302_GND_PIN 4
#define DS1302_VCC_PIN 3
/**********************************************/


//holders for infromation you're going to pass to shifting function
byte data;
byte dataArray[10];
byte minuteLed[9];
byte ledAddress[9];

void setup() {
  Serial.begin(9600);
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  setupRTC();


  //Binary notation as comment
  //  dataArray[0] = 0xFF; //0b11111111
  //  dataArray[1] = 0xFE; //0b11111110
  //  dataArray[2] = 0xFC; //0b11111100
  //  dataArray[3] = 0xF8; //0b000
  //  dataArray[4] = 0xF0; //0b11110000
  //  dataArray[5] = 0xE0; //0b11100000
  //  dataArray[6] = 0xC0; //0b11000000
  //  dataArray[7] = 0x80; //0b10000000
  //  dataArray[8] = 0x00; //0b00000000
  //  dataArray[9] = 0xE0; //0b11100000

  minuteLed[0] = 0x00;
  minuteLed[1] = 0x01;
  minuteLed[2] = 0x03;
  minuteLed[3] = 0x07;
  minuteLed[4] = 0x0F;

  ledAddress[0] = 0x01;
  ledAddress[1] = 0x02;
  ledAddress[2] = 0x04;
  ledAddress[3] = 0x08;
  ledAddress[4] = 0x10;
  ledAddress[5] = 0x20;
  ledAddress[6] = 0x40;
  ledAddress[7] = 0x80;

}

void setupRTC() {

  Serial.println("DS1302RTC Read Test");
  Serial.println("-------------------");
  // Activate RTC module
  digitalWrite(DS1302_GND_PIN, LOW);
  pinMode(DS1302_GND_PIN, OUTPUT);

  digitalWrite(DS1302_VCC_PIN, HIGH);
  pinMode(DS1302_VCC_PIN, OUTPUT);

  Serial.println("RTC module activated");
  Serial.println();
  delay(500);

  if (RTC.haltRTC()) {
    Serial.println("The DS1302 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  }
  if (!RTC.writeEN()) {
    Serial.println("The DS1302 is write protected. This normal.");
    Serial.println();
  }

  delay ( 2000 );

  Serial.println("RTC Sync");
  setSyncProvider(RTC.get); // the function to get the time from the RTC
  if (timeStatus() == timeSet)
    Serial.println(" Ok!");
  else
    Serial.println(" FAIL!");

   //last led
   pinMode(8, OUTPUT);

}

void loop() {
  //testLEDs(1);
  int minuteInt = minute();
  int hourInt = hour();
  showTime(hourInt, minuteInt);
 // writedata(ledAddress[minuteInt % 5], ledAddress[minuteInt % 5], ledAddress[minuteInt % 5]);


  delay(1000);

}
void showTime(int hour, int minute) {

  byte shift1 = ledAddress[4];//it is
  byte shift2 = 0x00;
  byte shift3 = 0x00;
  if (hour > 12)
    hour = hour - 12;

  String clockText = "";
  clockString(minute, hour, clockText);
  Serial.println(clockText);
 // int testval = clockText.indexOf("ten");
//Serial.println(testval);
  if(clockText.indexOf("aquarter")>-1)
    shift1 += ledAddress[5];
    
  if(clockText.indexOf("twentyfive")>-1)
    shift1 += ledAddress[6] + ledAddress[7]; 
  else if(clockText.indexOf("twentyfour")>-1)
  {
    shift1+= ledAddress[7];
    shift3 += ledAddress[0];
  }
  else if(clockText.indexOf("twenty")>-1)
  {
    shift1 += ledAddress[6];
  }
  
  else if(clockText.indexOf("five")>-1)
    shift1 += ledAddress[7];
    
  if(clockText.indexOf("half")>-1)
    shift2 += ledAddress[0];

  if(clockText.indexOf("ten")>-1){
    shift2 += ledAddress[1];
  }

  if(clockText.indexOf("to")>-1){
    shift2 += ledAddress[2];
  }
    
  if(clockText.indexOf("past")>-1)
    shift2 += ledAddress[3];

  if(clockText.indexOf("nine")>-1)
    shift2 += ledAddress[4];
    
  if(clockText.indexOf("one")>-1)
    shift2 += ledAddress[4];
    
  if(clockText.indexOf("six")>-1)
    shift2 += ledAddress[5];
    
  if(clockText.indexOf("three")>-1)
    shift2 += ledAddress[7];
    
  if(clockText.indexOf("four")>-1)
    shift3 += ledAddress[0];
    
  if(clockText.indexOf("past five")>-1)
    shift3 += ledAddress[1];
    
  if(clockText.indexOf("two")>-1)
    shift3 += ledAddress[2];
    
  if(clockText.indexOf("eight")>-1)
    shift3 += ledAddress[3];
    
  if(clockText.indexOf("eleven")>-1)
    shift3 += ledAddress[4];
    
  if(clockText.indexOf("seven")>-1)
    shift3 += ledAddress[5];
    
  if(clockText.indexOf("tweleve")>-1)
    shift3 += ledAddress[6];

  if(clockText.indexOf("past ten")>-1){
    shift3 += ledAddress[7];
  }
  
  if(clockText.indexOf("o'clock")>-1)
    digitalWrite(8, HIGH);
  else
    digitalWrite(8, LOW);

  shift1 += minuteLed[minute % 5];
  Serial.println(shift1);
  Serial.println(shift2);
  Serial.println(shift3);
  
  writedata(shift1, shift2, shift3);

}

void clockString(int minuteToCheck, int hour, String &clockText) {

  String hourText = "";
  String hourText2 = "";
  int  newHour = hour + 1 > 12 ? 1 : hour + 1;

  if (minuteToCheck < 5) {
    numberText(hour, hourText);
    clockText += hourText;
    clockText += " o'clock";
  }
  else if (minuteToCheck < 10)
  {
    numberText(5, hourText);
    numberText(hour, hourText2);
    clockText += hourText;
    clockText += " past ";
    clockText += hourText2;
  }

  else if (minuteToCheck < 15)
  {
    numberText(10, hourText);
    numberText(hour, hourText2);
    clockText += hourText;
    clockText += " past ";
  }
  else if (minuteToCheck < 20) {
    numberText(hour, hourText);
    clockText += "aquarter past ";

    clockText += hourText;
  }

  else if (minuteToCheck < 25) {
    numberText(20, hourText);
    numberText(hour, hourText2);
    clockText += hourText;
    clockText += " past ";
    clockText += hourText2;
  }
  else if (minuteToCheck < 30) {
    numberText(25, hourText);
    numberText(hour, hourText2);
    clockText += hourText;
    clockText += " past ";
    clockText += hourText2;
  }

  else if (minuteToCheck < 35) {
    numberText(hour, hourText);
    clockText += "half past ";
    clockText += hourText;

  }

  else if (minuteToCheck < 40)
  {
    numberText(25, hourText);
    numberText(newHour, hourText2);

    clockText += hourText;
    clockText += " to ";
    clockText += hourText2;
  }
  else if (minuteToCheck < 45) {
    numberText(20, hourText);
    numberText(newHour, hourText2);

    clockText += hourText;
    clockText += " to ";
    clockText += hourText2;
  }
  else if (minuteToCheck < 50) {
    numberText(newHour, hourText);
    clockText +=  "aquarter to ";
    clockText += hourText;
  }
  else if (minuteToCheck < 55) {
    numberText(10, hourText);
    numberText(newHour, hourText2);
    clockText +=   hourText;
    clockText += " to ";
    clockText += hourText2;
  }
  else if (minuteToCheck < 60) {
    numberText(5, hourText);
    numberText(newHour, hourText2);
    clockText += hourText;
    clockText += " to ";
    clockText += hourText2;
  }
  else if (minuteToCheck < 65)
  {
    numberText(hour + 1, hourText);
    clockText += hourText;
    clockText += " o'clock";
  }

  return;

}

void numberText(int hourToCheck, String &hourText)
{

  switch (hourToCheck) {
    case 1:
      hourText = "one"; break;
    case 2:
      hourText = "two"; break;
    case 3:
      hourText = "three"; break;
    case 4:
      hourText = "four";  break;
    case 5:
      hourText = "five"; break;
    case 6:
      hourText = "six"; break;
    case 7:
      hourText = "seven";   break;
    case 8:
      hourText = "eight"; break;
    case 9:
      hourText = "nine"; break;
    case 10:
      hourText = "ten"; break;
    case 11:
      hourText = "eleven"; break;
    case 12:
      hourText = "twelve"; break;
    case 20:
      hourText = "twenty"; break;
    case 25:
      hourText = "twentyfive"; break;
    default:
      hourText = "twentyfour"; break;
  }
}

void testLEDs(int set) {
  if (set == 1) {
    for (int i = 0; i <= 8; i++) {
     // Serial.println(ledAddress[i]);
      writedata(ledAddress[i], ledAddress[i], ledAddress[i]);
      delay(1000);
    }
  }
}
void writedata(byte set1, byte set2, byte set3) {

  //ground latchPin and hold low for as long as you are transmitting
  digitalWrite(latchPin, 0);
  //move 'em out
  shiftOut(dataPin, clockPin, set3);
  shiftOut(dataPin, clockPin, set2);
  shiftOut(dataPin, clockPin, set1);

  //return the latch pin high to signal chip that it
  //no longer needs to listen for information
  digitalWrite(latchPin, 1);
}

// the heart of the program
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i = 0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
  for (i = 7; i >= 0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}



