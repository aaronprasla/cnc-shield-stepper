// Endpoints
// Sets software endpoints on the X motor and shows what happens at them.
//
// Endpoints are limits you set in code, not wired switches. The travel is
// limited to the range you give. moveTo() clamps its target to the range,
// and moving past an endpoint stops the axis.
//
// The sequence is:
//   1. Move to position 1000.
//   2. Ask for position 3000. It clamps to the high endpoint, 2000.
//   3. Move down continuously. It stops at the low endpoint, 0.
//   4. Move up continuously. It stops at the high endpoint, 2000.
//   5. Go back to position 1000, then start over.
//
// Open the Serial Monitor at 115200 baud to see the position after each move.

#include <CNCShieldStepper.h>

int stage = 0;

// Starts the next move in the sequence.
void startNextMove() {
  switch (stage) {
    case 0:
      stepper.X.moveTo(1000);
      break;
    case 1:
      stepper.X.moveTo(3000);  // clamps to the high endpoint, 2000
      break;
    case 2:
      stepper.X.moveDown();    // runs until the low endpoint
      break;
    case 3:
      stepper.X.moveUp();      // runs until the high endpoint
      break;
    case 4:
      stepper.X.moveTo(1000);
      break;
  }
  stage = (stage + 1) % 5;
}

void setup() {
  Serial.begin(115200);

  stepper.begin();
  stepper.X.setSpeed(800);

  // Limit the travel to steps 0 to 2000.
  stepper.X.setEndpoints(0, 2000);

  startNextMove();
}

void loop() {
  stepper.update();

  // Act when the axis has stopped.
  if (!stepper.X.isMoving()) {
    Serial.print("Position: ");
    Serial.println(stepper.X.getPosition());
    if (stepper.X.endpointHit()) {
      Serial.println("Endpoint hit.");
    }

    delay(1000);  // fine here, because the motor is not moving
    startNextMove();
  }
}
