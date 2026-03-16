# Servo Calibration

Use these 2 files in this order.

## 1. Find servo pulse values

Upload `servo_calibration.ino` first.

Open Serial Monitor:

- Baud rate: `115200`
- Line ending: `Newline`

Commands:

- `T,320` -> move servo using raw pulse value
- `P` -> print current pulse value

Move the servo and find the pulse values for:

- `0 deg`
- `90 deg`
- `180 deg`

Write those 3 values down.

Example:

```text
0 deg = 100
90 deg = 264
180 deg = 428
```

## 2. Test servo by angle

Open `servo_calibration_test.ino`.

Update:

```cpp
#define SERVOMIN  100
#define SERVOMAX  428
```

Put your measured `0 deg` value in `SERVOMIN` and your measured `180 deg` value in `SERVOMAX`.

Then upload `servo_calibration_test.ino`.

Open Serial Monitor again.

Commands:

- `0` to `180` -> move servo to that angle
- `P` -> print current angle
- `S,1,15` -> change movement speed
- `L` -> show angle limits

Example test:

```text
0
90
180
```

## Notes

- Servo driver address: `0x40`
- Servo channel: `3`
- Install library: `Adafruit PWM Servo Driver Library`
