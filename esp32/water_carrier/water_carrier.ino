// SPDX-FileCopyrightText: 2023 Matej Vargovcik <matej.vargovcik@gmail.com>
// SPDX-License-Identifier: MIT

// Based on ESP32 BLE Arduino/BLE_uart example which is
// - Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
// - Ported to Arduino ESP32 by Evandro Copercini

#include "BluetoothSerial.h"
#include "dc_motor.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
#endif

BluetoothSerial SerialBT;

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

bool readingRight = false;
String readStringLeft;
String readStringRight;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        bool lineComplete = false;
        for (int i = 0; i < rxValue.length(); i++) {
          char c = rxValue[i];
          Serial.print(c);
          if (c == '\n') {
            lineComplete = true;
            readingRight = false;
            break;
          }
          else if (c == ' ')
            readingRight = true;
          else if (readingRight)
            readStringRight += c;
          else
            readStringLeft += c;
        }
        Serial.println();
        if (lineComplete) {
          double leftSpeed, rightSpeed;
          leftSpeed = readStringLeft.toDouble();
          rightSpeed = readStringRight.toDouble();

          // clamp to <-1..1> interval, keep ratio
          double maxSpeed = max(abs(leftSpeed), abs(rightSpeed));
          if (maxSpeed > 1.0) {
            leftSpeed /= maxSpeed;
            rightSpeed /= maxSpeed;
          }
          
          DCMotor::leftMotor.setSpeed(leftSpeed);
          DCMotor::rightMotor.setSpeed(-rightSpeed);
      
          Serial.print("Left: ");
          Serial.print(leftSpeed);
          Serial.print(", Right: ");
          Serial.println(rightSpeed);
      
          readStringLeft = "";
          readStringRight = "";
        }
        Serial.println("*********");
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting!");

  // Create the BLE Device

  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
}

void loop() {
  // Send data
//  if (deviceConnected) {
//    pTxCharacteristic->setValue(&txValue, 1);
//    pTxCharacteristic->notify();
//    txValue++;
//    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
//  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    Serial.println("Device connected");
  }
}

////////////////////////////// Stepper motor code //////////////////////////////////////
//// Include the AccelStepper Library
//#include <AccelStepper.h>
//
//// Define step constant
//#define MotorInterfaceType 4
//
//// Creates an instance
//// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
//AccelStepper myStepper(MotorInterfaceType, 12, 15, 13, 14);
//
//void setup() {
//  Serial.begin(115200);
//  Serial.println("Hello");
//  // set the maximum speed, acceleration factor,
//  // initial speed and the target position
//  myStepper.setMaxSpeed(700.0);
//  myStepper.setAcceleration(1000.0);
//  myStepper.setSpeed(700);
//  myStepper.moveTo(2038);
//}
//
//void loop() {
//  // Change direction once the motor reaches target position
//  if (myStepper.distanceToGo() == 0) {
//    myStepper.moveTo(2038-myStepper.currentPosition());
//    Serial.println(myStepper.currentPosition());
//  }
////  Serial.println("dist:");
////  Serial.println(myStepper.distanceToGo());
////  Serial.println("cur:");
////  Serial.println(myStepper.currentPosition());
//
//  // Move the motor one step
////  delay(2000);
//  myStepper.run();
//}
