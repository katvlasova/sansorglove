
/*
  
  This code uses pieces of example code made by Sandeep Mistry(@sandeepmistry)
*/


int sensorPin2 = A0;    // select the input pin for the potentiometer 1
int sensorPin3 = A4;    // select the input pin for the potentiometer 2
int sensorPin4 = A6;    // select the input pin for the potentiometer 3

int sensorValue = 0;  // variable to store the value coming from the sensor 1
int sensorValue2 = 0;  // variable to store the value coming from the sensor 2 
int sensorValue3 = 0;  // variable to store the value coming from the sensor 3
bool sensorBool = false;
bool sensorBool2 = false;
bool sensorBool3 = false;

#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h> // change to Arduino_LSM6DS3.h for Nano 33 IoT or Uno WiFi Rev 2

BLEService        customService                 ("2A5A20B9-0000-4B9C-9C69-4975713E0FF2");
BLECharacteristic accelerationCharacteristic ("2A5A20B9-0001-4B9C-9C69-4975713E0FF2", BLENotify, sizeof(float) * 3);
BLECharacteristic gyroscopeCharacteristic    ("2A5A20B9-0002-4B9C-9C69-4975713E0FF2", BLENotify, sizeof(float) * 3);
BLECharacteristic firstCharacteristic    ("2A5A20B9-0004-4B9C-9C69-4975713E0FF2", BLERead | BLENotify, '1' );
BLECharacteristic secondCharacteristic    ("2A5A20B9-0005-4B9C-9C69-4975713E0FF2", BLERead | BLENotify, '1' );
BLECharacteristic thirdCharacteristic    ("2A5A20B9-0006-4B9C-9C69-4975713E0FF2", BLERead | BLENotify, '1' );

void setup() {
  Serial.begin(9600);
  //set my input pins
  pinMode(sensorPin2, INPUT); 
  pinMode(sensorPin3, INPUT); 
  pinMode(sensorPin4, INPUT);  

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  BLE.setLocalName("KatArduino");
  BLE.setDeviceName("KatArduino");
  BLE.setAdvertisedService(customService);

  customService.addCharacteristic(accelerationCharacteristic);
  customService.addCharacteristic(gyroscopeCharacteristic);
  customService.addCharacteristic(firstCharacteristic);
  customService.addCharacteristic(secondCharacteristic);
  customService.addCharacteristic(thirdCharacteristic);
  
  BLE.addService(customService);

  //set initial value
  firstCharacteristic.writeValue((byte)0x00);
  secondCharacteristic.writeValue((byte)0x00);
  thirdCharacteristic.writeValue((byte)0x00);
  

  BLE.advertise();
}

void loop() {
  // poll for BLE events
  BLE.poll();
  //analog sensors
  // read the value from the sensor:
  //first
  sensorValue = analogRead(sensorPin2);
  if (sensorValue >= 840){
    sensorBool = true;
  }
  else if (sensorValue < 840){
    sensorBool = false;
  }
  //second
  sensorValue2= analogRead(sensorPin3);
  if (sensorValue2 >= 960){
    sensorBool2 = true;
  }
  else if (sensorValue2 <960){
    sensorBool2 = false;
  }
  //third
  sensorValue3 = analogRead(sensorPin4);
  if (sensorValue3 <= 700){
    sensorBool3 = true;
  }
  else if (sensorValue3 > 720){
    sensorBool3 = false;
  }
  Serial.print("First:");
  Serial.print(sensorValue);
  Serial.print("\t Second:");
  Serial.print(sensorValue2);
  Serial.print("\t Third:");
  Serial.println(sensorValue3);

  
  if (BLE.connected()) {
    if (accelerationCharacteristic.subscribed() && IMU.accelerationAvailable()) {
      float acceleration[3];

      // x, y, z
      IMU.readAcceleration(acceleration[0], acceleration[1], acceleration[2]);

      accelerationCharacteristic.writeValue(acceleration, sizeof(acceleration));
    }

    if (gyroscopeCharacteristic.subscribed() && IMU.gyroscopeAvailable()) {
      float gyroscope[3];

      // x, y, z
      IMU.readGyroscope(gyroscope[0], gyroscope[1], gyroscope[2]);

      gyroscopeCharacteristic.writeValue(gyroscope, sizeof(gyroscope));
    }

    //--------------------------------------------------------------------
    //Writing T or F based on finger presses
    //first writing
    if (sensorBool == true){
      //change
      firstCharacteristic.writeValue((byte)0x01);
    }
    else if (sensorBool == false){
      //no change
      firstCharacteristic.writeValue((byte)0x00);
    }
    
    //second writing
    if (sensorBool2 == true){
      //change
      secondCharacteristic.writeValue((byte)0x01);
    }
    else if (sensorBool2 == false){
      //no change
      secondCharacteristic.writeValue((byte)0x00);
    }
    //third writing
    if (sensorBool3 == true){
      //change
      thirdCharacteristic.writeValue((byte)0x01);
    }
    else if (sensorBool3 == false){
      //no change
      thirdCharacteristic.writeValue((byte)0x00);
    }
    //--------------------------------------------------
  }
}
