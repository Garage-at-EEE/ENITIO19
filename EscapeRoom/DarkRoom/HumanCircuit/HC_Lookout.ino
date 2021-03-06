//Human Circuit V0.1
//Created by Yen Hau

//This is a sketch for a particular painting link to the skeleton
//When it's touched, it will light up like other painting and also send a signal to the skeleton

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
int actS3 = 0;

char msg[3] = {'1', '2', '3'};
char msgNo[3] = {'a', 'b', 'c'};
char comm = 0;

int actedS3 = 0;
int deactedS3 = 0;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);

  pixels.begin();

  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(100); 
  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

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
    //Turn off the neopixels
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }

    pixels.show();

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
  }
} // Loop
