#define SBAUD 115200

#define STB 10
#define CLK 9
#define DIO 8

#define CMDMSK 0x80 // 8 bit mask


void cmdout(int cmd){ // Send 8 bit instruction
  byte j;
  for(j=0; j<8; j++){
    if(cmd & CMDMSK){ // Set output pin to 1/0 from mask
      digitalWrite(DIO,HIGH);
    } else {
      digitalWrite(DIO,LOW);
    }
    // clocks the MSB at the output pin
    digitalWrite(CLK, HIGH);  
    cmd = cmd<<1;
    digitalWrite(CLK, LOW); 
  }
}

void setLED() {
  int cmd;
  cmd = 0x8f; //turn on display with max brightness
  digitalWrite(STB, LOW); // strobe low sets peripheral to listen on DIO
  cmdout(cmd); // send instruction
  digitalWrite(STB, HIGH); // complete data transmission
}

void setup() {
  pinMode(STB, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT); // set i/o to MOSI at first to activate LED

  Serial.begin(SBAUD);
  Serial.print("\nCold Boot\n");

  digitalWrite(STB, HIGH); // set high on completion of instruction and data transmissions
  digitalWrite(CLK, LOW);
  digitalWrite(DIO, LOW);
  
  digitalWrite(STB, LOW);
  shiftOut(DIO, CLK, LSBFIRST, 0x8f);
  digitalWrite(STB, HIGH);

  // setLED(); // activate led with max brightness
}

void loop() {
  Serial.println("\nTM1638 Start\n");
  
}
