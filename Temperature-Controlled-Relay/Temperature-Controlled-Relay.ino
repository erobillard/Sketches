#include <SPI.h>
#include <SD.h>

/* Temperature-Controlled Relay 
 *  Eli Robillard eli@erobillard.com
 *  This sketch turns a relay on when the temperature falls below the threshold in degrees celcius, 
 *  and off when the temperature is above. Times between samples are set separately for the on and off states.
 *  Parts: 
 *  Arduino Uno (or compatible)
 *  LM35DZ thermostat
 *  Grove 30V SPDT Relay and cable (Sig to Digital pin 7)
 *  RTC Data logger shield (uses pin 4)
 */

// set up variables using the SD utility library functions:
File logFile; 
const int sdSelectPin=4; // 4 is reserved to read the on-board switch
int loggingEnabled=1; // Value is set in setup if it finds a writable chip and the logging shield

const int LM35Pin = A1;
const int LM35Ref = A2;
const int sigPin = 7;
int tempRaw = 9999; 
float tempC = 9999; 
float tempC2 = 9999;
int tempA = 9999;
int tempD = 9999;
int stateNow = 0; 
int thresholdOn = 22; 
int thresholdOff = 24; 
int msOff = 5000; // ms between samples when Off 
int msOn = 20000; // ms between samples when On 

void setup() {
  Serial.begin(9600);
  if (SD.begin(4)) {
    Serial.println("SD card initialized for logging.");
    loggingEnabled=1;
  } else {
    Serial.println("Logging disabled: SD card not present or logging shield not installed.");        
    loggingEnabled=0;
  }

  if (loggingEnabled==1) {
    if (SD.exists("data.csv")) {
      Serial.println("data.csv exists, appending new data.");
    } else {
      Serial.println("data.csv doesn't exist, file will be created.");
    }
  }
  pinMode(LM35Pin, INPUT); // set the thermostat's signal pin as input 
  pinMode(LM35Ref, INPUT); // set the LM35's ground reference as input 
  pinMode(sigPin, OUTPUT); // set the relay's signal pin as output 
  PowerOnTest(); 
}
 
void loop() {
  tempRaw = analogRead(LM35Pin); 
  tempC = readLM35(true); 
  if ((tempC<=thresholdOn) && stateNow==0) {
    PowerOn(); 
  }
  if ((stateNow==1) && (tempC>thresholdOff)) {
    PowerOff(); 
  }
  writeStatus(); 
  delay(msOff);   
}

void PowerOnTest() {
  initStatus();
  digitalWrite(sigPin, HIGH); 
  delay(2000);   
  digitalWrite(sigPin, LOW); 
}

void PowerOn() {
  digitalWrite(sigPin, HIGH); 
  stateNow=1;
  writeStatus();
  delay(msOn);
}

void PowerOff() {
  digitalWrite(sigPin, LOW); 
  stateNow = 0; 
  writeStatus(); 
}

void initStatus() {
  Serial.println("Temp, RawTemp, RawRef, Status, KY028 Analog, Calculated");
  logStatus("Temp, RawTemp, RawRef, Status");
}

void writeStatus() {
  String logStr = "";
  logStr += String(tempC); 
  logStr += ",";
  logStr += String(tempRaw);
  logStr += ",";
  logStr += String(stateNow); 
  Serial.println(logStr); 
  logStatus(logStr);
}

void logStatus(String logStr) {
  if (loggingEnabled==1) {
    logFile = SD.open("data.csv", FILE_WRITE);  
    logFile.println(logStr);
    logFile.close();
  } 
  else { // if the file isn't open, display an error
    Serial.println("Error opening data.csv");
    logFile.close(); 
  }
}

float readLM35(boolean celcius){      
   int analogVal = 0;
   for(int i = 0; i < 10; i++) {                   // takes 10 samples to make sure we get a good value
     analogVal += (analogRead(LM35Pin) - analogRead(LM35Ref)); // subtract Vout ADC reading from LM35 ground ADC reading
     delay(100);
   }
   float tempCelsius = (50.0 * analogVal) / 1023;
   if (celcius == true) {
     return tempCelsius; // return temperature in degrees Celcius
   }
   else {
     return (tempCelsius * 9 / 5) + 32; // return temperature in degrees Fahrenheit
   }
}
