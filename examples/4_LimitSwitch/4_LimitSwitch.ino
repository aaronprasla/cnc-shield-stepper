// LimitSwitch
// Moves the X motor down until it hits a limit switch, backs away 500 steps,
// then does it again.
//
// Wiring: connect a switch between the X limit pin (9) and GND. The switch
// must be at the down end of the X axis.
//
// Open the Serial Monitor at 115200 baud to see what is happening.

#include <CNCShieldStepper.h>

void setup() {
  Serial.begin(115200);

  stepper.begin();
  stepper.X.setSpeed(800);

  // Turn on the switch at the down end. For a normally closed switch, use:
  // stepper.X.attachLimit(LIMIT_DOWN, NORMALLY_CLOSED);
  stepper.X.attachLimit(LIMIT_DOWN);
}

void loop() {
  stepper.update();

  // Act when the axis has stopped.
  if (!stepper.X.isMoving()) {
    if (stepper.X.limitHit()) {
      Serial.println("Limit switch hit. Backing away.");
      stepper.X.moveSteps(500);  // moving away from the switch is allowed
    } else {
      Serial.println("Moving toward the switch.");
      stepper.X.moveDown();
    }
  }
}
