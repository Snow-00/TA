//int test[]
int input1, input2, input3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  input1 = 1;
  input2 = 2;
  input3 = 3;
  int test[] = {input1, input2, input3};
  float size_ = sizeof(test) / sizeof(test[0]);
  Serial.println(size_);
  for (int i=0; i<size_; i++){
    Serial.println(test[i]);
    delay(100);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
