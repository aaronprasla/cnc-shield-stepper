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

// Which end of travel a limit switch is at.
enum LimitEnd : uint8_t { LIMIT_DOWN, LIMIT_UP };

// How a limit switch is wired. Both types connect the pin to GND.
// NORMALLY_OPEN: the circuit closes when the switch is pressed.
// NORMALLY_CLOSED: the circuit opens when pressed. A broken wire also counts
// as pressed, which makes this the safer choice.
enum SwitchType : uint8_t { NORMALLY_OPEN, NORMALLY_CLOSED };

class StepperAxis {
public:
  StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t limitPin);

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

  // Returns true while the axis is moving (including while homing).
  bool isMoving() const;

  // Limit switch
  // The switch protects one end of travel. While it is pressed, moving toward
  // it is blocked and moving away from it is allowed.

  // Turns on the limit switch at the given end. The switch connects the
  // axis' limit pin to GND (X: pin 9, Y: pin 10, Z: pin 11).
  // Example: stepper.X.attachLimit(LIMIT_DOWN);
  //          stepper.Z.attachLimit(LIMIT_UP, NORMALLY_CLOSED);
  void attachLimit(LimitEnd end, SwitchType type = NORMALLY_OPEN);

  // Turns the limit switch off.
  void detachLimit();

  // Returns true while the switch is pressed.
  bool isLimitPressed() const;

  // Returns true if the last move was stopped by the limit switch.
  // It is cleared when a new move starts.
  bool limitHit() const;

  // Homing
  // Homing moves toward the limit switch, sets the position to 0 where the
  // switch triggers, then backs away. Needs attachLimit() first.

  // Starts homing. Returns false (and does nothing) if no limit switch is
  // attached. Keep calling stepper.update() until isHoming() is false.
  bool home();

  // Returns true while homing is running.
  bool isHoming() const;

  // Returns true once the axis has been homed, so position 0 is the switch.
  bool isHomed() const;

  // Sets the speed used while homing, in steps per second (default 300).
  void setHomingSpeed(long stepsPerSecond);

  // Sets how many steps to back away from the switch after homing
  // (default 200). The axis ends up at that position, since 0 is the switch.
  void setHomeBackoff(long steps);

  // Takes a step if one is due. Called by stepper.update().
  // Call it directly only if you want to update a single axis.
  void update();

private:
  enum Mode : uint8_t { IDLE, CONTINUOUS, TARGET, HOMING_SEEK, HOMING_BACKOFF };

  void takeStep(bool up);
  bool limitPressed() const;
  void startBackoff();

  uint8_t _stepPin;
  uint8_t _dirPin;
  uint8_t _limitPin;

  Mode _mode;
  bool _continuousUp;          // direction used in CONTINUOUS mode
  bool _reversed;              // flips the physical direction pin

  bool _limitEnabled;          // true after attachLimit()
  bool _limitUp;               // true if the switch is at the up end
  bool _limitNormallyClosed;   // switch wiring type
  bool _limitHit;              // last move was stopped by the switch
  bool _homed;                 // position 0 is the switch

  long _homeBackoff;           // steps to back away after homing
  long _position;              // current position in steps
  long _target;                // goal position in TARGET and HOMING_BACKOFF
  unsigned long _intervalUs;        // time between steps
  unsigned long _homingIntervalUs;  // time between steps while homing
  unsigned long _lastStepUs;        // time of the last step
};
