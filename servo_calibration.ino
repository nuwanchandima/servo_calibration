#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVO_CH 3

int currentTick = 480;   // start near middle

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pwm.begin();
  pwm.setPWMFreq(50);
  delay(10);

  pwm.setPWM(SERVO_CH, 0, currentTick);

  Serial.println("=== SERVO CALIBRATION ===");
  Serial.println("Commands:");
  Serial.println("  T,xxx   -> set raw tick value");
  Serial.println("  P       -> print current tick");
}

void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();

  if (line.equalsIgnoreCase("P")) {
    Serial.print("Current tick = ");
    Serial.println(currentTick);
    return;
  }

  if (line.startsWith("T,") || line.startsWith("t,")) {
    int tick;
    if (sscanf(line.c_str(), "T,%d", &tick) == 1 ||
        sscanf(line.c_str(), "t,%d", &tick) == 1) {

      tick = constrain(tick, 75, 650);
      currentTick = tick;
      pwm.setPWM(SERVO_CH, 0, currentTick);

      Serial.print("Tick set to ");
      Serial.println(currentTick);
    } else {
      Serial.println("Format: T,xxx   example: T,320");
    }
  }
}
