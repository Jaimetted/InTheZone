const int SIZE = 25;
char data[SIZE] = "x+123.456+1234567-00000.1"; //String data

void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
}

String format(float n){
  String result(""),number(n);;
  if(n >= 0) result += '+';
  result += number;
  for(int i = result.length(); i < 8; i++){
    result += '0';
  }
  return result.substring(0,8);
}

void loop() {
  // Serial usa los pins 0 y 1 
  //Serial3.write(data,SIZE); //Write the serial data
  Serial.println(format(250.45));
  Serial.println(format(-250.45));
  Serial.println(format(250.0000));
  Serial.println(format(0));
  Serial.println(format(-0.1));
  Serial.println(format(10.11));
  delay(10000);
}
