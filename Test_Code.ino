void setup() {
  Serial.begin(115200);
  pinMode(9,OUTPUT); // VELOCITAT
  pinMode(10,OUTPUT); // DIRECCIÓ

}

void loop() {
  digitalWrite(10,HIGH); // SWAP
  digitalWrite(9, HIGH); // PWM 255

}
