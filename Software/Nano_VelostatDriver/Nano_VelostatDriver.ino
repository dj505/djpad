// uncomment this for 5-panel (PIU) mode
 #define __5_PANEL__ 
// uncomment this for player 2 mode
// #define __PLAYER_2__ 

#include "SerialUtils.h"

// these are the default sensitivity values we'll use, until
// overridden by the controller board
#define DEFAULT_SENSITIVITY 800

// set the serial address
#ifdef __PLAYER_2__
  #define SERIAL_ADDR 0x0B
#else
  #define SERIAL_ADDR 0x0A
#endif

// keep track of the current sensor values and what the thresholds
// are before we trigger a button press
#ifdef __5_PANEL__
  #define NUM_INPUTS 5
  int inputs[] = { A0, A1, A2, A3, A4 };
  int outputs[] = { 2, 3, 4, 5, 6 };
#else
  #define NUM_INPUTS 4
  int inputs[] = { A0, A1, A2, A3, A4 };
  int outputs[] = { 2, 3, 4, 5, 6 };
#endif


// the current thresholds for each arrow
struct THRESHOLD_DATA {
  uint16_t thresholds[NUM_INPUTS];
};

// the current values of each sensor, and the last poll rate
struct PAD_READOUT_DATA {
  uint16_t pressures[NUM_INPUTS];
  uint16_t pollRate;
};

THRESHOLD_DATA thresholdData;
PAD_READOUT_DATA padReadoutData;

// keep track of poll rate
long lastMillis = 0;
int loops = 0;

/**
 * Setup the IO pins
 */
void setup() {
  Serial.begin(115200);

  // init IO pins
  for (int i = 0; i < NUM_INPUTS; i++) {
    pinMode(inputs[i], INPUT_PULLUP);
    pinMode(inputs[i], HIGH);
    pinMode(outputs[i], OUTPUT);
    thresholdData.thresholds[i] = DEFAULT_SENSITIVITY;
  }
}

/**
 * Main program loop, drives the sensors and
 * updates the ESP32 SPI master
 */
void loop() {
  // keep track of our poll rate
  long currentMillis = millis();
  long deltaMillis = currentMillis - lastMillis;
  loops++;

  // every second, update the poll rate and send the readouts to the ESP module
  if (deltaMillis > 200) {
    padReadoutData.pollRate = (loops / (deltaMillis / 1000.0f));

    Serial.print("Current poll rate: ");
    Serial.print(padReadoutData.pollRate);
    Serial.println(" Hz");

    lastMillis = currentMillis;
    loops = 0;

    // update the ESP module
    sendReadouts();
  }

  // check if any of the buttons are pressed
  for (int i = 0; i < NUM_INPUTS; i++) {
    padReadoutData.pressures[i] = analogRead(inputs[i]);

    if (padReadoutData.pressures[i] <= thresholdData.thresholds[i]) {
      digitalWrite(outputs[i], LOW);
    } else {
      digitalWrite(outputs[i], HIGH);
    }
  }

  // check for updates from the ESP modules for new sensitivity thresholds and 
  // apply them if there are any
  recvSerialBytes();
  checkSerialData();
}

/**
 * Send the pad readouts and poll rate to the ESP module.
 */
void sendReadouts() {
    Serial.println("Sending pad readouts to ESP module");
    Serial.write(START_MARKER);

    sendSerialInt(SERIAL_ADDR);

    for (int i = 0; i < NUM_INPUTS; i++) {
      sendSerialInt(padReadoutData.pressures[i]);
    }

    sendSerialInt(padReadoutData.pollRate);
    Serial.write(END_MARKER);
}

/**
 * Receive new threshold values from the ESP module.
 */
void checkSerialData() {
  if (newData) {
    // make sure this message was for our address and not the other player's board
    uint16_t addr = receivedBytes[0] | (receivedBytes[1] << 8);

    // read the thresholds
    if (addr == SERIAL_ADDR) {
      for (int i = 0; i < NUM_INPUTS; i++) {
        thresholdData.thresholds[i] = receivedBytes[(i * 2) + 2] | (receivedBytes[(i * 2) + 3] << 8);
      }
      
      Serial.println("Reading threshold data from serial master");
      Serial.print("New thresholds: ");
      
      for (int i = 0; i < NUM_INPUTS; i++) {
        Serial.print(thresholdData.thresholds[i]);
        Serial.print(" ");
      }
      
      Serial.println("");
    }
    
    newData = false;
  }
 }
