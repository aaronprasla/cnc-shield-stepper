# CNCShieldStepper

Simple control of the X, Y and Z stepper motors on an Arduino CNC Shield V3.

- Move a motor up, down, by a number of steps, or to a position
- Tracks each motor's position
- Non-blocking, so all three axes run at the same time
- A ready-made `stepper` object, so there is nothing to configure

## Install

1. Download the library as a .zip file.
2. In the Arduino IDE, choose Sketch > Include Library > Add .ZIP Library.
3. Open File > Examples > CNCShieldStepper to try an example.

## Quick start

```cpp
#include <CNCShieldStepper.h>

void setup() {
  stepper.begin();            // set up the pins and turn the drivers on
  stepper.X.setSpeed(800);    // steps per second
  stepper.X.moveSteps(200);   // move 200 steps up
}

void loop() {
  stepper.update();           // always call this in loop()
}
```

`stepper.update()` is what actually moves the motors. Call it as often as
possible and avoid long `delay()` calls while a motor is moving.

## Axes

Each motor is an axis: `stepper.X`, `stepper.Y` and `stepper.Z`. They all
work the same way, for example `stepper.Y.moveDown()`.

"Up" is the positive direction and "down" is the negative direction. This
applies to every axis, even horizontal ones.

## Axis functions

| Function | What it does |
|---|---|
| `moveUp()` | Moves up until `stop()` is called |
| `moveDown()` | Moves down until `stop()` is called |
| `moveSteps(n)` | Moves `n` steps from the current position. Negative moves down |
| `moveTo(position)` | Moves to an absolute position |
| `stop()` | Stops stepping. The motor keeps holding its position |
| `setSpeed(sps)` | Sets the speed in steps per second (1 to 4000) |
| `getSpeed()` | Returns the speed in steps per second |
| `reverseDirection(true)` | Flips the physical direction if the motor turns the wrong way |
| `getPosition()` | Returns the current position in steps |
| `setPosition(n)` | Sets the current position without moving |
| `resetPosition()` | Sets the current position to 0 |
| `stepsRemaining()` | Steps left in a `moveSteps()` or `moveTo()` move |
| `isMoving()` | True while the axis is moving |
| `update()` | Steps this one axis. Normally you use `stepper.update()` instead |

## Board functions

| Function | What it does |
|---|---|
| `stepper.begin()` | Sets up the pins and turns the drivers on. Call once in `setup()` |
| `stepper.update()` | Steps every axis. Call it in `loop()` |
| `stepper.enable()` | Turns all drivers on |
| `stepper.disable()` | Turns all drivers off, so the motors can be turned by hand |
| `stepper.stopAll()` | Stops every axis |
| `stepper.isAnyMoving()` | True if any axis is moving |

## Things to know

- **Call a move function once, not every loop.** `moveSteps()` and `moveTo()`
  start a new move each time they are called, so calling `moveSteps(200)` on
  every pass of `loop()` keeps pushing the target further away. Start the move
  once, then wait for `isMoving()` to become false. `moveUp()`, `moveDown()`
  and `stop()` are safe to call repeatedly.
- **Position is counted, not measured.** The position is the number of steps
  sent to the driver. The shield has no feedback, so if a motor stalls or skips
  steps, the position will be wrong.
- **Speed limit.** The speed is limited to 4000 steps per second per axis. The
  real maximum also depends on your motor, driver, supply voltage and load.
- **A new move replaces the old one.** Calling a move function while an axis is
  moving changes what it does. `moveSteps()` is relative to the current
  position, not the old target.
- **Limit switches are not supported yet.** The motors will not stop at the end
  of travel on their own.

## Pins (CNC Shield V3)

| Axis | Step | Direction |
|---|---|---|
| X | 2 | 5 |
| Y | 3 | 6 |
| Z | 4 | 7 |
| Enable (all axes) | 8 | |

## Hardware tips

- Plug the motor drivers (A4988 or DRV8825) in with the correct orientation.
- Power the shield from the motor supply (12 to 24 V). USB alone will not run
  the motors.
- Microstepping jumpers under each driver set how many steps make one
  revolution. With no jumpers, a typical 1.8 degree motor needs 200 steps per
  revolution. With all three jumpers, it needs 3200 on an A4988 or 6400 on a
  DRV8825.
- If a motor only buzzes or shakes, check its coil wiring and the driver's
  current limit.

## Examples

- **1_BasicMove**: one motor up, stop, down, stop
- **2_MoveSteps**: `moveSteps()`, `moveTo()` and position printing
- **3_ThreeAxes**: X, Y and Z moving together

## License

MIT. See the LICENSE file.
