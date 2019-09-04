//Human Circuit V0.1
//Created by Yen Hau

////////////////////////Role Declaration Start////////////////////////

//role: 0(Master),1(Slave_1),2(Slave_2),3(Slave_3)
int role = 3;

////////////////////////Role Declaration Ended////////////////////////

////////////////////////RF24 Declaration Start////////////////////////
#include <SPI.h>
#include "RF24.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> 
#endif

#define PIN            6

#define NUMPIXELS      3

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

RF24 radio(7, 8); //CE,CSN

// The unique address is a 24/32/40 bit, it can be string literal,binary or hexadecimal
byte addresses[][6] = {"hcm", "hcs", "hcl"}; //Master is hcm, Slave is hcs

////////////////////////RF24 Declaration Ended////////////////////////

int buttonPin = 10;

//activation
int actM = 0;
int actS1 = 0;
int actS2 = 0;
int actS3 = 0;

char msg[3] = {'1', '2', '3'};
char msgNo[3] = {'a', 'b', 'c'};
char msgLook[2] = {'p', 'q'};
char comm = 0;

int actedM = 0;
int actedS1 = 0;
int actedS2 = 0;
int actedS3 = 0;

int deactedM = 0;
int deactedS1 = 0;
int deactedS2 = 0;
int deactedS3 = 0;

int lookout = 0;

void setup() {

  pinMode(buttonPin,INPUT_PULLUP);
  
  pixels.begin();

  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(100); //110 //100 is dark room 2
  radio.setDataRate(RF24_250KBPS);

  if (role == 0) //This is master
  {
    radio.openReadingPipe(1, addresses[0]); //read at hcm address for slave input at pipe 1
    radio.openWritingPipe(addresses[1]); //write to hcs address to reset slaves
  } else //This is slave
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }

  radio.startListening();

  Serial.println("Ready");
}

void loop() {

  if (digitalRead(buttonPin) == 0)
  {
    
    
    //The painting is activated
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(200, 200, 200));
    }
    pixels.show();

    if (role == 3)
    {
      actS3 = 1;
      if (actS3 != actedS3)
      {
        Serial.println("Neopixel Activated");
        radio.stopListening();
        //radio.openWritingPipe(addresses[2]);
        radio.write(&msg[role - 1], sizeof(char));
        radio.startListening();
        //radio.openWritingPipe(addresses[0]);
        Serial.print("Data sent: ");
        Serial.println(msg[role - 1]);
        actedS3 = 1;
        deactedS3 = 0;
      }
    } else
    {
      //This is Slave
      //Send msg to master

      radio.stopListening();
      radio.write(&msg[role - 1], sizeof(char));
      radio.startListening();

      Serial.print("Data sent: ");
      Serial.println(msg[role - 1]);
      //while(1);
    }
  } else
  {
    
    
    //Turn off the neopixels
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }

    pixels.show();

    if (role == 0)
    {
      //This is master
      actM = 0;
      actedM = 0;

      if (actM == !deactedM) //first time
      {
        //nothing
      } else
      {
        deactedM = 1;
        Serial.println("Master deactivated");
      }
    } else if (role == 3)
    {
      actS3 = 0;
      if (actS3 == deactedS3)
      {
        Serial.println("Neopixel Deactivated");
        radio.stopListening();
        //radio.openWritingPipe(addresses[2]);
        radio.write(&msgNo[role - 1], sizeof(char));
        radio.startListening();
        //radio.openWritingPipe(addresses[0]);
        Serial.print("Data sent: ");
        Serial.println(msgNo[role - 1]);
        actedS3 = 0;
        deactedS3 = 1;
      }
    } else
    {
      //This is Slave
      //Send msg to master

      radio.stopListening();
      radio.write(&msgNo[role - 1], sizeof(char));
      radio.startListening();

      if (role == 3)
      {

      }
    }
  }

  if (radio.available())
  {
    while (radio.available())
    {
      radio.read(&comm, sizeof(char));
    }
  }

  if (comm == '1')
  {
    actS1 = 1;
    deactedS1 = 0;

    if (actS1 == actedS1)
    {
      //nothing
    } else //first time
    {
      actedS1 = 1;
      Serial.println("S1 activated");
    }

    //delay(1000);
  } else if (comm == '2')
  {
    actS2 = 1;
    deactedS2 = 0;

    if (actS2 == actedS2)
    {
      //nothing
    } else
    {
      actedS2 = 1;
      Serial.println("S2 activated");
    }
  } else if (comm == '3')
  {
    actS3 = 1;
    deactedS3 = 0;

    if (actS3 == actedS3)
    {
      //nothing
    } else
    {
      actedS3 = 1;
      Serial.println("S3 activated");
    }
  } else if (comm == 'a')
  {
    actS1 = 0;
    actedS1 = 0;

    if (actS1 == !deactedS1)
    {
      //nothing
    } else //first time
    {
      deactedS1 = 1;
      Serial.println("S1 deactivated");
    }

    //delay(1000);
  } else if (comm == 'b')
  {
    actS2 = 0;
    actedS2 = 0;

    if (actS2 == !deactedS2)
    {
      //nothing
    } else
    {
      deactedS2 = 1;
      Serial.println("S2 deactivated");
    }
  } else if (comm == 'c')
  {
    actS3 = 0;
    actedS3 = 0;

    if (actS3 == !deactedS3)
    {
      //nothing
    } else
    {
      deactedS3 = 1;
      Serial.println("S3 deactivated");
    }
  } else if (comm == 'z')
  {
    lookout = 1;
    Serial.println("Lookout activated");
  } else if (comm == 'y')
  {
    lookout = 0;
    Serial.println("Lookout deactivated");
  }

  comm = 0;
} // Loop
