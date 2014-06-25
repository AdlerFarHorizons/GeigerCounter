/*
  Adapted from CosmicRayCounter V2.1 software
*/

#include <MsTimer2.h>
#include <SD.h>

const int chipSelect = 10;

long deltaT;
volatile int crCount;
File dataFile;
long int time;

void setup() {
  deltaT = 5000;
  Serial.begin(9600);
  Serial.flush();
  Serial.print("Initializing SD card...");
  //make sure that the default chip select pin is set to
  //output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  dataFile = SD.open( "datalog.txt", FILE_WRITE );
  dataFile.println( "\n==== Counter Started =====" );
  dataFile.println( "time window start (ms), counts in time window" );
  dataFile.flush(); 
  
  attachInterrupt(1, increment, RISING);
}

void loop()
{  
  time = millis();
  crCount = 0;
  delay( deltaT );
  Serial.println( crCount );
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    
    //dataFile.println(dataString);
    dataFile.print( time );dataFile.print( ",");dataFile.println( crCount );
    dataFile.close();
    // print to the serial port too:   

  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  
}

void increment()
{
  crCount++;
}

