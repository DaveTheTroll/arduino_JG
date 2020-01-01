#include <Wire.h>
#include <TimerOne.h>

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18

void i2cread(uint8_t address, uint8_t reg, uint8_t count, uint8_t * data)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(address, count);
  uint8_t j = 0;
  while(Wire.available())
  {
    data[j++] = Wire.read();
  }
}

void i2cwrite(uint8_t address, uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

int led = 1;
long int ti;
volatile bool intFlag = false;
void timer_callback()
{
  intFlag = true;
  digitalWrite(LED_BUILTIN, led ? HIGH : LOW);
  led = !led;
}

void setup()
{
  Wire.begin();
  Serial.begin(115200);

  i2cwrite(MPU9250_ADDRESS, 29, 0x06);                      // accelerometers low pass at 5Hz
  i2cwrite(MPU9250_ADDRESS, 26, 0x06);                      // gyro low pass at 5Hz
  i2cwrite(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);  // gyro range
  i2cwrite(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_4_G);        // accelerometer range
  i2cwrite(MPU9250_ADDRESS, 0x37, 0x02);                    // bypass mode for magnetometers
  i2cwrite(MAG_ADDRESS, 0x0A, 0x16);                        // continuous magnetometer measurements in 16 bits

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Timer1.initialize(500000); // 0.5s
  Timer1.attachInterrupt(timer_callback);

  ti = millis();
}

void loop()
{
  while(!intFlag);
  intFlag = false;

  long int now = millis();
  Serial.print(now - ti, DEC); Serial.print('\t');

  uint8_t buf[14];
  i2cread(MPU9250_ADDRESS, 0x3b, sizeof(buf), buf);  // read accelerometer and gyro

  int16_t ax = (buf[0] << 8) | buf[1];
  int16_t ay = (buf[2] << 8) | buf[3];
  int16_t az = (buf[4] << 8) | buf[5];
  int16_t temperature = (buf[6] << 8) | buf[7];
  int16_t gx = (buf[8] << 8) | buf[9];
  int16_t gy = (buf[10] << 8) | buf[11];
  int16_t gz = (buf[12] << 8) | buf[13];
  
  Serial.print(ax * 4.0 / 32768); Serial.print('\t');
  Serial.print(ay * 4.0 / 32768); Serial.print('\t');
  Serial.print(az * 4.0 / 32768); Serial.print(" :\t");
  Serial.print(temperature, DEC); Serial.print(" :\t");
  Serial.print(gx, DEC); Serial.print('\t');
  Serial.print(gy, DEC); Serial.print('\t');
  Serial.print(gz, DEC); Serial.print(" :\t");

  uint8_t st1;
  do
  {
    i2cread(MAG_ADDRESS, 0x02, 1, &st1);
  }
  while(!(st1 & 0x01));

  uint8_t mag[7];
  i2cread(MAG_ADDRESS, 0x03, sizeof(mag), mag);

  int16_t mx = (mag[1] << 8) | mag[0];
  int16_t my = (mag[3] << 8) | mag[2];
  int16_t mz = (mag[5] << 8) | mag[4];
  
  Serial.print(mx, DEC); Serial.print('\t');
  Serial.print(my, DEC); Serial.print('\t');
  Serial.print(mz, DEC); Serial.print('\t');

  Serial.println();
}
