#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

#include <Adafruit_GFX.h>  // Include core graphics library for the display.
#include <Adafruit_SSD1306.h>  // Include Adafruit_SSD1306 library to drive the display.

//----Define OLED Display Settings------
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
//-----End OLED Display Settings--------

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("Kel's Speedometer");

  Wire.begin();


  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(20,10);
  display.println("CAROL"); 
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
}

float distance = 0;

float odo = 0;

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > (1000))
  {
    lastTime = millis(); // Update the timer
    
    long speed = myGPS.getGroundSpeed();
    float speedMPH = (speed * 0.00223694);
    // distance is equal to the old distance plus the new speed / (polls per second * seconds in an hour)
    distance = speedMPH >= 0.5 ? distance + (speedMPH / (60 * 60)) : distance;

    //TEMP TODO: write speedMPH to the LCD

    //TODO: map the speedMPH to the stepper position
    
    Serial.print(F(" Speed: "));
    Serial.print(speedMPH);
    Serial.print(F(" (mph)"));
    Serial.print(F(" Distance: "));
    Serial.print(distance, 6);
    Serial.print(F(" Miles"));
    // every 10th of a mile we change/write the odometer
    if (distance >= (odo + .1)) {
      odo = distance;    
      Serial.print(F(" | Odometer: "));
      Serial.print(odo, 1);

      // TODO: write odo to memory

      // TODO: write odo to LCD
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.clearDisplay();
        
        display.setCursor(0,10);
        display.println(odo, 1);


      // NOTE: This will have the disadvantage of losing up to .09 when you shut the car off
    }

    Serial.println();
  }
}
