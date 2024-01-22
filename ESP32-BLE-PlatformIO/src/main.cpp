#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDs for the BLE service and characteristic
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// GPIO pin for the LED
const int ledPin = 2; 

// GPIO pins for Traffic Light Leds
const int redPin = 15; 
const int yellowPin = 4;
const int greenPin = 16;

// GPIO pins for Seven-segment Display
const int segmentA = 17; 
const int segmentB = 5;
const int segmentF = 18;
const int segmentG = 19;
const int segmentE = 21;
const int segmentD = 3; 
const int segmentC = 23;
const int segmentDP = 22;

// BOOT button pins
const int buttonPin = 0; // BOOT button on ESP32 connected to GPIO0

BLECharacteristic *pCharacteristic;
bool buttonState = false;

void setLedColor(const std::string& value) {
    // On-board led
    if (value == "LED_ON") {
        digitalWrite(ledPin, HIGH); 
        Serial.println("LED Turned ON");
        return;
    } else if (value == "LED_OFF") {
        digitalWrite(ledPin, LOW);
        Serial.println("LED Turned OFF");
        return;
    } 

    // Traffic light leds
    if (value == "RED") {
        digitalWrite(redPin, HIGH);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, LOW);
        Serial.println("Red Led Turned ON");
    } else if (value == "YELLOW") {
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, HIGH);
        digitalWrite(greenPin, LOW);
        Serial.println("Yellow Led Turned ON");
    } else if (value == "GREEN") {
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, HIGH);
        Serial.println("Green Led Turned ON");
    }
}

void set7SegmentDisplayNumber(int num) {
    switch (num) {
        case 0:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, LOW);
            digitalWrite(segmentF, LOW);
            digitalWrite(segmentG, HIGH);
            digitalWrite(segmentDP, HIGH);
            break;
        case 1:
            digitalWrite(segmentA, HIGH);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, HIGH);
            digitalWrite(segmentE, HIGH);
            digitalWrite(segmentF, HIGH);
            digitalWrite(segmentG, HIGH);
            digitalWrite(segmentDP, HIGH);
            break;
        case 2:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, HIGH);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, LOW);
            digitalWrite(segmentF, HIGH);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, HIGH);
            break;
        case 3:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, HIGH);
            digitalWrite(segmentF, HIGH);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, HIGH);
            break;
        case 4:
            digitalWrite(segmentA, HIGH);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, HIGH);
            digitalWrite(segmentE, HIGH);
            digitalWrite(segmentF, LOW);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, HIGH);
            break;
        case 5:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, HIGH);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, HIGH);
            digitalWrite(segmentF, LOW);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, HIGH);
            break;
        case 6:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, HIGH);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, LOW);
            digitalWrite(segmentF, LOW);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, LOW);
            break;
        case 7:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, HIGH);
            digitalWrite(segmentE, HIGH);
            digitalWrite(segmentF, HIGH);
            digitalWrite(segmentG, HIGH);
            digitalWrite(segmentDP, HIGH);
            break;
        case 8:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, LOW);
            digitalWrite(segmentF, LOW);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, HIGH);
            break;
        case 9:
            digitalWrite(segmentA, LOW);
            digitalWrite(segmentB, LOW);
            digitalWrite(segmentC, LOW);
            digitalWrite(segmentD, LOW);
            digitalWrite(segmentE, HIGH);
            digitalWrite(segmentF, LOW);
            digitalWrite(segmentG, LOW);
            digitalWrite(segmentDP, LOW);
            break;
        default:
            break;
    }
}

void resetValues() {
    setLedColor("LED_OFF");
    set7SegmentDisplayNumber(0);
    setLedColor("RED");
}


// BLE server callback class for handling BLE events
class MyServerCallbacks: public BLEServerCallbacks {
    // Called when a BLE device connects to the ESP32
    void onConnect(BLEServer* pServer) {
        Serial.println("Device Connected!");
        resetValues(); // Resets LED and display values to their default state
    }

    // Called when a BLE device disconnects from the ESP32
    void onDisconnect(BLEServer* pServer) {
        pServer->startAdvertising(); // Restart advertising to allow new connections
        Serial.println("Device Disconnected!");
    }
};

// BLE characteristic callback class for handling data written to the characteristic
class ESP32Callbacks: public BLECharacteristicCallbacks {
    // Called when data is written to the characteristic
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue(); // Get the written value

        // Log received data
        if (value.length() > 0) {
            Serial.print("Received Data: ");
            Serial.println(value.c_str());

            // Set LED color based on received value
            if (!value.empty()) {
                setLedColor(value);
            }

            // Set the number on the 7-segment display based on received value
            set7SegmentDisplayNumber(atoi(value.c_str()));
        }
    }
};

void setup() {
    Serial.begin(9600);
    // Setup pin modes for LEDs and segments
    pinMode(ledPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(segmentA, OUTPUT);
    pinMode(segmentB, OUTPUT);
    pinMode(segmentC, OUTPUT);
    pinMode(segmentD, OUTPUT);
    pinMode(segmentE, OUTPUT);
    pinMode(segmentF, OUTPUT);
    pinMode(segmentG, OUTPUT);
    pinMode(segmentDP, OUTPUT);

    pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with pull-up resistor

    resetValues(); // Reset LED and display values

    // Initialize BLE device with a unique name
    BLEDevice::init("ESP32_Control");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); // Set server callbacks

    // Create BLE service and characteristic
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                         CHARACTERISTIC_UUID,
                         BLECharacteristic::PROPERTY_READ |
                         BLECharacteristic::PROPERTY_WRITE
                     );

    // Set characteristic callbacks and start the service
    pCharacteristic->setCallbacks(new ESP32Callbacks());
    pCharacteristic->setValue("ESP32 Control Service");
    pService->start();

    // Setup BLE advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(0x1234); 
    pAdvertising->addServiceUUID(SERVICE_UUID); 
    pAdvertising->setScanResponse(true); 
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12); 
    pAdvertising->start(); 

    Serial.println("BLE Device Initialized and Ready");
}

void sendButtonState(bool buttonState) {
    uint8_t val[1];
    val[0] = (uint8_t)buttonState; // Convert button state to byte
    pCharacteristic->setValue(val, 1); // Set the characteristic value
    pCharacteristic->notify(); // Notify connected device of the new value
}

void loop() {
    // Check button state and send its state via BLE
    if (digitalRead(buttonPin) == LOW) { // Button pressed
        if (!buttonState) {
            buttonState = true;
            sendButtonState(buttonState);
        }
    } else { // Button released
        if (buttonState) {
            buttonState = false;
            sendButtonState(buttonState);
        }
    }
}