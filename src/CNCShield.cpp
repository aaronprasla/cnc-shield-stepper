#include "CNCShield.h"

// CNC Shield V3 pin assignments.
static const uint8_t X_STEP_PIN = 2;
static const uint8_t X_DIR_PIN = 5;
static const uint8_t X_LIMIT_PIN = 9;
static const uint8_t Y_STEP_PIN = 3;
static const uint8_t Y_DIR_PIN = 6;
static const uint8_t Y_LIMIT_PIN = 10;
static const uint8_t Z_STEP_PIN = 4;
static const uint8_t Z_DIR_PIN = 7;
static const uint8_t Z_LIMIT_PIN = 11;
static const uint8_t ENABLE_PIN = 8;

CNCShield::CNCShield()
  : X(X_STEP_PIN, X_DIR_PIN, X_LIMIT_PIN),
    Y(Y_STEP_PIN, Y_DIR_PIN, Y_LIMIT_PIN),
    Z(Z_STEP_PIN, Z_DIR_PIN, Z_LIMIT_PIN),
    _enablePin(ENABLE_PIN) {
}

void CNCShield::begin() {
  X.begin();
  Y.begin();
  Z.begin();

  pinMode(_enablePin, OUTPUT);
  enable();
}

void CNCShield::update() {
  X.update();
  Y.update();
  Z.update();
}

void CNCShield::enable() {
  // LOW turns the drivers on.
  digitalWrite(_enablePin, LOW);
}

void CNCShield::disable() {
  digitalWrite(_enablePin, HIGH);
}

void CNCShield::stopAll() {
  X.stop();
  Y.stop();
  Z.stop();
}

bool CNCShield::isAnyMoving() const {
  return X.isMoving() || Y.isMoving() || Z.isMoving();
}

// The one shared CNC shield object.
CNCShield stepper;
