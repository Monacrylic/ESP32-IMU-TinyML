

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LSM6DS3TRC.h>
#include "../connections/hardware.h"

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 300;

int samplesRead = numSamples;
Adafruit_LSM6DS3TRC lsm6ds; 
Adafruit_Sensor *lsm_temp, *lsm_accel, *lsm_gyro;

sensors_event_t accel;
sensors_event_t gyro;

void setup()
{
  Serial.begin(115200);
  pinMode(ONBOARD_LED, OUTPUT);
  Wire.begin(4,5); // SDA connected to pin 5, SCL connected to pin 4
  if (!lsm6ds.begin_I2C(0x6A, &Wire))
  {
    Serial.println("Failed to find LSM6DS chip");
    while (1)
    {
      delay(10);
    }
  }
  lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_8_G);
  lsm6ds.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);

  lsm6ds.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
  lsm6ds.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
  //Serial.println("LSM6DS Found!");

  // lsm_accel = lsm6ds.getAccelerometerSensor();
  // lsm_accel->printSensorDetails();

  // lsm_gyro = lsm6ds.getGyroSensor();
  // lsm_gyro->printSensorDetails();
  Serial.println("aX,aY,aZ,gX,gY,gZ");
}

void loop()
{
  float aX, aY, aZ, gX, gY, gZ;

  // wait for significant motion
  while (samplesRead == numSamples)
  {
    if (lsm6ds.accelerationAvailable())
    {
      // read the acceleration data
      lsm6ds.readAcceleration(aX, aY, aZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold)
      {
        // reset the sample read count
        samplesRead = 0;
        digitalWrite(ONBOARD_LED, HIGH);
        break;
      }
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples)
  {
    // check if both new acceleration and gyroscope data is
    // available
    if (lsm6ds.accelerationAvailable() && lsm6ds.gyroscopeAvailable())
    {
      // read the acceleration and gyroscope data
      lsm6ds.readAcceleration(aX, aY, aZ);
      lsm6ds.readGyroscope(gX, gY, gZ);

      samplesRead++;

      // print the data in CSV format
      Serial.print(aX, 3);
      Serial.print(',');
      Serial.print(aY, 3);
      Serial.print(',');
      Serial.print(aZ, 3);
      Serial.print(',');
      Serial.print(gX, 3);
      Serial.print(',');
      Serial.print(gY, 3);
      Serial.print(',');
      Serial.print(gZ, 3);
      Serial.println();

      if (samplesRead == numSamples)
      {
        // add an empty line if it's the last sample
        Serial.println();
        digitalWrite(ONBOARD_LED, LOW);
      }
    }
  }
}
