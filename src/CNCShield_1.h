// CNCShield: the Arduino CNC Shield V3 with X, Y and Z stepper drivers.
//
// A ready-made object called "stepper" is created for you, so you never
// need to build one yourself:
//
//   stepper.begin();            // in setup()
//   stepper.X.moveUp();         // control one axis
//   stepper.update();           // in loop(), steps every axis
//
// Pins (CNC Shield V3):
//   X: step 2, direction 5, limit switch 9
//   Y: step 3, direction 6, limit switch 10
//   Z: step 4, direction 7, limit switch 11
//   All drivers share one enable pin: 8

#pragma once

#include <Arduino.h>
#include "StepperAxis.h"

class CNCShield {
public:
  CNCShield();

  // The three motors.
  StepperAxis X;
  StepperAxis Y;
  StepperAxis Z;

  // Sets up all pins and turns the drivers on. Call once in setup().
  void begin();

  // Steps every axis if a step is due. Call it as often as possible in loop().
  void update();

  // Turns all motor drivers on. The motors hold their position.
  void enable();

  // Turns all motor drivers off. The motors can be turned by hand.
  void disable();

  // Stops every axis. The drivers stay on.
  void stopAll();

  // Returns true if any axis is moving.
  bool isAnyMoving() const;

private:
  uint8_t _enablePin;
};

// The ready-made CNC shield object.
extern CNCShield stepper;
