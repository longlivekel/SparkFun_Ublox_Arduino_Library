#include <Wire.h> //Needed for I2C to GPS


long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.

void setup()
{
  Serial.begin(115200);
  while (!Serial); //Wait for user to open terminal
  Serial.println("Kel's Speedometer");
}

float distance = 0;

float odo = 0;

void loop()
{
  //Query module only every second. Doing it more often will just cause I2C traffic.
  //The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); //Update the timer
    
    long speed = random(0, 22);
    //float speedMPH = (speed * 0.00223694);
    float speedMPH = 60;
    distance = distance + (speedMPH / 3600); // distance is equal to the old distance plus the new speed/seconds in an hour
    
    //if (distance >= (odo + .1)) { // should happen every 10th of a mile
     // odo = distance;      
    //}
    
    Serial.print(F(" Speed: "));
    Serial.print(speedMPH);
    Serial.print(F(" (mph)"));
    Serial.print(F(" Distance: "));
    Serial.print(distance, 6);
    Serial.print(F(" Miles "));
    if (distance >= (odo + .1)) {
      odo = distance;    
      Serial.print(F(" | Odometer: "));
      Serial.print(odo, 1);
    }
    // if distance > previous odo + .01
    
    


    Serial.println();
  }
}
