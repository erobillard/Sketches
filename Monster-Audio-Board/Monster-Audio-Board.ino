/*  Excape Room Monster: Audio Sub-board
  Eli Robillard May 2017
  Built for Arduino Uno or Nano

  Revision 1705v1 change log
  - First draft
  - Listen to main board on pin 4, play when high, off when low.

*/
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define controlPin 8  // Main board to digital pin 8
#define volumePin A3  // Pot wiper to analog pin 3
#define busyPin A4    // Busy (playing) indicator, low==busy, high==off
#define onPin 12       // Always on, used to test the control pin
#define rxPin 10      // Receive pin
#define txPin 11      // Transmit pin

int volumeVal = 0; // stores the value read from the pot (0 to 1023)
int volumeOut = 0; // the volume set on the device (0 to 30)
bool soundOff = false; 

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup()
{
  pinMode(volumePin, INPUT);
  pinMode(busyPin, INPUT);
  pinMode(controlPin, INPUT_PULLUP);
  pinMode(onPin, OUTPUT);
  digitalWrite(onPin,HIGH); 

  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please check the connection!"));
    Serial.println(F("2.Please check the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  setVolume();

  myDFPlayer.play(3);  //Play the third track
  delay(14000);

  // EQ selection
  //  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
}

void loop()
{
  printStatus(); 
  soundOff = (digitalRead(controlPin)==0); // disable / enable sound based on the state of the controlPin
  while (soundOff) { // Wait for the control pin to light up / go high
      myDFPlayer.stop(); 
      Serial.println("Off");
      delay(2000); 
      soundOff = (digitalRead(controlPin)==0); 
      printStatus(); 
  }
  while (!soundOff) {
    setVolume();
    
    myDFPlayer.playMp3Folder(1); //play 0001.mp3
    while (((analogRead(busyPin)<500))&&(!soundOff)) {
      delay(10000);
      Serial.println("0001.mp3");
      printStatus(); 
      soundOff = (digitalRead(controlPin)==0); 
    } 

    myDFPlayer.playMp3Folder(2); //play 0002.mp3
    while (((analogRead(busyPin)<500))&&(!soundOff)) {
      delay(10000);
      Serial.println("0002.mp3");
      printStatus(); 
      soundOff = (digitalRead(controlPin)==0); 
    }
  } 

}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          Serial.println(F("All good..."));
          break;
      }
      break;
    default:
      Serial.println(F("All good..."));
      break;
  }
}

  void checkPlayState() {
      int playState = digitalRead(busyPin);
      Serial.print("Busy Pin: "); 
      Serial.println("busyPin");
      if (playState == 1) {
        delay(200);
      }
    }

void printStatus() {
      Serial.print("Always On: ");
      Serial.println(digitalRead(onPin)); 
      Serial.print("Control Signal: ");
      Serial.println(digitalRead(controlPin)); 
      Serial.print("Volume (0-30): ");
      Serial.println(volumeOut); 
      Serial.print("Busy Pin (>512 when done): ");
      Serial.println(analogRead(busyPin)); 
}

void setVolume() {
  volumeVal = analogRead(volumePin);
  volumeOut = map(volumeVal, 0, 1023, 0, 30);
  myDFPlayer.volume(volumeOut);  //Set volume value. From 0 to 30
  delay(400);
  Serial.print("Volume: ");
  Serial.println(volumeVal);
  Serial.print("Volume map: ");
  Serial.println(volumeOut);
}


