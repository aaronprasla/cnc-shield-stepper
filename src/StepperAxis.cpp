#include "StepperAxis.h"

// Speed limits in steps per second.
static const long MIN_SPEED = 1;
static const long MAX_SPEED = 3000;
static const long DEFAULT_SPEED = 1000;

// Homing defaults.
static const long DEFAULT_HOMING_SPEED = 1000;
static const long DEFAULT_HOME_BACKOFF = 200;

// Length of the step pulse in microseconds. Drivers need at least 2.
static const unsigned int STEP_PULSE_US = 5;

StepperAxis::StepperAxis(uint8_t stepPin, uint8_t dirPin, uint8_t limitPin)
  : _stepPin(stepPin),
    _dirPin(dirPin),
    _limitPin(limitPin),
    _mode(IDLE),
    _continuousUp(true),
    _reversed(false),
    _limitEnabled(false),
    _limitUp(false),
    _limitNormallyClosed(false),
    _limitHit(false),
    _endpointsEnabled(false),
    _lowEndpoint(0),
    _highEndpoint(0),
    _endpointHit(false),
    _homed(false),
    _homeBackoff(DEFAULT_HOME_BACKOFF),
    _position(0),
    _target(0),
    _intervalUs(1000000UL / DEFAULT_SPEED),
    _homingIntervalUs(1000000UL / DEFAULT_HOMING_SPEED),
    _lastStepUs(0) {
}

void StepperAxis::begin() {
  pinMode(_stepPin, OUTPUT);
  pinMode(_dirPin, OUTPUT);
  digitalWrite(_stepPin, LOW);
  _mode = IDLE;
}

// Movement

void StepperAxis::moveUp() {
  _continuousUp = true;
  _limitHit = false;
  _endpointHit = false;
  _mode = CONTINUOUS;
}

void StepperAxis::moveDown() {
  _continuousUp = false;
  _limitHit = false;
  _endpointHit = false;
  _mode = CONTINUOUS;
}

void StepperAxis::moveSteps(long steps) {
  moveTo(_position + steps);
}

void StepperAxis::moveTo(long position) {
  _limitHit = false;
  _endpointHit = false;

  // A target outside the endpoints is clamped to the range.
  if (_endpointsEnabled) {
    position = constrain(position, _lowEndpoint, _highEndpoint);
  }

  _target = position;
  _mode = (_target == _position) ? IDLE : TARGET;
}

void StepperAxis::stop() {
  _mode = IDLE;
}

// Settings

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

// Position tracking

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

// Limit switch

void StepperAxis::attachLimit(LimitEnd end, SwitchType type) {
  _limitUp = (end == LIMIT_UP);
  _limitNormallyClosed = (type == NORMALLY_CLOSED);
  _limitHit = false;

  // The switch connects the pin to GND.
  pinMode(_limitPin, INPUT_PULLUP);
  _limitEnabled = true;
}

void StepperAxis::detachLimit() {
  _limitEnabled = false;

  // Homing can't finish without a switch.
  if (_mode == HOMING_SEEK) {
    _mode = IDLE;
  }
}

bool StepperAxis::isLimitPressed() const {
  return limitPressed();
}

bool StepperAxis::limitHit() const {
  return _limitHit;
}

bool StepperAxis::limitPressed() const {
  if (!_limitEnabled) {
    return false;
  }

  // With the pull-up, a normally open switch reads LOW when pressed and a
  // normally closed switch reads HIGH when pressed.
  bool high = (digitalRead(_limitPin) == HIGH);
  return _limitNormallyClosed ? high : !high;
}

// Endpoints

void StepperAxis::setEndpoints(long low, long high) {
  if (low > high) {
    long temp = low;
    low = high;
    high = temp;
  }

  _lowEndpoint = low;
  _highEndpoint = high;
  _endpointsEnabled = true;
  _endpointHit = false;
}

void StepperAxis::clearEndpoints() {
  _endpointsEnabled = false;
}

bool StepperAxis::endpointHit() const {
  return _endpointHit;
}

// Homing

bool StepperAxis::home() {
  if (!_limitEnabled) {
    return false;
  }

  _limitHit = false;
  _endpointHit = false;
  _homed = false;
  _mode = HOMING_SEEK;
  return true;
}

bool StepperAxis::isHoming() const {
  return _mode == HOMING_SEEK || _mode == HOMING_BACKOFF;
}

bool StepperAxis::isHomed() const {
  return _homed;
}

void StepperAxis::setHomingSpeed(long stepsPerSecond) {
  stepsPerSecond = constrain(stepsPerSecond, MIN_SPEED, MAX_SPEED);
  _homingIntervalUs = 1000000UL / stepsPerSecond;
}

void StepperAxis::setHomeBackoff(long steps) {
  _homeBackoff = (steps < 0) ? 0 : steps;
}

// Called when the switch triggers during homing. The switch position
// becomes 0, then the axis backs away from it.
void StepperAxis::startBackoff() {
  _position = 0;
  _homed = true;

  // Away from the switch is up if the switch is at the down end, and down
  // if the switch is at the up end.
  _target = _limitUp ? -_homeBackoff : _homeBackoff;
  _mode = (_target == _position) ? IDLE : HOMING_BACKOFF;
}

// Stepping

void StepperAxis::update() {
  if (_mode == IDLE) {
    return;
  }

  // Work out which way to step.
  bool up;

  if (_mode == CONTINUOUS) {
    up = _continuousUp;
  } else if (_mode == HOMING_SEEK) {
    up = _limitUp;  // toward the switch
  } else {
    // TARGET or HOMING_BACKOFF: head for the target.
    if (_position == _target) {
      _mode = IDLE;
      return;
    }
    up = (_target > _position);
  }

  // Wait until the next step is due.
  unsigned long interval = isHoming() ? _homingIntervalUs : _intervalUs;
  unsigned long nowUs = micros();
  if (nowUs - _lastStepUs < interval) {
    return;
  }
  _lastStepUs = nowUs;

  // The switch blocks movement toward it. Moving away is always allowed.
  if (_limitEnabled && up == _limitUp && limitPressed()) {
    if (_mode == HOMING_SEEK) {
      startBackoff();
    } else {
      _mode = IDLE;
      _limitHit = true;
    }
    return;
  }

  // Endpoints block movement past them, in either direction.
  if (_endpointsEnabled &&
      ((up && _position >= _highEndpoint) ||
       (!up && _position <= _lowEndpoint))) {
    _mode = IDLE;
    _endpointHit = true;
    return;
  }

  takeStep(up);

  // Go idle right after the final step of a target move.
  if ((_mode == TARGET || _mode == HOMING_BACKOFF) && _position == _target) {
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
