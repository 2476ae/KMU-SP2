/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/faq/how-to-control-speed-of-servo-motor
 */

#include <Servo.h>

#define PIN_LED   9
#define PIN_SERVO 10
#define PIN_TRIG  12
#define PIN_ECHO  13

#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 20
#define _DIST_MAX 300
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)
#define _EMA_ALPHA 0.5

#define DETECT_DIST 100
#define HOLD_TIME 1000
#define ANGLE_DOWN 0
#define ANGLE_UP 90

#define EASE_MODE 1
#define SIGMOID_K 10.0

Servo myServo;
unsigned long MOVING_TIME = 3000; // moving time is 3 seconds
unsigned long moveStartTime;
float startAngle = ANGLE_DOWN;
float stopAngle  = ANGLE_DOWN;
float curAngle   = ANGLE_DOWN;

unsigned long last_sampling_time;
unsigned long last_seen_time;
float dist_prev = _DIST_MAX;
float dist_ema = _DIST_MAX;
bool want_up = false;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);

  myServo.attach(PIN_SERVO);
  myServo.write(ANGLE_DOWN); // Set position
  delay(500);
  moveStartTime = millis() - MOVING_TIME;
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL)
    return;
  unsigned long now = millis();

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  float dist_filtered;
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
    dist_filtered = _DIST_MAX;
  } else if (dist_raw < _DIST_MIN) {
    dist_filtered = dist_prev;
  } else {
    dist_filtered = dist_raw;
  }
  dist_prev = dist_filtered;
  dist_ema = _EMA_ALPHA * dist_filtered + (1 - _EMA_ALPHA) * dist_ema;

  bool car = dist_ema < DETECT_DIST;
  if (car) {
    last_seen_time = now;
    want_up = true;
  } else if (now - last_seen_time >= HOLD_TIME) {
    want_up = false;
  }

  float target = want_up ? ANGLE_UP : ANGLE_DOWN;
  if (target != stopAngle) {
    startAngle = curAngle;
    stopAngle = target;
    moveStartTime = now;
  }

  unsigned long progress = now - moveStartTime;
  float p = min((float)progress / MOVING_TIME, 1.0);
  float sig = startAngle + (stopAngle - startAngle) * sigmoid_ease(p);
  float cosn = startAngle + (stopAngle - startAngle) * cosine_ease(p);
  curAngle = (EASE_MODE == 0) ? sig : cosn;
  myServo.writeMicroseconds(MIN_PULSE_WIDTH + curAngle * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / 180.0);

  digitalWrite(PIN_LED, car ? 0 : 1);

  Serial.print("Min:");      Serial.print(_DIST_MIN);
  Serial.print(",dist:");    Serial.print(min(dist_raw, _DIST_MAX + 100));
  Serial.print(",ema:");     Serial.print(dist_ema);
  Serial.print(",angle:");   Serial.print(curAngle);
  Serial.print(",sigmoid:"); Serial.print(sig);
  Serial.print(",cosine:");  Serial.print(cosn);
  Serial.print(",Max:");     Serial.print(_DIST_MAX);
  Serial.println("");

  last_sampling_time += INTERVAL;
}

float sigmoid_ease(float p) {
  float s0 = 1.0 / (1.0 + exp(SIGMOID_K * 0.5));
  float s1 = 1.0 / (1.0 + exp(-SIGMOID_K * 0.5));
  float s = 1.0 / (1.0 + exp(-SIGMOID_K * (p - 0.5)));
  return (s - s0) / (s1 - s0);
}

float cosine_ease(float p) {
  return (1.0 - cos(PI * p)) / 2.0;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
