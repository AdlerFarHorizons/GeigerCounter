const String fwver = "1.0.1";

/*  
  A compatible logic level pulsed detector output from the Geiger counter is
  required and connected to Pin 3 (INT1) configured for rising edge interrupt
  trigger.
  
  SD Card file is created or appended if the card is present.
  
  An optional LED between Pin 9 and GND indicates presence of valid SD card.
 
  An optional LM60 temperature sensor with pins connected to 5V, A0, GND in
  order from left to right, flat side facing you.
*/

#include <MsTimer2.h>
#include <SD.h>

#define filename "GEIGEREX.TXT"

const int chipSelect = 10;
const int ledPin = 9;
const int vTempPin = 0;
const float vRef = 5.0;

long deltaT;
volatile int crCount;
File dataFile;
float time;
volatile boolean active;

void setup() {
  
  deltaT = 5000;
  Serial.begin(9600);
  Serial.flush();
  
  Serial.print("Initializing SD card...");

  // SD library requires Pin 10 as output, even if not used as CS:
  pinMode(10, OUTPUT);
  analogWrite( ledPin, 50 );
  
  if (!SD.begin(chipSelect)) {
    
    Serial.println("Card failed or not present.");
    analogWrite( ledPin, 0 );
    
  } else {
    
    Serial.println("card initialized.");
    analogWrite( ledPin, 50 );
    
  }
  
  dataFile = SD.open( filename, FILE_WRITE );

  Serial.println( "\n==== Counter Started =====\n" );
  Serial.print( "Count Window:" );Serial.print( deltaT / 1000.0, 3 );
  Serial.print( " sec\n" );
  Serial.flush(); 

  if ( dataFile ) {
    dataFile.println( "\n==== Counter Started =====\n" );
    dataFile.print( "Count Window:," );dataFile.print( deltaT / 1000.0, 3 );
    dataFile.println( ",sec\n" );
    dataFile.println( "Window Start (sec), Counts" ); //, T (C)" );
    dataFile.flush();
  }

  MsTimer2::set( deltaT, timesUp );
  MsTimer2::start();
  active = true;
  crCount = 0;
  attachInterrupt( 1, increment, RISING );
  time = 0;
  
}

void loop() {
  
  while( active );
  detachInterrupt( 1 );
  long tempCount = crCount;
  crCount = 0;
  attachInterrupt( 1, increment, RISING );
  active = true;

  Serial.print( time, 3 );Serial.print( " sec: " );
  Serial.print( tempCount );Serial.print( " counts " );;
  //Serial.print( readTemp( vTempPin, 0 ) );Serial.print( " deg C");
  Serial.println("");
  if (dataFile) {
    
    dataFile.print( time, 3 );dataFile.print( ",");
    dataFile.print( tempCount );dataFile.print( ",");
    //dataFile.println( readTemp( vTempPin, 0 ) );
    dataFile.println("");
    dataFile.flush();
    
  }  
  
  time += deltaT / 1000.0;
  
}

void increment() {
  
  crCount++;
  
}

void timesUp() {
  
  active = false;
  
}

float readTemp( int pin, int sensType ) {
  // Temperature Sensor constants:
  //   0  LM60
  //   1  MAX6605
  //   2  TMP36
  int mVoltsAtRefTemp[] = { 424, 744, 750 };
  int refTempC[] = { 0, 0, 25 };
  float mVperDegC[] = { 6.25, 11.9, 10.0 };

  int reading = analogRead(pin);
  float mVolts = reading * vRef / 1.024;

  return( ( mVolts - mVoltsAtRefTemp[sensType] ) / 
            ( mVperDegC[sensType] ) + 
            refTempC[sensType]);
  
}

