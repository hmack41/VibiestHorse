#define LED_PIN PC13 // <-- Replace this with your board's correct LED pin
// #define MY_LED_PIN PC13
void setup() {
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  
}