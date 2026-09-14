#define PIN_LED 13

unsigned int count;
bool toggle;

bool toggle_state(bool state) {
  return !state;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);

  while (!Serial) {
    // 시리얼 연결 대기
  }

  Serial.println("Hello World!");

  count = 0;
  toggle = LOW;
  digitalWrite(PIN_LED, toggle);
}

void loop() {
  Serial.println(++count);

  toggle = toggle_state(toggle);
  digitalWrite(PIN_LED, toggle);  // 세미콜론 추가

  delay(1000);
}
