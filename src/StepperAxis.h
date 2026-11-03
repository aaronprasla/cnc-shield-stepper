// StepperAxis: controls one stepper motor on a CNC shield.
//
// You normally don't create this yourself. Use the ready-made axes
// stepper.X, stepper.Y and stepper.Z from CNCShield.h.
//
// "Up" is the positive direction (position increases).
// "Down" is the negative direction (position decreases).
//
// Movement is non-blocking. The move functions only set what the motor should
// do. The motor actually steps when update() runs, so call stepper.update()
// as often as possible in loop().

#pragma once

#include <Arduino.h>

class StepperAxis {
public:
  StepperAxis(uint8_t stepPin, uint8_t dirPin);

  // Sets up the step and direction pins. Called by stepper.begin().
  void begin();

  // Movement

  // Moves up until stop() is called.
  void moveUp();

  // Moves down until stop() is called.
  void moveDown();

  // Moves a number of steps from the current position.
  // Positive = up, negative = down. Example: moveSteps(-200)
  void moveSteps(long steps);

  // Moves to an absolute position. Example: moveTo(1000)
  void moveTo(long position);

  // Stops sending steps. The motor stays powered and holds its position.
  void stop();

  // Settings

  // Sets the speed in steps per second (limited to 1 - 4000).
  void setSpeed(long stepsPerSecond);

  // Returns the speed in steps per second.
  long getSpeed() const;

  // Flips the physical direction if the motor turns the wrong way.
  // "Up" still means a rising position.
  void reverseDirection(bool reversed);

  // Position tracking
  // The position counts steps sent to the driver. It starts at 0 and does not
  // know if the motor stalled or skipped steps.

  // Returns the current position in steps.
  long getPosition() const;

  // Sets the current position without moving. Use it to define a new origin.
  void setPosition(long position);

  // Sets the current position to 0 without moving.
  void resetPosition();

  // Returns how many steps are left in a moveSteps() or moveTo() move.
  // Returns 0 when idle or moving continuously.
  long stepsRemaining() const;

  // Returns true while the axis is moving.
  bool isMoving() const;

  // Takes a step if one is due. Called by stepper.update().
  // Call it directly only if you want to update a single axis.
  void update();

private:
  enum Mode : uint8_t { IDLE, CONTINUOUS, TARGET };

  void takeStep(bool up);

  uint8_t _stepPin;
  uint8_t _dirPin;

  Mode _mode;
  bool _continuousUp;       // direction used in CONTINUOUS mode
  bool _reversed;           // flips the physical direction pin
  long _position;           // current position in steps
  long _target;             // goal position in TARGET mode
  unsigned long _intervalUs;  // time between steps
  unsigned long _lastStepUs;  // time of the last step
};
