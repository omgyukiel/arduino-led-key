
/*
 * cbbob.ino
 * 93c46 128x8 NVram
 */

#define SBAUD 115200


#define CS 10
#define SK 13
#define DI 11
#define DO 12

#define DBG 10
#define DBG2 11

#define CMDMSK 0x200

void cmdout(int cmd, int msk){
  byte j;
  for(j=0; j<10; j++){
    if(cmd & msk){
      digitalWrite(DI,HIGH);
    } else {
      digitalWrite(DI,LOW);
    }
    digitalWrite(SK, HIGH);  // clock data into NV
    cmd = cmd<<1;
    digitalWrite(SK, LOW);
  }
}

int datin(byte cnt){
  int v;
  byte j;
  v=0;
  for(j=0; j<cnt; j++){
    v = v<<1;
    digitalWrite(SK,HIGH);
    v = v | digitalRead(DO);
    digitalWrite(SK,LOW);
  }
  return v;
  
}

void datout(int v, byte cnt){
  byte j;
  int msk;
  msk=0x8000;  // d15 msk
  if (cnt==8) msk=0x80;  // d7 msk
  for(j=0; j<cnt; j++){
    if(v & msk){
      digitalWrite(DI,HIGH);
    } else {
      digitalWrite(DI,LOW);
    }
    digitalWrite(SK, HIGH);  // clock data into NV
    v = v<<1;
    digitalWrite(SK, LOW);
    
  }
}

void NV_wral(int v){
  int cmd;
  cmd = 0x220;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  datout(v, 8);  
  digitalWrite(CS,LOW);
  digitalWrite(CS,HIGH);
  while( 0 == digitalRead(DO));
  digitalWrite(CS,LOW);
}

void NV_ewds(){
  int cmd;
  cmd = 0x200;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  digitalWrite(CS,LOW);
}

void NV_ewen(){
  int cmd;
  cmd = 0x260;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  digitalWrite(CS,LOW);
}

void NV_eral(){
  int cmd;
  cmd = 0x240;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  digitalWrite(CS,LOW);
  digitalWrite(CS,HIGH);
  while( 0 == digitalRead(DO));
  digitalWrite(CS,LOW);
  
}


int NV_read(byte adr){
  int cmd;
  int v;
  cmd = 0x300 + adr;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  v = datin(8);  
  digitalWrite(CS,LOW);
  return v;
}

void NV_write(byte adr, int v){
  int cmd;
  cmd = 0x280 + adr;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  datout(v,8);  
  digitalWrite(CS,LOW);
  digitalWrite(CS,HIGH);
  while( 0 == digitalRead(DO));
  digitalWrite(CS,LOW);
  
}

void NV_erase(byte adr){
  int cmd;
  cmd = 0x380 + adr;
  digitalWrite(CS,HIGH);
  cmdout(cmd, CMDMSK);
  digitalWrite(CS,LOW);
  digitalWrite(CS,HIGH);
  while( 0 == digitalRead(DO));
  digitalWrite(CS,LOW);
  
}

void setup() {
    pinMode(DBG, OUTPUT);
    pinMode(DBG2, OUTPUT);
    
    pinMode(CS, OUTPUT);
    pinMode(SK, OUTPUT);
    pinMode(DI, OUTPUT);
    pinMode(DO, INPUT_PULLUP);
    Serial.begin(SBAUD);
    Serial.print("\nCold Boot\n");
    digitalWrite(CS, LOW);
    digitalWrite(SK, LOW);
    digitalWrite(DI, LOW);
    
}

void buttons()

{

  uint8_t promptText[] =

  {

    /*P*/ /*r*/ /*E*/ /*S*/ /*S*/ /* */ /* */ /* */

    0x73, 0x50, 0x79, 0x6d, 0x6d, 0x00, 0x00, 0x00,

    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /*b*/ /*u*/ /*t*/ /*t*/ /*o*/ /*n*/ /*S*/ /* */

    0x7c, 0x1c, 0x78, 0x78, 0x5c, 0x54, 0x6d, 0x00,

    /* */ /* */ /* */ /* */ /* */ /* */ /* */ /* */

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  };




  static uint8_t block = 0;




  uint8_t textStartPos = (block / 4) << 3;

  for(uint8_t position = 0; position < 8; position++)

  {

    sendCommand(0x44);

    digitalWrite(strobe, LOW);

    shiftOut(data, clock, LSBFIRST, 0xC0 + (position << 1));

    shiftOut(data, clock, LSBFIRST, promptText[textStartPos + position]);

    digitalWrite(strobe, HIGH);

  }




  block = (block + 1) % 16;




  uint8_t buttons = readButtons();




  for(uint8_t position = 0; position < 8; position++)

  {

    uint8_t mask = 0x1 << position;




    setLed(buttons & mask ? 1 : 0, position);

  }

}




