#define SBAUD 115200

#define STB 10
#define CLK 9
#define DIO 8

#define CMDMSK 0x01 // 8 bit mask for least significant bit (LSB)
#define ON 0x8F // turn on display with max brightness
#define OFF 0x87 // turn off display

              /*   0     1     2     3    4     5     6     7     8     9 */
int digits[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; // dictionary for 7-segement integer representations
// int led[8] = {};
int count = 0;
int t = 0;
int debounce = 130;

void cmdout(int cmd){ // Send 8 bit instruction
  byte j;
  for(j=0; j<8; j++){ // left shift out 8 bits
    if(cmd & CMDMSK){ // Set output pin to 1/0 from mask
      digitalWrite(DIO,HIGH);
    } else {
      digitalWrite(DIO,LOW);
    }
    // clocks the LSB at the output pin
    digitalWrite(CLK, HIGH);  
    cmd = cmd>>1;
    digitalWrite(CLK, LOW); 
  }
}

int push_read() {
  int cmd;
  byte v;
  int buttons = 0x00;
  int place;

  cmd = 0x42; // Read the buttons and set DIO to MISO and read 4 bytes
  digitalWrite(STB, LOW);
  cmdout(cmd); // Read data
  pinMode(DIO, INPUT_PULLUP);


  for (int i = 0; i < 4; i++) { // read 4 bytes
    place = 1000/(pow(10, i));
    v = datin(8); // receive 1 byte (8 bits)
    // Serial.println("Received byte ");
    // Serial.print(i);
    // Serial.print("\t");
    // Serial.println(v, HEX);
    if (v == 0x8) { // S5-S8 is pushed
      v = 0x10;
      v = v<<i;
      // Serial.print("\nbutton pushed at v:");
      // Serial.println(v);
      count += place;
      if (count > 9999) {
        count = 0;
      }
    }
    else if (v == 0x80) {
      v = 0x1;
      v = v<<i;
      // Serial.print("\nbutton pushed at v:");
      // Serial.println(v);
      count = 0;

    }
    buttons = buttons | v;
    // buttons << 8;
  }
  
  pinMode(DIO, OUTPUT);

  digitalWrite(STB, HIGH);
  return buttons;
}

void led(byte v, int pos) {
  int cmd;
  pos = 0xc1 + (pos<<1);
  // Serial.print("pos");
  // Serial.println(pos);
  cmd = 0x44; // single address display
  digitalWrite(STB, LOW);
  cmdout(0x44);
  digitalWrite(STB, HIGH);

  digitalWrite(STB, LOW);
  // Serial.print("c1 + pos");
  // Serial.println(pos, BIN);
  // Serial.print("bit ");
  // Serial.println(v);
  cmdout(pos); // led 1 address increcement by next led position
  cmdout(v);
  digitalWrite(STB, HIGH);
}
void display_led(byte buttons) {
  int msk;
  for(int pos = 0; pos <8; pos++) {
    msk = 0x1<<pos; // mask each bit position
    led(buttons&msk ? 1:0, pos); // bit at position i is 1, set led
  }
}
int counter() {
  int buttons = push_read();
  // if (count > 9999) {
  //   reset();
  // }
  // Serial.println("\nbuttons: ");
  // Serial.print(buttons, BIN);
  // Serial.println("");
  display_led(buttons);

  // Serial.print("\ncounter: ");
  // Serial.println(count);
  // Serial.println("\n");
  
  display_num(count, buttons);


  return buttons;
}

int datin(byte cnt) {
  int v;
  byte j;
  v = 0;
  for(j = 0; j<cnt; j++) { // loop for cnt bits
    v = v<<1;
    digitalWrite(CLK, HIGH);
    v = v | digitalRead(DIO);
    digitalWrite(CLK, LOW);
  }
  return v;
}

void shiftcmd(int cmd) { // shift cmd for debug
  digitalWrite(STB, LOW);
  shiftOut(8, CLK, LSBFIRST, cmd);
  digitalWrite(STB, HIGH);
}

void display_num(int num, int buttons) { // supports 4 digit display
  // if (buttons > 8) {
  //   num = 0;
  //   count = 0;
  // }
  int cmd;
  int digit;
  // int msk;
  // for(int pos = 0; pos <4; pos++) {
  //   msk = 0x1<<pos; // mask each bit position
  //   led(buttons&msk ? 1:0, pos); // bit at position i is 1, set led
  // }
  // return;
  int powten[4] = {1000,100,10,1};
  for (int i = 0; i < 4; i++) {
    cmd = 0x44; 
    digitalWrite(STB, LOW);
    cmdout(cmd);
    digitalWrite(STB, HIGH);

    cmd = (0xc8 + i*2); // 1000s address
    digit = (num/powten[i] % 10);
    digitalWrite(STB, LOW);
    cmdout(cmd);
    cmdout(digits[digit]); // sets digit to dec;
    digitalWrite(STB, HIGH);
  }

  // digitalWrite(STB, LOW);

  // int powten[4] = {1, 10, 100, 1000};
  // digit = (num/powten[i] % 10);
  // cmdout(digits[digit]); // sets digit to dec;

  // for (int i = 3; i >= 0; i--) {
  //   // Serial.print("counter =");
  //   // Serial.println(num);
  //   // Serial.print("powten =");
  //   // Serial.println(powten[i]);
  //   digit = (num/powten[i] % 10);

  //   // Serial.print("digit = ");
  //   // Serial.println(digit);

  //   // Serial.print("digits[i] = ");
  //   // Serial.println(digits[digit], HEX);
  //   cmdout(digits[digit]); // sets digit to dec;
  //   // cmdout(0x00); // sets led off;

  // }
  // digitalWrite(STB, HIGH);

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
  digitalWrite(STB, HIGH);

  digitalWrite(STB, LOW);
  for (int i = 0; i < 17; i++) {
    if (i%2) { // odd
      shiftOut(DIO, CLK, LSBFIRST, 0x3F); // sets digit to 0;
    } else {
      shiftOut(DIO, CLK, LSBFIRST, 0x01); // set led on
    }
  }
  digitalWrite(STB, HIGH);
}
void reset() {
  count = 0;
  int cmd;
  cmd = 0x40; // sequential addressing
  digitalWrite(STB, LOW);
  cmdout(cmd);
  digitalWrite(STB, HIGH);


  cmd = 0xc0; // first address
  digitalWrite(STB, LOW);
  cmdout(cmd);
  // digitalWrite(STB, HIGH);

  // digitalWrite(STB, LOW);
  for (int i = 0; i < 16; i++) {
    if (i%2) { // odd
      cmdout(0x00); // sets led offS

    } else {
      cmdout(0x3F); // sets digit to 0;
    }
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
  
  
  // debug_setLED(ON); // activate led with max brightness
  setLED(ON); // Activate led with max brightness
  reset(); // reset led
  // debug_reset();

}

void loop() {
  int button = counter();
  if (button) { // buttons is being pressed
    t++;
  }
  else {
    t = 0;
  }

  if (t > 4) {
    Serial.println(t);
    delay( debounce/(log(t)));
  } else {
    Serial.println(t);
    delay(debounce); // debounce time
  }

}
