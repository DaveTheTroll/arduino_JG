void setup() {
  pinMode(2, OUTPUT);
}

void loop() {
  GPOS = 0x0004; // 1 << 2
  GPOC = 0x0004;
  GPOS = 0x0004;
  GPOC = 0x0004;
  GPOS = 0x0004;
  GPOC = 0x0004;
}
