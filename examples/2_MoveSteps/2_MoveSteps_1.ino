// MoveSteps
// Shows moveSteps(), moveTo() and position tracking on the X motor.
//
// Open the Serial Monitor at 115200 baud to see the position after each move.
// The sequence is: move 200 steps up, go to position 1000, go back to 0.

#include <CNCShieldStepper.h>

int stage = 0;

// Starts the next move in the sequence.
void startNextMove() {
  switch (stage) {
    case 0:
      stepper.X.moveSteps(200);  // 200 steps from where it is now
      break;
    case 1:
      stepper.X.moveTo(1000);    // go to position 1000
      break;
    case 2:
      stepper.X.moveTo(0);       // go back to the start
      break;
  }
  stage = (stage + 1) % 3;
}

void setup() {
  Serial.begin(115200);
  stepper.begin();
  stepper.X.setSpeed(800);

  startNextMove();
}

void loop() {
  stepper.update();

  // isMoving() is false once a move has finished.
  if (!stepper.X.isMoving()) {
    Serial.print("Position: ");
    Serial.println(stepper.X.getPosition());

    delay(1000);  // fine here, because the motor is not moving
    startNextMove();
  }
}
