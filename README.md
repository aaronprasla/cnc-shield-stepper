# CNCShieldStepper

Simple control of the X, Y and Z stepper motors on an Arduino CNC Shield V3.

- Move a motor up, down, by a number of steps, or to a position
- Tracks each motor's position
- Limit switch protection and homing
- Software travel endpoints with `setEndpoints()`
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
| `setSpeed(sps)` | Sets the speed in steps per second (1 to 3000) |
| `getSpeed()` | Returns the speed in steps per second |
| `reverseDirection(true)` | Flips the physical direction if the motor turns the wrong way |
| `getPosition()` | Returns the current position in steps |
| `setPosition(n)` | Sets the current position without moving |
| `resetPosition()` | Sets the current position to 0 |
| `stepsRemaining()` | Steps left in a `moveSteps()` or `moveTo()` move |
| `isMoving()` | True while the axis is moving, including while homing |
| `update()` | Steps this one axis. Normally you use `stepper.update()` instead |

### Limit switch functions

| Function | What it does |
|---|---|
| `attachLimit(end, type)` | Turns on the limit switch. `end` is `LIMIT_DOWN` or `LIMIT_UP`. `type` is `NORMALLY_OPEN` (default) or `NORMALLY_CLOSED` |
| `detachLimit()` | Turns the limit switch off |
| `isLimitPressed()` | True while the switch is pressed |
| `limitHit()` | True if the last move was stopped by the switch. Cleared when a new move starts |

### Endpoint functions

| Function | What it does |
|---|---|
| `setEndpoints(low, high)` | Sets the low and high endpoints of travel in steps. The smaller value becomes the low endpoint |
| `clearEndpoints()` | Turns the endpoints off, so the axis can move anywhere again |
| `endpointHit()` | True if the last move was stopped by an endpoint. Cleared when a new move starts |

### Homing functions

| Function | What it does |
|---|---|
| `home()` | Starts homing. Returns false if no limit switch is attached |
| `isHoming()` | True while homing is running |
| `isHomed()` | True once the axis has been homed |
| `setHomingSpeed(sps)` | Speed used while homing (default 1000) |
| `setHomeBackoff(steps)` | Steps to back away from the switch after homing (default 200) |

## Board functions

| Function | What it does |
|---|---|
| `stepper.begin()` | Sets up the pins and turns the drivers on. Call once in `setup()` |
| `stepper.update()` | Steps every axis. Call it in `loop()` |
| `stepper.enable()` | Turns all drivers on |
| `stepper.disable()` | Turns all drivers off, so the motors can be turned by hand |
| `stepper.stopAll()` | Stops every axis |
| `stepper.isAnyMoving()` | True if any axis is moving |

## Limit switches

Each axis has one limit switch input. Tell the axis which end of travel the
switch is at:

```cpp
stepper.X.attachLimit(LIMIT_DOWN);                   // switch at the down end
stepper.Z.attachLimit(LIMIT_UP, NORMALLY_CLOSED);    // switch at the up end
```

While the switch is pressed:

- Moving toward the switch is blocked. The axis stops right away and any
  move in progress is cancelled. `limitHit()` becomes true.
- Moving away from the switch is always allowed, so the axis can back off.
- If the switch is already pressed, a move toward it does nothing.

An axis without `attachLimit()` has no limit protection.

### Wiring

Connect the switch between the axis' limit pin and GND. The library turns on
the internal pull-up resistor, so no extra parts are needed.

| Axis | Limit pin |
|---|---|
| X | 9 |
| Y | 10 |
| Z | 11 |

A normally closed switch is the safer choice, because a broken wire counts as
pressed and stops the axis. Use `NORMALLY_CLOSED` when wiring one.

## Homing

Homing finds a known starting point using the limit switch:

1. The axis moves toward the switch at the homing speed.
2. When the switch triggers, the position is set to 0. Position 0 is the
   switch.
3. The axis backs away from the switch by the home backoff distance.

```cpp
stepper.X.attachLimit(LIMIT_DOWN);
stepper.X.setHomingSpeed(1000);
stepper.X.setHomeBackoff(200);
stepper.X.home();            // starts homing, then keep calling update()
```

Homing doesn't block. Keep calling `stepper.update()` and wait for
`isHoming()` to become false. Afterwards `isHomed()` is true and the axis sits
at the backoff position. For a switch at the up end the backoff position is
negative, because the axis backs away downward.

If the switch never triggers, the axis keeps moving toward it until you call
`stop()`.

## Endpoints

Endpoints are software limits for the range of travel, so no wiring is
needed. You set them in steps, relative to position 0:

```cpp
stepper.X.setEndpoints(0, 2000);  // travel is limited to steps 0 to 2000
```

With endpoints set:

- Moving past an endpoint is blocked, just like at a limit switch. The axis
  stops right away and `endpointHit()` becomes true. 
- `moveTo()` clamps its target to the range. `moveTo(5000)` moves to 2000.
- `moveUp()` and `moveDown()` stop at the endpoint.
- Homing also stops if the limit switch is beyond an endpoint.
- If you are caught outside of an endpoint when you set it, you can move
  back into the endpoint range, but not away from the endpoint.

The endpoints are only as good as the position tracking. If the motor
stalls, the position is wrong and the endpoints no longer match the
machine. A physical limit switch is still the safer protection, and
endpoints don't replace one.

The arguments may be given in either order; the smaller value always
becomes the low endpoint.

`clearEndpoints()` turns the endpoints off, so the axis can move anywhere
again.

## Things to know

- **Call a move function once, not every loop.** `moveSteps()` and `moveTo()`
  start a new move each time they are called, so calling `moveSteps(200)` on
  every pass of `loop()` keeps pushing the target further away. Start the move
  once, then wait for `isMoving()` to become false. `moveUp()`, `moveDown()`
  and `stop()` are safe to call repeatedly.
- **Position is counted, not measured.** The position is the number of steps
  sent to the driver. The shield has no feedback, so if a motor stalls or skips
  steps, the position will be wrong. Homing resets it.
- **Speed limit.** The speed is limited to 3000 steps per second per axis. The
  real maximum also depends on your motor, driver, supply voltage and load.
- **A new move replaces the old one.** Calling a move function while an axis is
  moving changes what it does. `moveSteps()` is relative to the current
  position, not the old target. Starting a move or calling `stop()` during
  homing cancels homing.
- **One limit switch per axis.** The shield has one input per axis, so each
  axis protects one end of travel.

## Pins (CNC Shield V3)

| Axis | Step | Direction | Limit switch |
|---|---|---|---|
| X | 2 | 5 | 9 |
| Y | 3 | 6 | 10 |
| Z | 4 | 7 | 11 |
| Enable (all axes) | 8 | | |

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
- **4_LimitSwitch**: stops at a limit switch and backs away
- **5_Homing**: homes the X axis, then moves between two positions
- **6_Endpoints**: sets software endpoints and shows the motor stopping at them

## License

MIT. See the LICENSE file.
