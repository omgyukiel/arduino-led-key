#define SBAUD 115200

#define STB 10
#define CLK 9
#define DIO 8

#define CMDMSK 0x80 // 8 bit mask
#define ON 0x8F // turn on display with max brightness
#define OFF 0x87 // turn off display


void cmdout(int cmd){ // Send 8 bit instruction
  byte j;
  for(j=0; j<8; j++){ // left shift out 8 bits
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

void debug_setLED(int cmd) {
  shiftcmd(cmd); // debug
}
void setLED(int cmd) {
  digitalWrite(STB, LOW); // strobe low sets peripheral to listen on DIO
  cmdout(cmd); // send instruction
  digitalWrite(STB, HIGH); // complete data transmission
}

void debug_reset() {

  shiftcmd(0x40); // sequential addressing
  digitalWrite(STB, LOW);
  shiftOut(DIO, CLK, LSBFIRST, 0xC0); // starting address to 0

  for (int i = 0; i < 16; i++) {
    shiftOut(DIO, CLK, LSBFIRST, 0x3F); // sets digit to 0;
  }
  digitalWrite(STB, HIGH);
}
void reset() {
  shiftcmd(0x40); // sequential addressing
  // digitalWrite(STB, LOW);
  // shiftOut(DIO, CLK, LSBFIRST, 0xc0); // starting address to 0

  // for (int i = 0; i < 8; i++) {
  //   shiftOut(DIO, CLK, LSBFIRST, 0x00); // sets digit to 0;
  // }
  // digitalWrite(STB, HIGH);
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
  
  
  debug_setLED(ON); // activate led with max brightness
  // setLED(ON);
  // debug_reset();

}


void loop() {
  char mode;
  Serial.println("\nTM1638 Start\n");
  Serial.println("0 - Reset");
  Serial.println("1 - On");
  Serial.println("2 - Off");

  Serial.println("");
  Serial.print("CMD: ");
  while(Serial.available() == 0);
  mode = Serial.read();

  switch (mode) {
    case '0':

        Serial.print("reset");

      debug_reset();
      break;
    case '1':
        Serial.print("on");

      debug_setLED(ON);
      break;
    case '2':
    Serial.println("off");
      debug_setLED(OFF);
      break;
    default:
      break;
  }
  // delay(200);
  // int mode = RESET;

  // switch (MODE) {
  //   case 'RESET':
  //     Serial.println("Reset LED");
  //     break;
  // }
  
}
