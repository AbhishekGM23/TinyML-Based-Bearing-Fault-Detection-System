#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_NeoPixel.h>
#include <bearing_fault1_inferencing.h>

// Edge Impulse
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

/* ================= I2C CONFIG ================= */
#define I2C_SDA 8
#define I2C_SCL 9

/* ================= RGB LED ================= */
#define LED_PIN 21
#define NUM_LEDS 1

Adafruit_NeoPixel rgb(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ================= SENSOR ================= */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

/* ================= ML BUFFER ================= */
static float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static size_t feature_ix = 0;

/* ================= EI SIGNAL ================= */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

/* ================= SETUP ================= */
void setup() {
    Serial.begin(115200);
    delay(2000);

    Wire.begin(I2C_SDA, I2C_SCL);

    if (!accel.begin()) {
        Serial.println("ADXL345 not detected!");
        while (1);
    }

    accel.setRange(ADXL345_RANGE_2_G);

    rgb.begin();
    rgb.setBrightness(50);
    rgb.clear();
    rgb.show();

    Serial.println("System Ready...");
}

/* ================= LOOP ================= */
void loop() {

    sensors_event_t event;
    accel.getEvent(&event);

    // Fill buffer (x,y,z)
    features[feature_ix++] = event.acceleration.x;
    features[feature_ix++] = event.acceleration.y;
    features[feature_ix++] = event.acceleration.z;

    // Run inference when buffer full
    if (feature_ix >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {

        signal_t signal;
        ei_impulse_result_t result;

        signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        signal.get_data = &raw_feature_get_data;

        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

        if (res != EI_IMPULSE_OK) {
            Serial.println("Inference failed");
            feature_ix = 0;
            return;
        }

        float off = 0, normal = 0, fault = 0;

        Serial.print("Prediction: ");

        // Read all labels
        for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {

            const char* label = result.classification[i].label;
            float value = result.classification[i].value;

            Serial.print(label);
            Serial.print(": ");
            Serial.print(value, 3);
            Serial.print("  ");

            if (strcmp(label, "off") == 0) {
                off = value;
            }
            else if (strcmp(label, "normal") == 0) {
                normal = value;
            }
            else if (strcmp(label, "fault") == 0) {
                fault = value;
            }
        }
        Serial.println();

        /* ================= LED LOGIC ================= */

        if (off > normal && off > fault) {
            // OFF → Blue
            rgb.setPixelColor(0, rgb.Color(0, 0, 255));
            Serial.println("STATE: OFF");
        }
        else if (normal > fault) {
            // NORMAL → Green
            rgb.setPixelColor(0, rgb.Color(255, 0, 0));
            Serial.println("STATE: NORMAL");
        }
        else {
            // FAULT → Red
            rgb.setPixelColor(0, rgb.Color(0, 255, 0));
            Serial.println("STATE: FAULT");
        }

        rgb.show();

        // Reset buffer
        feature_ix = 0;
    }

    // Maintain sampling rate (~100 Hz)
    delay(12);
}