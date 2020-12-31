void setup() {


Serial.begin(9600);

}

int (*modeSelect[])(int a, int b) = { funkyAdd, funkySub, funkyMul, funkyDiv } ;


void loop() {
  // put your main code here, to run repeatedly:
  
Serial.println((*modeSelect[0])(16, 2));
Serial.println((*modeSelect[1])(16, 2));
Serial.println((*modeSelect[2])(16, 2));
Serial.println((*modeSelect[3])(16, 2));

delay(1000);

}


  
int funkyAdd (int a, int b) {return a + b;}
int funkySub (int a, int b) {return a - b;}
int funkyMul (int a, int b) {return a * b;}
int funkyDiv (int a, int b) {return a / b;}
