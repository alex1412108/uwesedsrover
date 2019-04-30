



int SHDN6V = 23;
int SHDN12V = 22;






void setup() {
  // put your setup code here, to run once:

  pinMode(SHDN6V, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(SHDN6V, HIGH);
  delay(2000); 
  digitalWrite(SHDN6V, LOW);
  delay(2000); 

}
