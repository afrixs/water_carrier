// SPDX-FileCopyrightText: 2023 Matej Vargovcik <matej.vargovcik@gmail.com>
// SPDX-License-Identifier: MIT

// Arduino IDE setup for control with gamepad: https://gitlab.com/ricardoquesada/bluepad32/-/blob/main/docs/plat_arduino.md
// Based on Bluepad32/Controller example with
//  - Copyright 2021 - 2023, Ricardo Quesada
//  - SPDX-License-Identifier: Apache 2.0 or LGPL-2.1-or-later

#include <Bluepad32.h>
#include "dc_motor.h"

ControllerPtr gamepads[BP32_MAX_CONTROLLERS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (gamepads[i] == nullptr) {
      Serial.print("CALLBACK: Controller is connected, index=");
      Serial.println(i);
      gamepads[i] = ctl;
      foundEmptySlot = true;

      // Optional, once the gamepad is connected, request further info about the
      // gamepad.
      ControllerProperties properties = ctl->getProperties();
      char buf[80];
      sprintf(buf,
              "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
              "flags: 0x%02x",
              properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
              properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
              properties.vendor_id, properties.product_id, properties.flags);
      Serial.println(buf);
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println(
        "CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundGamepad = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (gamepads[i] == ctl) {
      Serial.print("CALLBACK: Controller is disconnected from index=");
      Serial.println(i);
      gamepads[i] = nullptr;
      foundGamepad = true;
      break;
    }
  }

  if (!foundGamepad) {
    Serial.println(
        "CALLBACK: Controller disconnected, but not found in gamepads");
  }
}

void processGamepad(ControllerPtr gamepad) {
  double linear_vel_x = -gamepad->axisY()/512.0;
  double angular_vel_z = -gamepad->axisX()/512.0;
  double leftSpeed = linear_vel_x - angular_vel_z;
  double rightSpeed = linear_vel_x + angular_vel_z;

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
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting!");

  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);

  // To get the BD Address (MAC address) call:
  const uint8_t* addr = BP32.localBdAddress();
  Serial.print("BD Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(addr[i], HEX);
    if (i < 5)
      Serial.print(":");
    else
      Serial.println();
  }

  // BP32.pinMode(27, OUTPUT);
  // BP32.digitalWrite(27, 0);

  // This call is mandatory. It setups Bluepad32 and creates the callbacks.
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();
}

void loop() {
  BP32.update();

  // It is safe to always do this before using the controller API.
  // This guarantees that the controller is valid and connected.
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    ControllerPtr gamepad = gamepads[i];

    if (gamepad && gamepad->isConnected()) {
      if (gamepad->isGamepad())
        processGamepad(gamepad);
    }
  }
  delay(50);
}