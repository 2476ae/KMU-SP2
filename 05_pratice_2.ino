
const int LED = 7;
const int ON  = LOW;  
const int OFF = HIGH;   

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, ON);           
  delay(1000);

  for (int i = 0; i < 5; i++) {    
    digitalWrite(LED, OFF);
    delay(100);
    digitalWrite(LED, ON);
    delay(100);
  }

  digitalWrite(LED, OFF);           
  while (1) {}                      
}
