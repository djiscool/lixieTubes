/*
  Lixie Tube Clock

  Based on the following libraries:

  DS3231_Serial_Easy
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  web: http://www.RinkyDinkElectronics.com/

  Arduino Uno/2009/Nano:
  ----------------------
  DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
  The internal pull-up resistors will be activated when using the
  hardware I2C interfaces.

  You can connect the DS3231 to any available pin but if you use any
  other than what is described above the library will fall back to
  a software-based, TWI-like protocol which will require exclusive access
  to the pins used, and you will also have to use appropriate, external
  pull-up resistors on the data and clock signals.


  Arduino Protothreading Example v1.1
  by Drew Alden (@ReanimationXP) 1/12/2016

  - Update: v1.1 - 8/18/17
  Arduino 1.6.6+ prototyping changed, small fixes.
  (create functions ahead of use, removed foreach and related library).

  Note that TimedAction is now out of date. Be sure to read notes about
  TimedAction and WProgram.h / Arduino.h errors.


  COMPONENTS


  This code was made using the Sunfounder Arduino starter kit's blue LCD.
  It can be found at Amazon.com in a variety of kits.


  THIRD-PARTY LIBRARIES
  these must be manually added to your Arduino IDE installation

  TimedAction
  allows us to set actions to perform on separate timed intervals
  http://playground.arduino.cc/Code/TimedAction
  http://wiring.uniandes.edu.co/source/trunk/wiring/firmware/libraries/TimedAction
*/

#include <TimedAction.h>
//NOTE: This library has an issue on newer versions of Arduino. After
//      downloading the library you MUST go into the library directory and
//      edit TimedAction.h. Within, overwrite WProgram.h with Arduino.h

#include "DS3231.h"
#include "Lixie.h" // Include Lixie Library
#include <util/atomic.h> // this library includes the ATOMIC_BLOCK macro.

#define DATA_PIN   13
#define NUM_LIXIES 2

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

Lixie lix(DATA_PIN, NUM_LIXIES); // Set class name to "lix" and define your Lixie setup

volatile String curTime = "0"; // Use this number for setting time for lixie

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int hrButton = 3;
const int minButton = 4;

// variables will change:
volatile int buttonState;             // the current reading from the input pin
bool clockStatus;

Time t;



//FUNCTIONS

// this our function for updating the lixie tube. will be called as a thread.
// this may be overkill using a thread to update the lixie tubes, but it works.
void updateLixie() {
  Serial.print("curTime: ");//debug statement
  Serial.println(curTime.c_str());//debug statement
  char temp[3];
  curTime.toCharArray(temp, 3);
  Serial.print("temp: "); //debug statement
  Serial.println(temp); //debug statement
  //Serial.println(curTime.toInt());

  lix.write(temp);             // Write the curTime to the displays
  // lix.write(int input OR char* input);
  // int: Clears the displays, and pushes in a multi-digit integer as in: 2016
  // char*: Clears the displays, and pushes in a char array as in: "2016".
  // This ignores any non-numeric chars in the string, allowing you to send "12:52:47 PM" and have the displays show "12 52 47" for a clock.

  //delay(1000);                  // this will be handled by the timer instead
}
int getHrs()
{
  t = rtc.getTime();
  // variable t
  return t.hour; // Returns the hour for the given
  // time t
}

int getMins()
{
  t = rtc.getTime();
  // variable t
  return t.min; // Returns the minute for the given
  // time t
}

int getSecs()
{
  t = rtc.getTime();
  // variable t
  return t.sec; // Returns the second for the given
  // time t
}



