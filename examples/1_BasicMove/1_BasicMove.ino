// BasicMove
// Moves the X motor up for 2 seconds, stops for 1, moves down for 2,
// stops for 1, then repeats.

#include <CNCShieldStepper.h>

void setup() {
  stepper.begin();         // set up the pins and turn the drivers on
  stepper.X.setSpeed(800); // steps per second
}

void loop() {
  // Position within a 6 second cycle.
  unsigned long phase = millis() % 6000;

  if (phase < 2000) {
    stepper.X.moveUp();
  } else if (phase < 3000) {
    stepper.X.stop();
  } else if (phase < 5000) {
    stepper.X.moveDown();
  } else {
    stepper.X.stop();
  }

  // Always call update() in loop(). This is what steps the motor.
  stepper.update();
}
