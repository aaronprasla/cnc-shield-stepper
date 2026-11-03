// Homing
// Homes the X motor, then moves it between two positions.
//
// Homing moves toward the limit switch, sets the position to 0 where the
// switch triggers, then backs away. After homing, position 0 is the switch.
//
// Wiring: connect a switch between the X limit pin (9) and GND. The switch
// must be at the down end of the X axis.
//
// Open the Serial Monitor at 115200 baud to see the position.

#include <CNCShieldStepper.h>

bool atFar = false;

void setup() {
  Serial.begin(115200);

  stepper.begin();
  stepper.X.setSpeed(800);

  stepper.X.attachLimit(LIMIT_DOWN);  // homing needs a limit switch
  stepper.X.setHomingSpeed(300);      // steps per second while homing
  stepper.X.setHomeBackoff(200);      // steps to back away from the switch

  stepper.X.home();                   // start homing
}

void loop() {
  stepper.update();

  // Wait while homing or any other move is running.
  if (stepper.X.isMoving()) {
    return;
  }

  Serial.print("Position: ");
  Serial.println(stepper.X.getPosition());

  delay(1000);  // fine here, because the motor is not moving

  // Move between two positions, both away from the switch.
  stepper.X.moveTo(atFar ? 300 : 1000);
  atFar = !atFar;
}
