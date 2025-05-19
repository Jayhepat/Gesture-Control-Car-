#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;
bool dmpReady = false;
uint8_t devStatus;
uint16_t packetSize;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];

const uint64_t pipeOut = 0xF9E8F0F0E1LL;
RF24 radio(8, 9);

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
} data;

void setupRadioTransmitter() {
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  radio.stopListening();
  data.xAxisValue = 127;
  data.yAxisValue = 127;
}

void setupMPU() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000);
#endif

  mpu.initialize();
  devStatus = mpu.dmpInitialize();

  if (devStatus == 0) {
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.setDMPEnabled(true);
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
}

void setup() {
  setupRadioTransmitter();
  setupMPU();
}

void loop() {
  if (!dmpReady) return;

  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    int xAxisValue = constrain(ypr[2] * 180 / M_PI, -90, 90);
    int yAxisValue = constrain(ypr[1] * 180 / M_PI, -90, 90);

    data.xAxisValue = map(xAxisValue, -90, 90, 0, 254);
    data.yAxisValue = map(yAxisValue, -90, 90, 254, 0);

    radio.write(&data, sizeof(PacketData));
  }
}
