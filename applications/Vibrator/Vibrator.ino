
const int vib_pin = 13;
const int vib_pwm_freq = 5000;
const int vib_channel = 0;
const int vib_resolution = 8;

void setup() {
  Serial.begin(9600);

  ledcSetup(vib_channel, vib_pwm_freq, vib_resolution);
  ledcAttachPin(vib_pin, vib_channel);

  Serial.println("Vibrator Test");
}

void loop() {
  Serial.print(".");
  if (Serial.available() > 0)
  {
    int val = Serial.parseInt();

    if (val > 0)
    {
      Serial.print("> "); Serial.println(val, DEC);
      ledcWrite(vib_channel, val-1);
    }
  }
}
