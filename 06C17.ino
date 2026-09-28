const int LED_PIN = 7;
const int PERIODS[] = {10000, 1000, 100};
const unsigned long TICK_US = 5000;

int cur_period = 1000;
int cur_duty = 0;
int on_us, off_us;

void update_pulse() {
  on_us = (long)cur_period * cur_duty / 100;
  off_us = cur_period - on_us;
}
void set_period(int period) { cur_period = constrain(period, 100, 10000); update_pulse(); }
void set_duty(int duty) { cur_duty = constrain(duty, 0, 100); update_pulse(); }

void pwm_cycle() {
  if (on_us > 0) { digitalWrite(LED_PIN, LOW); delayMicroseconds(on_us); }
  if (off_us > 0) { digitalWrite(LED_PIN, HIGH); delayMicroseconds(off_us); }
}

unsigned long last_tick;
int tick = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  set_period(PERIODS[0]);
  set_duty(0);
  last_tick = micros();
}

void loop() {
  pwm_cycle();
  if (micros() - last_tick < TICK_US) return;

  while (micros() - last_tick >= TICK_US) {
    last_tick += TICK_US;
    tick = (tick + 1) % 3000;
  }
  int s = tick % 200;
  set_period(PERIODS[tick / 1000]);
  set_duty(s <= 100 ? s : 200 - s);
}
