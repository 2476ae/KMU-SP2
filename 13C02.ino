#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10

// configurable parameters for Servo
#define _DUTY_MIN 400 // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1500 // servo neutral position (90 degree)
#define _DUTY_MAX 2600 // servo full counter-clockwise position (180 degree)

// experiment 1: speed 3.0, end _DUTY_MAX (180 degree in 60 sec)
// experiment 2: speed 0.3, end _DUTY_NEU (90 degree in 300 sec)
#define _SERVO_SPEED 3.0       // servo angular speed (unit: degree/sec)
#define _POS_START _DUTY_MIN   // servo start position (0 degree)
#define _POS_END   _DUTY_MAX   // servo end position

// Loop Interval
#define INTERVAL 20     // servo update interval (unit: msec)

// global variables
unsigned long last_sampling_time; // unit: msec
unsigned long start_time;         // unit: msec

Servo myservo;

float duty_per_msec;  // duty change per msec
int duty_curr;        // Current duty time

void setup() {
  myservo.attach(PIN_SERVO, _DUTY_MIN, _DUTY_MAX);

  duty_curr = _POS_START;
  myservo.writeMicroseconds(duty_curr);

  // initialize serial port
  Serial.begin(57600);

  // convert angular velocity into duty change per msec.
  duty_per_msec = (_DUTY_MAX - _DUTY_MIN) / 180.0 * _SERVO_SPEED / 1000.0;

  delay(3000);  // hold at start position before moving

  start_time = last_sampling_time = millis();
}

void loop() {
  // wait until next sampling time.
  if (millis() < (last_sampling_time + INTERVAL))
    return;
  last_sampling_time += INTERVAL;

  // duty from elapsed time: constant speed, no rounding error accumulates
  unsigned long elapsed = millis() - start_time;
  float duty = _POS_START + duty_per_msec * elapsed;
  if (duty > _POS_END)
    duty = _POS_END;
  duty_curr = duty + 0.5;  // round to nearest usec

  // update servo position
  myservo.writeMicroseconds(duty_curr);

  // output the read value to the serial port
  Serial.print("time:");   Serial.print(elapsed / 1000.0);
  Serial.print(",angle:"); Serial.print((duty - _DUTY_MIN) * 180.0 / (_DUTY_MAX - _DUTY_MIN));
  Serial.print(",duty:");  Serial.println(duty_curr);
}
