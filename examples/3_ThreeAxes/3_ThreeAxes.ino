// ThreeAxes
// Moves X, Y and Z at the same time, each at its own speed and distance.
// Every axis goes to its distance, then back to 0, over and over.

#include <CNCShieldStepper.h>

// Moves an axis between 0 and the given distance. Starts a new move
// whenever the axis has finished the last one.
void bounce(StepperAxis &axis, long distance) {
  if (!axis.isMoving()) {
    axis.moveTo(axis.getPosition() == 0 ? distance : 0);
  }
}

void setup() {
  stepper.begin();

  stepper.X.setSpeed(1000);
  stepper.Y.setSpeed(600);
  stepper.Z.setSpeed(400);

  // Uncomment a line if that motor turns the wrong way.
  // stepper.X.reverseDirection(true);
  // stepper.Y.reverseDirection(true);
  // stepper.Z.reverseDirection(true);
}

void loop() {
  bounce(stepper.X, 800);
  bounce(stepper.Y, 600);
  bounce(stepper.Z, 400);

  stepper.update();
}
