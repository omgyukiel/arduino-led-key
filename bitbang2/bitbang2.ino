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

void shiftcmd(int cmd) { // shift cmd for debug
  digitalWrite(STB, LOW);
  shiftOut(8, CLK, LSBFIRST, cmd);
  digitalWrite(STB, HIGH);
}

void setLED() {
  shiftcmd(0x8F); // debug
  return;
  int cmd;
  cmd = 0x8f; //turn on display with max brightness
  digitalWrite(STB, LOW); // strobe low sets peripheral to listen on DIO
  cmdout(cmd); // send instruction
  digitalWrite(STB, HIGH); // complete data transmission
}

void reset() {
  shiftcmd(0x40); // sequential addressing
  digitalWrite(STB, LOW);
  shiftOut(DIO, CLK, LSBFIRST, 0xc0); // starting address to 0

  for (int i = 0; i < 8, i++) {
    shiftOut(DIO, CLK, LSBFIRST, 0x00); // sets digit to 0;
  }
  digitalWrite(STB, HIGH);
  return;
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
  
  

  setLED(); // activate led with max brightness
}

#define RESET 0
void loop() {
  Serial.println("\nTM1638 Start\n");
  // int mode = RESET;

  // switch (MODE) {
  //   case 'RESET':
  //     Serial.println("Reset LED");
  //     break;
  // }
  
}
