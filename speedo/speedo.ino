#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("Kel's Speedometer");

  Wire.begin();

  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
}

float distance = 0;

float odo = 0;

int pollsPerSecond = 4;

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > (1000 / pollsPerSecond))
  {
    lastTime = millis(); // Update the timer
    
    long speed = random(min, max)
    float speedMPH = (speed * 0.00223694);
    // distance is equal to the old distance plus the new speed / (polls per second * seconds in an hour)
    distance = distance + (speedMPH / (pollsPerSecond * (60 * 60))); 
    
    Serial.print(F(" Speed: "));
    Serial.print(speedMPH);
    Serial.print(F(" (mph)"));
    Serial.print(F(" Distance: "));
    Serial.print(distance, 6);
    Serial.print(F(" Miles"));
    // every 10th of a mile
    if (distance >= (odo + .1)) {
      odo = distance;    
      Serial.print(F(" | Odometer: "));
      Serial.print(odo, 1);
      // TODO: write odo to memory
      // This will have the disadvantage of losing up to .09 when you shut the car off
    }

    Serial.println();
  }
}