//Task for updating time. This will be blocking (wait for user input)
void updateTime() {
  clockStatus = true;
  if (checkModeButton()) // if button is still pressed
  {
    delay(500);
    checkModeButton();
    updateTime();
    return;
  }
  int minsToUpdate = getMins();
  int hrsToUpdate = getHrs();
  int secsToUpdate = 0;
  while (clockStatus)
  {
    //Serial.println("in Update Time");
    //const int hrButton = 3; const int minButton = 4;
    // assuming that we will not be pressing two buttons at the same time
    if (checkButton(hrButton))
    {
      hrsToUpdate += 1;
    }
    else if (checkButton(minButton))
    {
      minsToUpdate += 1;
    }

    delay(100);
    if (checkModeButton()) // check if exit update Time button pressed
    {
      clockStatus = false;
    }
  }
  while (checkModeButton()) // We don't want to create a false positive and end up back in updateTime, so wait until button is no longer pressed.
  {
    delay(100);
  }
  //Serial.println("leaving Update Time"); //debug statement
  // update time on rtc module
  rtc.setTime(hrsToUpdate, minsToUpdate, secsToUpdate);
  
}

bool checkModeButton() {
  // read the state of the pushbutton value:
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // code with interrupts blocked (consecutive atomic operations will not get interrupted)
    buttonState = digitalRead(buttonPin);
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    //Serial.println("Not Pressed"); //debug statement
    return false;
  } else {
    // turn LED off:
    //Serial.println("Pressed"); //debug statement
    return true;
  }

}

bool checkHrButton() {
  // read the state of the pushbutton value:
  int btnStatus = false;
  btnStatus = digitalRead(hrButton);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (btnStatus == HIGH) {
    // turn LED on:
    //Serial.println("Not Pressed"); //debug statement
    return false;
  } else {
    // turn LED off:
    //Serial.println("Pressed"); //debug statement
    return true;
  }
}

bool checkMinButton() {
  // read the state of the pushbutton value:
  int btnStatus = false;
  btnStatus = digitalRead(minButton);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (btnStatus == HIGH) {
    // turn LED on:
    //Serial.println("Not Pressed"); //debug statement
    return false;
  } else {
    // turn LED off:
    //Serial.println("Pressed"); //debug statement
    return true;
  }
}
// function to replace above button check functions
// checks the button passed to it
bool checkButton(int btnNumber) {
  // read the state of the pushbutton value:
  int btnStatus = false;
  btnStatus = digitalRead(btnNumber);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (btnStatus == HIGH) {
    // turn LED on:
    //Serial.println("Not Pressed"); //debug statement
    return false;
  } else {
    // turn LED off:
    //Serial.println("Pressed"); //debug statement
    return true;
  }
}

//Create a timer that will fire repeatedly every x ms

//edit: this has to be called after the function is defined.
TimedAction lixieThread = TimedAction(500, updateLixie); // updated every 1 second(s)



void setup()
{
  // Setup Serial connection
  Serial.begin(115200);

  // Initialize the rtc object
  rtc.begin();

  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014


  lix.begin();                   // Initialize LEDs
  lix.nixie_mode(true);          // Formats the Lixies to appear like nixies
  lix.brightness(128);          // sets overall brightness of lixe tubes
  //Maybe dim based on time of day later.

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(hrButton, INPUT_PULLUP);
  pinMode(minButton, INPUT_PULLUP);

  //initialize our mode to clock (true for program clock)
  bool clockStatus = false;
}


void loop()
{

  // Check for Button Press (Check if mode button pressed)
  // Maybe later add functionality for long press vs short press.
  // long press for change time, and short press to change clock modes
  // (i.e. doomsday clock, regular clock, etc.)
  if (checkModeButton())
  {
    updateTime();
  }

  //check on our thread. based on how long the system has been
  //running, do they need to fire and do work? if so, do it!
  lixieThread.check();

  // Get Clock Time

  // Serial Print used for debugging purposes
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");

  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  String got_time = rtc.getTimeStr();
  //Serial.println(got_time);
  //Serial.println(got_time.substring(got_time.length()-2,got_time.length()).c_str());

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    // code with interrupts blocked (consecutive atomic operations will not get interrupted)
    curTime = got_time.substring(got_time.length() - 2, got_time.length());
  }
  //updateLixie();

  // check if Update Lixie Tubes (via thread)is needed
  lixieThread.check();

  // We need to burn some time, but still want a responsive button
  for (int i = 0; i < 100; i++)
  {
    if (checkModeButton())
    {
      updateTime();
      break;
    }
    delay (10);
  }

}
