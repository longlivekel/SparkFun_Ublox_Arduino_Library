#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
SFE_UBLOX_GPS myGPS;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to Ublox module.
float distance = 0;
float odo = 0;
const double StepsPerDegree = 2.0;         // Motor step is 1/3 of a degree of rotation
const unsigned int MaxMotorRotation = 315; // 315 max degrees of movement
const unsigned int MaxMotorSteps = MaxMotorRotation * StepsPerDegree;
unsigned int motorStep = 0;
double SpeedoDegreesPerMPH = 155.0 / 120.0;

//----Define OLED Display Settings------  NOTE.. can do the same without this.. Relocated OLED_Reset
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
//-----End OLED Display Settings--------

//----Define Stepper motor library variables and pin outs-------------------------------------------
SwitecX25 Motor(MaxMotorSteps, 4, 5, 6, 7); // Create the motor object with the maximum steps allowed

void setup()
{

  Serial.begin(115200);

  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println("Kel's Speedometer");
  Wire.begin();

  if (myGPS.begin(Wire, 0x3F) == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  // I2C OLED display
  display.begin(SSD1306_SWITCHCAPVCC); // initialize with the I2C addr 0x3C (128x32)
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(20, 10);
  display.println("CAROL");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(20, 10);
  display.println(odo, 1);
  display.display();

  Motor.zero(); //Initialize stepper at 0 location
  Motor.setPosition(470);
  Motor.updateBlocking();
  delay(100);
  Motor.setPosition(0); //0MPH
  Motor.updateBlocking();
  delay(100);
}

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

    //map the speedMPH to the stepper position
    //motorStep = MphToStep(speedMPH);
    motorStep = MphToStep(60);
    Motor.setPosition(motorStep);

    Serial.print(F(" Speed: "));
    Serial.print(speedMPH);
    Serial.print(F(" (mph)"));
    Serial.print(F(" Distance: "));
    Serial.print(distance, 6);
    Serial.print(F(" Miles"));
    // every 10th of a mile we change/write the odometer
    if (distance >= (odo + .1))
    {
      odo = distance;
      Serial.print(F(" | Odometer: "));
      Serial.print(odo, 1);

      // TODO: write odo to memory

      // TODO: write odo to LCD

      //display.setTextSize(3);
      //display.setTextColor(WHITE);
      display.clearDisplay();
      display.setCursor(0, 10); // WTF??
      display.println(odo, 1);
      display.display();

      // NOTE: This will have the disadvantage of losing up to .09 when you shut the car off
    }

    Serial.println();
  }
}

unsigned int MphToStep(float mph)
{
  // convert mph to degrees on the speedo
  return (unsigned int)(SpeedoDegreesPerMPH * mph);
}
