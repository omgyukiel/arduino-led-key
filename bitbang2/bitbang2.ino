#define SBAUD 115200

#define STB 10
#define CLK 9
#define DIO 8

void setup() {
  pinMode(STB, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT); // set peripheral to read at first

  Serial.begin(SBAUD);
  Serial.print("\nCold Boot\n");

  digitalWrite(CLK, LOW);
  digitalWrite(DIO, LOW);

}

void loop() {
  Serial.println("\nTM1638 Start\n");
  
}
