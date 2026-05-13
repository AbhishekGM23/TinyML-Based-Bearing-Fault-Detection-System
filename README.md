# TinyML-Based Bearing Fault Detection System

This project is a TinyML-based system for detecting bearing faults using an ADXL345 accelerometer, an ESP32-S3 microcontroller, and a machine learning model trained via Edge Impulse. The system categorizes bearing states into three classes: `off`, `normal`, and `fault`, and provides real-time visual feedback using an RGB LED.

## Repository Structure

### `/Codes`
This directory contains all the source code required to build the project.

- **`/sensor_reading_firmware`**
  - `sensor_reading_firmware.ino`: This firmware is used for **data collection**. It reads x, y, and z-axis acceleration data from the ADXL345 accelerometer (sampled at ~100Hz) and outputs it over the serial port in CSV format. This format is required by the Edge Impulse Data Forwarder to ingest training data.

- **`/bearing_fault1`**
  - `bearing_fault1.ino`: This is the **main application firmware**. It runs the trained Edge Impulse machine learning model locally on the ESP32-S3. It reads real-time data from the accelerometer, runs inferences to classify the current state, and changes the color of the NeoPixel RGB LED according to the result:
    - **Blue**: `off` state
    - **Green**: `normal` state
    - **Red**: `fault` state

- **`ei-bearing_fault1-arduino-1.0.2-impulse-#1.zip`**
  - The exported Edge Impulse Arduino library containing the trained Neural Network (NN) model for bearing fault detection.

### `/Images`
Contains diagrams and images showcasing the system, including block diagrams, hardware details (ESP32-S3-Zero), and screenshots of the Edge Impulse workflow (signal processing, classification, and deployment).

## Hardware Requirements

- **ESP32-S3-Zero**
- **ADXL345 Accelerometer Module**
- Jumper wires and breadboard
- DC Motor / Bearing setup (for actual testing)

### Wiring Guide

| Component | Pin on Component | ESP32-S3 Pin |
| :--- | :--- | :--- |
| ADXL345 | VCC | 3.3V |
| ADXL345 | GND | GND |
| ADXL345 | SDA | GPIO 8 |
| ADXL345 | SCL | GPIO 9 |
| ESP32-S3-Zero | Data In (DIN) | GPIO 21 |
| ESP32-S3-Zero | VCC | 3.3V |
| ESP32-S3-Zero | GND | GND |

## Step-by-Step Instructions to Build from Scratch

### Phase 1: Environment Setup
1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the ESP32 board package in the Arduino IDE (via Boards Manager).
3. Install the required libraries via the Arduino Library Manager:
   - `Adafruit Unified Sensor`
   - `Adafruit ADXL345`
   - `Adafruit NeoPixel`
4. Add the Edge Impulse model to your Arduino IDE:
   - Go to **Sketch** -> **Include Library** -> **Add .ZIP Library...**
   - Select the file `Codes/ei-bearing_fault1-arduino-1.0.2-impulse-#1.zip`.

### Phase 2: Data Collection (Optional if using the pre-trained model)
If you want to train your own model:
1. Open `Codes/sensor_reading_firmware/sensor_reading_firmware.ino` in the Arduino IDE.
2. Select your ESP32-S3 board and the correct COM port.
3. Compile and upload the code.
4. Install the [Edge Impulse CLI](https://docs.edgeimpulse.com/docs/edge-impulse-cli/cli-installation).
5. Open a terminal and run `edge-impulse-data-forwarder`. Follow the prompts to connect your device and start sampling data for different states (`off`, `normal`, `fault`).
6. Train your model on the Edge Impulse Studio and export it as an Arduino Library. Replace the provided `.zip` file with your new one.

### Phase 3: Deployment & Testing
1. Open `Codes/bearing_fault1/bearing_fault1.ino` in the Arduino IDE.
2. Connect your hardware according to the wiring guide.
3. Select your board and COM port.
4. Click **Upload** to flash the code to the ESP32-S3.
5. Once uploaded, open the **Serial Monitor** (set baud rate to `115200`).
6. Observe the predictions. The Serial Monitor will print out the confidence scores for `off`, `normal`, and `fault`.
7. Watch the NeoPixel LED change colors in real-time based on the detected state.
