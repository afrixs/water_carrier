// SPDX-FileCopyrightText: 2023 Matej Vargovcik <matej.vargovcik@gmail.com>
// SPDX-License-Identifier: MIT

#include "Arduino.h"

class DCMotor {
  byte tag;
  byte pwm_pin, dir_pin;
  byte slow, fast, boost, boost_duration;

public:
  DCMotor(byte tag, byte ia_pin, byte ib_pin, byte slow, byte fast, byte boost, byte boost_duration_ms);

  static DCMotor leftMotor;
  static DCMotor rightMotor;

  void setSpeed(double value);
};
