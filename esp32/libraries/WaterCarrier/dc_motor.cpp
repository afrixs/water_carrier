// SPDX-FileCopyrightText: 2023 Matej Vargovcik <matej.vargovcik@gmail.com>
// SPDX-License-Identifier: MIT

#include "dc_motor.h"

DCMotor DCMotor::leftMotor = DCMotor(0, 27, 5, 65, 255, 71, 100);
DCMotor DCMotor::rightMotor = DCMotor(1, 17, 16, 65, 255, 71, 100);

DCMotor::DCMotor(byte tag, byte ia_pin, byte ib_pin, byte slow, byte fast, byte boost, byte boost_duration_ms) :
  tag(tag), pwm_pin(ia_pin), dir_pin(ib_pin), slow(slow), fast(fast), boost(boost), boost_duration(boost_duration_ms)
{
  pinMode( dir_pin, OUTPUT );
  pinMode( pwm_pin, OUTPUT );
  digitalWrite( dir_pin, LOW );
  analogWrite( pwm_pin, 0 );
}

void DCMotor::setSpeed(double value)
{
  byte pwm, dir;
  pwm = value == 0.0 ? 0 : slow + round(abs(value)*(fast - slow));
  if (value <= 0.0)
    dir = LOW;
  else {
    dir = HIGH;
    pwm = 255 - pwm;
  }

  digitalWrite(dir_pin, dir);
  analogWrite(pwm_pin, pwm);

  Serial.print(tag);
  Serial.print(" ");
  Serial.print(dir);
  Serial.print(" ");
  Serial.print(pwm);
  Serial.println();
}