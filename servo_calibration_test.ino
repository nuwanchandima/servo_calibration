#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// ---- PCA9685 timing (calibrated) ----
#define SERVOMIN  100   // 0 deg
#define SERVOMAX  428   // 180 deg

// ---- Channel ----
#define SERVO_BASE_CH  3

// ---- Joint limits for base ----
const int BASE_MIN_ANGLE = 0;
const int BASE_MAX_ANGLE = 180;

// ---- Current angle ----
int base_angle = 0;

// ---- Smooth speed ----
int g_stepDeg     = 1;
int g_stepDelayMs = 15;

// Convert angle -> PCA9685 tick
int angleToTick(int angle) {
  angle = constrain(angle, BASE_MIN_ANGLE, BASE_MAX_ANGLE);
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

// Write only base servo
void writeBaseServo() {
  pwm.setPWM(SERVO_BASE_CH, 0, angleToTick(base_angle));
}

// Smoothly move base servo to target
void smoothMoveBase(int target, int stepDeg, int stepDelayMs) {
  target = constrain(target, BASE_MIN_ANGLE, BASE_MAX_ANGLE);

  while (base_angle != target) {
    if (base_angle < target) {
      base_angle = min(base_angle + stepDeg, target);
    } else {
      base_angle = max(base_angle - stepDeg, target);
    }
    writeBaseServo();
    delay(stepDelayMs);
  }
}

void printCurrent() {
  Serial.print("base = ");
  Serial.print(base_angle);
  Serial.print(" deg   | tick = ");
  Serial.print(angleToTick(base_angle));
  Serial.print("   | step = ");
  Serial.print(g_stepDeg);
  Serial.print("   | delay = ");
  Serial.println(g_stepDelayMs);
}

void printHelp() {
  Serial.println("Commands:");
  Serial.println("  angle          -> move base to this angle (0-180)");
  Serial.println("  S,step,delay   -> change smoothing (example: S,1,15)");
  Serial.println("  P              -> print current angle and tick");
  Serial.println("  L              -> show limits");
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(10);

  // Start position
  base_angle = constrain(base_angle, BASE_MIN_ANGLE, BASE_MAX_ANGLE);
  writeBaseServo();

  Serial.println("\n=== BASE SERVO TEST (channel 0 only) ===\n");
  printHelp();
  printCurrent();
}

void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  // P -> print current
  if (line.equalsIgnoreCase("P")) {
    printCurrent();
    return;
  }

  // L -> show limits
  if (line.equalsIgnoreCase("L")) {
    Serial.print("Base limits: ");
    Serial.print(BASE_MIN_ANGLE);
    Serial.print(" - ");
    Serial.print(BASE_MAX_ANGLE);
    Serial.println(" deg");
    return;
  }

  // S,step,delay
  if (line.startsWith("S,") || line.startsWith("s,")) {
    int st, dly;
    int cnt = sscanf(line.c_str(), "S,%d,%d", &st, &dly);
    if (cnt != 2) {
      cnt = sscanf(line.c_str(), "s,%d,%d", &st, &dly);
    }

    if (cnt == 2) {
      g_stepDeg     = constrain(st, 1, 30);
      g_stepDelayMs = constrain(dly, 0, 200);

      Serial.print("Speed updated -> step=");
      Serial.print(g_stepDeg);
      Serial.print("  delay=");
      Serial.println(g_stepDelayMs);
    } else {
      Serial.println("Format: S,step,delay   example: S,2,20");
    }
    return;
  }

  // Single number = target angle
  int target;
  if (sscanf(line.c_str(), "%d", &target) == 1) {
    Serial.print("Moving base to -> ");
    Serial.print(target);
    Serial.println(" deg");

    if (target < BASE_MIN_ANGLE || target > BASE_MAX_ANGLE) {
      Serial.println("-> OUT OF RANGE (0-180)");
      return;
    }

    smoothMoveBase(target, g_stepDeg, g_stepDelayMs);
    Serial.println("-> Done");
    printCurrent();
    return;
  }

  // Unknown command
  Serial.println("Unknown command.");
  printHelp();
}
