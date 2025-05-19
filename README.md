
# Gesture Controlled Car

This project demonstrates a gesture-controlled robotic car that responds to hand movements using an MPU6050 sensor and wireless communication via NRF24L01 modules. It showcases an intuitive, touch-free interface to control a 4WD vehicle by translating real-time hand gestures into directional commands. Built with Arduino Nano, the system includes both a transmitter (gesture input) and a receiver (motor controller) unit. The aim is to enhance human-machine interaction, especially for accessibility and potential use in automation, assistive technologies, and hazardous environments.

## Hardware Components

- **Arduino Nano** – Microcontroller board for both transmitter and receiver.
- **MPU6050 Module** – 3-axis accelerometer and gyroscope for gesture detection.
- **NRF24L01+ Module** – Wireless transceiver for communication between hand unit and car.
- **NRF Adapter** – Voltage regulator for safely connecting the NRF24L01 to Arduino.
- **L298N Motor Driver** – Controls the motors based on received signals.
- **DC Motors (4x)** – For driving the car in all directions (4WD).

##  System Design

The system consists of two main parts: a **transmitter unit** (gesture controller) and a **receiver unit** (car base).

### Transmitter Unit:
- Detects hand gestures using the **MPU6050** sensor.
- Sends gesture data wirelessly through the **NRF24L01+ module**.

### Receiver Unit:
- Receives gesture data via the NRF module.
- Processes the data using **Arduino Nano**.
- Controls the **DC motors** through the **L298N Motor Driver** to move the car in the desired direction.

### Data Flow Summary:
1. **Hand Gesture** → Detected by MPU6050  
2. **Gesture Data** → Transmitted via NRF24L01  
3. **Receiver Unit** → Decodes data and sends signals to motors  
4. **Motors** → Rotate/move based on gesture

## How It Works

1. The user wears a glove or holds a device with an **MPU6050** sensor that detects hand gestures (tilt forward, backward, left, right).

2. These movements are translated into **X and Y axis values** using onboard processing.

3. The data is wirelessly transmitted to the car using an **NRF24L01+ module** connected to an **Arduino Nano**.

4. On the receiver side, another **Arduino Nano** receives the gesture data and interprets it.

5. The interpreted signals are sent to an **L298N Motor Driver**, which controls the **DC motors**.

6. The car moves **forward, backward, left, or right** based on the detected hand gesture direction.

## Installation and Setup

**Transmitter Side (MPU6050 + NRF24L01)**
1.  MPU6050 Initialization
```bash
mpu.initialize();
devStatus = mpu.dmpInitialize();

```
2. Gesture Reading (Yaw, Pitch, Roll)
```bash
mpu.dmpGetQuaternion(&q, fifoBuffer);
mpu.dmpGetGravity(&gravity, &q);
mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

```
3.  Gesture Mapping to Values
```bash
int xAxisValue = constrain(ypr[2] * 180/M_PI, -90, 90);
int yAxisValue = constrain(ypr[1] * 180/M_PI, -90, 90);
data.xAxisValue = map(xAxisValue, -90, 90, 0, 254);
data.yAxisValue = map(yAxisValue, -90, 90, 254, 0);

```
4. Data Transmission via RF
```bash
radio.write(&data, sizeof(PacketData));
```
**Receiver Side (NRF24L01 + Motor Driver)**
1. Data Reception
```bash
radio.read(&receiverData, sizeof(PacketData));
```
2. Gesture to Motor Speed Mapping
```bash
int mappedYValue = map(receiverData.yAxisValue, 0, 254, -255, 255);
int mappedXValue = map(receiverData.xAxisValue, 0, 254, -255, 255);

```
 3. Motor Speed Calculation
```bash
rightMotorSpeed = abs(mappedYValue) - mappedXValue;
leftMotorSpeed = abs(mappedYValue) + mappedXValue;
```
4.  Motor Control Logic
```bash
rotateMotor(rightMotorSpeed * motorDirection, leftMotorSpeed * motorDirection);
```

## Results
 Results:
1. **Successful Communication** → Stable gesture data transmission via NRF24L01

2. **Accurate Movement** → Car responded correctly to forward, backward, left, and right gestures

3. **Minimal Latency** → Real-time control with negligible delay between gesture and movement

4. **Reliable Operation** → System functioned consistently after multiple tests

5. **Improved Usability** → Hands-free control achieved using intuitive hand movements
## Conclusion
The Gesture Controlled Car project successfully demonstrates the practical application of hand gesture recognition for vehicle control. By integrating affordable components like the MPU6050, NRF24L01, and Arduino Nano, we created a cost-effective prototype that enables intuitive, hands-free control. The system performed reliably during testing, responding accurately to directional gestures with minimal latency. This project highlights the potential for assistive technologies that can aid individuals with physical disabilities, offering an accessible and user-friendly interface. Moreover, it sets a foundation for future advancements in human-machine interaction, particularly in automotive and robotics applications.