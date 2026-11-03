#include "StepperAxis.h"

// Speed limits in steps per second.
static const long MIN_SPEED = 1;
static const long MAX_SPEED = 4000;
static const long DEFAULT_SPEED = 1000;

// Length of the step pulse in microseconds. Drivers need at least 2.
static const unsigned int STEP_PULSE_US = 5;

StepperAxis::StepperAxis(uint8_t stepPin, uint8_t dirPin)
  : _stepPin(stepPin),
    _dirPin(dirPin),
    _mode(IDLE),
    _continuousUp(true),
    _reversed(false),
    _position(0),
    _target(0),
    _intervalUs(1000000UL / DEFAULT_SPEED),
    _lastStepUs(0) {
}

void StepperAxis::begin() {
  pinMode(_stepPin, OUTPUT);
  pinMode(_dirPin, OUTPUT);
  digitalWrite(_stepPin, LOW);
  _mode = IDLE;
}

void StepperAxis::moveUp() {
  _continuousUp = true;
  _mode = CONTINUOUS;
}

void StepperAxis::moveDown() {
  _continuousUp = false;
  _mode = CONTINUOUS;
}

void StepperAxis::moveSteps(long steps) {
  moveTo(_position + steps);
}

void StepperAxis::moveTo(long position) {
  _target = position;
  _mode = (_target == _position) ? IDLE : TARGET;
}

void StepperAxis::stop() {
  _mode = IDLE;
}

void StepperAxis::setSpeed(long stepsPerSecond) {
  stepsPerSecond = constrain(stepsPerSecond, MIN_SPEED, MAX_SPEED);
  _intervalUs = 1000000UL / stepsPerSecond;
}

long StepperAxis::getSpeed() const {
  return 1000000UL / _intervalUs;
}

void StepperAxis::reverseDirection(bool reversed) {
  _reversed = reversed;
}

long StepperAxis::getPosition() const {
  return _position;
}

void StepperAxis::setPosition(long position) {
  _position = position;

  // Keep a running move consistent with the new position.
  if (_mode == TARGET && _target == _position) {
    _mode = IDLE;
  }
}

void StepperAxis::resetPosition() {
  setPosition(0);
}

long StepperAxis::stepsRemaining() const {
  if (_mode != TARGET) {
    return 0;
  }
  return abs(_target - _position);
}

bool StepperAxis::isMoving() const {
  return _mode != IDLE;
}

void StepperAxis::update() {
  if (_mode == IDLE) {
    return;
  }

  bool up;

  if (_mode == TARGET) {
    if (_position == _target) {
      _mode = IDLE;
      return;
    }
    up = (_target > _position);
  } else {
    up = _continuousUp;
  }

  unsigned long nowUs = micros();
  if (nowUs - _lastStepUs < _intervalUs) {
    return;
  }
  _lastStepUs = nowUs;

  takeStep(up);

  // Go idle right after the final step of a target move.
  if (_mode == TARGET && _position == _target) {
    _mode = IDLE;
  }
}

void StepperAxis::takeStep(bool up) {
  // Set the direction before sending the step pulse.
  bool dirLevel = (up != _reversed);
  digitalWrite(_dirPin, dirLevel ? HIGH : LOW);

  // One pulse = one step.
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(STEP_PULSE_US);
  digitalWrite(_stepPin, LOW);

  _position += up ? 1 : -1;
}