uint8_t readButtons(void)

{

  uint8_t buttons = 0;

  digitalWrite(strobe, LOW);

  shiftOut(data, clock, LSBFIRST, 0x42);




  pinMode(data, INPUT);




  for (uint8_t i = 0; i < 4; i++)

  {

    uint8_t v = shiftIn(data, clock, LSBFIRST) << i;

    buttons |= v;

  }




  pinMode(data, OUTPUT);

  digitalWrite(strobe, HIGH);

  return buttons;

}

void loop() {
  int bug=0;
  char ch;
  char ascii;
  char str[16];
  // str[17] = "\0";
  int x;
  int v;
  int r;
  int addr;
  
  Serial.println("\n93c46 Test Menu");
  Serial.println("1 - NVRAM Hex Dump");
  Serial.println("2 - EWEN Write Enable");
  Serial.println("3 - ERAL Erase ALL");
  Serial.println("4 - WRAL 0x6B");
  Serial.println("5 - WRAL 0x24");
  Serial.println("6 - EWDS Write Disable");
  Serial.println("7 - ERASE  0, 3,4, 7");
  Serial.println("8 - WRITE  0:12,34,56,78,9A,BC,DE,FA");
  Serial.println("9 - WRITE 10 : \"Hello World!\"");
  Serial.println("a - WRITE 20 : \"Yes\"");
  Serial.println("b - WRITE 30 : \"Kenny\"");
  Serial.println("c - WRITE SEQUENTIAL ASCII");

 Serial.println("");
  Serial.println("CDM: ");
  while(Serial.available()==0);  // wait for input
  ch= Serial.read();   // Note - Handle CR LF on input

  switch (ch) {
    case '1':
      Serial.println("NVRAM Hex Dump");
      for (r=0; r<8; r++) {
        Serial.print(r);
        Serial.print("0  ");

        for(x=0; x<16; x++){
          addr = x + (r*16);
          v=NV_read(addr);
          if ( v<32 || v>127) {
            ascii = char(46);
          }
          else {
            ascii = char(v);
          }
          str[x] = ascii;
          // if (v < 16) {
          //   Serial.print("0");
          //   Serial.print(v, HEX);
          // }
          // else {
            Serial.print(v, HEX);
          // }
          Serial.print(" ");
        }
        Serial.print("\t");
        Serial.println(str);
      }
      break;
    case '2':
      Serial.print("EWEN Write Enable");
        NV_ewen();
      break;
    case '3':
      Serial.print("ERAL Erase ALL");
        NV_eral();
      break;
    case '4':
      Serial.print("WRAL Write ALL = 0x6B");
        NV_wral(0x6B);
      break;
    case '5':
      Serial.print("WRAL Write ALL = 0x24");
        NV_wral(0x24);
      break;
    case '6':
      Serial.print("EWDS Write Disable");
          NV_ewds();
      break;
    case '7':
      Serial.print("ERASE  0, 3,4, 7");
          NV_erase(0);
          NV_erase(3);
          NV_erase(4);
          NV_erase(7);
      break;
    case '8':
      Serial.print("WRITE  00:22,44,66,88,AA,BB,CC,DD");
        NV_write(0,0x22);
        NV_write(1,0x44);
        NV_write(2,0x66);
        NV_write(3,0x88);
        NV_write(4,0xAA);
        NV_write(5,0xBB);
        NV_write(6,0xCC);
        NV_write(7,0xDD);
      break;
    case '9':
      Serial.print("9 - WRITE 10 : \"Hello World!\" ");
        NV_write(16,0x48);
        NV_write(17,0x65);
        NV_write(18,0x6c);
        NV_write(19,0x6c);
        NV_write(20,0x6f);
        NV_write(21,0x20);
        NV_write(22,0x57);
        NV_write(23,0x6f);
        NV_write(24,0x72); 
        NV_write(25,0x6c);
        NV_write(26,0x64);
        NV_write(27,0x21); 
      break;
    case 'c':
      Serial.print("WRITE SEQUENTIAL ASCII");
      v = 0x00;
      for (x = 0; x < 128; x++) {
        NV_write(x, v++);
      }
      break;
    default:
      break;
    
  }//switch
  digitalWrite(DBG, bug & 1);
  bug++;
}
