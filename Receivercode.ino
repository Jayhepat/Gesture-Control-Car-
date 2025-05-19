#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define SIGNAL_TIMEOUT 500  // Timeout in milliseconds to stop motors if no signal

const uint64_t pipeIn = 0xF9E8F0F0E1LL;  // Communication pipe
RF24 radio(8, 9);  // CE and CSN pins for nRF24L01
unsigned long lastRecvTime = 0;

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
} receiverData;

// Right motor pins
int enableRightMotor = 5;
int rightMotorPin1 = 2;
int rightMotorPin2 = 3;

// Left motor pins
int enableLeftMotor = 6;
int leftMotorPin1 = 4;
int leftMotorPin2 = 7;

void setup() {
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  rotateMotor(0, 0);  // Stop motors at start

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipeIn);
  radio.startListening();  // Start receiving

#ifdef PRINT_DEBUG
  Serial.begin(115200);
#endif
}

void loop() {
  int rightMotorSpeed = 0;
  int leftMotorSpeed = 0;

  if (radio.isChipConnected() && radio.available()) {
    radio.read(&receiverData, sizeof(PacketData));

    int mappedYValue = map(receiverData.yAxisValue, 0, 254, -255, 255);
    int mappedXValue = map(receiverData.xAxisValue, 0, 254, -255, 255);
    int motorDirection = (mappedYValue < 0) ? -1 : 1;

    rightMotorSpeed = abs(mappedYValue) - mappedXValue;
    leftMotorSpeed = abs(mappedYValue) + mappedXValue;

    rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);
    leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);

    rotateMotor(rightMotorSpeed * motorDirection, leftMotorSpeed * motorDirection);
    lastRecvTime = millis();

#ifdef PRINT_DEBUG
    Serial.println(receiverData.xAxisValue);
    Serial.println(receiverData.yAxisValue);
#endif
  } else {
    // Stop motors if no signal for defined timeout
    unsigned long now = millis();
    if (now - lastRecvTime > SIGNAL_TIMEOUT) {
      rotateMotor(0, 0);
    }
  }
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  // Right motor control
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }

  // Left motor control
  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
  }

  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));
}
