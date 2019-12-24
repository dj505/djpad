// uncomment if you want to run this on an ESP32
// not needed if you're using an ESP8266
// #include <EmbAJAXOutputDriverESPAsync.h>

#include <EEPROM.h>
#include <EmbAJAX.h>

#include "SensorAdjustmentPage.h"
#include "SerialUtils.h"

// constants for storing config values
#define CONFIG_VERSION "v0"
#define CONFIG_START 32
#define __5_PANEL__

// #################################################
// Wifi and webserver configuration
// #################################################

#ifdef __5_PANEL__
  #define WIFI_SSID "Pump It Up Pad - D1 Mini"
#else
  #define WIFI_SSID "Dance Dance Revolution Pad - TEST UNIT"
#endif

#define WIFI_PASSWORD "danceboiz2k19"

// setup web server and register it with EmbAJAX
EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);

// buffers to hold the strings to display on the page for our poll
// rates and sensor values
char p1pollBuf[8], p2pollBuf[8];
char p1a1buf[8], p1a2buf[8], p1a3buf[8], p1a4buf[8], p1a5buf[8], p2a1buf[8], p2a2buf[8], p2a3buf[8], p2a4buf[8], p2a5buf[8];
char p1a1thresh[8], p1a2thresh[8], p1a3thresh[8], p1a4thresh[8], p1a5thresh[8], p2a1thresh[8], p2a2thresh[8], p2a3thresh[8], p2a4thresh[8], p2a5thresh[8];
char* pollbufs[2] = { p1pollBuf, p2pollBuf };
char* displayBufs[2][5] = { { p1a1buf, p1a2buf, p1a3buf, p1a4buf, p1a5buf },
                          { p2a1buf, p2a2buf, p2a3buf, p2a4buf, p2a5buf } };
char* threshBufs[2][5] = { { p1a1thresh, p1a2thresh, p1a3thresh, p1a4thresh, p1a5thresh },
                          { p2a1thresh, p2a2thresh, p2a3thresh, p2a4thresh, p2a5thresh } };
EmbAJAXMutableSpan* pollrateDisplays[2] = { &p1pollrate, &p2pollrate };
EmbAJAXMutableSpan* valueDisplays[2][5] = { { &p1a1display, &p1a2display, &p1a3display, &p1a4display, &p1a5display },
                                         { &p2a1display, &p2a2display, &p2a3display, &p2a4display, &p2a5display } };
EmbAJAXTextInput<BUFLEN>* thresholdInputs[2][5] = { { &p1a1threshold, &p1a2threshold, &p1a3threshold, &p1a4threshold, &p1a5threshold },
                                         { &p2a1threshold, &p2a2threshold, &p2a3threshold, &p2a4threshold, &p2a5threshold } };

// ################################################
// Sensor and serial configuration 
// ################################################

#define SERIAL_ADDR_1 0x0A
#define SERIAL_ADDR_2 0x0B

uint16_t serialAddrs[2] = { SERIAL_ADDR_1, SERIAL_ADDR_2 };

#ifdef __5_PANEL__
  #define NUM_INPUTS 5
#else
  #define NUM_INPUTS 4
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

THRESHOLD_DATA thresholdData[2];
PAD_READOUT_DATA padReadoutData[2];

// allow us to store threshold values in EEPROM
struct ConfigStore {
  char version[4];
  int thresholds[2][5];
} storage = {
  // defaults
  CONFIG_VERSION,
  { { 800, 800, 800, 800, 800 },
    { 800, 800, 800, 800, 800 } }
};

// timer so we can update the attached Arduinos
long lastMillis = 0;

// function prototypes
void initPage();
void handleUpdates();
void loadConfig();
void saveConfig();

/**
   Start the web server and setup comms with the slaves
*/
void setup() {
  // load the saved threshold values so we can send them to the arduinos
  loadConfig();

  // setup a wifi network to connect to this pad ad-hoc
  Serial.begin(115200);
  boolean result = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  
  if (result) {
    Serial.println("Wifi ready!");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("Password: ");
    Serial.println(WIFI_PASSWORD);
  } else {
    Serial.println("Failed to setup the wifi!");
  }

  // setup the EmbAJAX content handlers and start the webserver
  driver.installPage(&page, "/", handleUpdates);
  server.begin();

  initPage();
}

/**
   Main program loop, keeps track of poll rate and updates the UI elements
   with current sensor values.
*/
void loop() {
  // keep track of our poll rate
  long currentMillis = millis();

  // every second, we need to send new sensitivities to the slaves
  if (currentMillis - lastMillis > 200) {
    for (int i = 0; i < 2; i++) {
      sendThresholds(i);
    }

    lastMillis = currentMillis;
  }

  // handle web clients
  driver.loopHook();

  // handle incoming serial data (pad readouts and poll rates)
  recvSerialBytes();
  checkSerialData();
}

/**
 * Send threshold data over serial with the specified player so we can address the packet
 */
void sendThresholds(int player) {
  Serial.println("Sending new thresholds to Arduino slaves");
  Serial.write(START_MARKER);

  uint16_t addr = (player == 0) ? SERIAL_ADDR_1 : SERIAL_ADDR_2;
  sendSerialInt(addr);

  for (int i = 0; i < NUM_INPUTS; i++) {
    sendSerialInt(thresholdData[player].thresholds[i]);
  }
  
  Serial.write(END_MARKER);
}

/**
 * Process incoming serial data from one of the Arduino slaves.
 */
void checkSerialData() {
  if (newData) {
    Serial.println("Receiving serial pad readout data");

    // check which address the message came from
    uint16_t addr = receivedBytes[0] | (receivedBytes[1] << 8);
    int player = (addr == SERIAL_ADDR_1) ? 0 : 1;

    // read the sensor readouts
    for (int i = 0; i < NUM_INPUTS; i++) {
      padReadoutData[player].pressures[i] = receivedBytes[(i * 2) + 2] | (receivedBytes[(i * 2) + 3] << 8);
    }

    // read the poll rate
    padReadoutData[player].pollRate = receivedBytes[(NUM_INPUTS * 2) + 2] | (receivedBytes[(NUM_INPUTS * 2) + 3] << 8);

    // update the UI elements with the new data
    for (int j = 0; j < NUM_INPUTS; j++) {
      (valueDisplays[player][j])->setValue(itoa(padReadoutData[player].pressures[j], displayBufs[player][j], 10));
    }
    
    pollrateDisplays[player]->setValue(itoa(padReadoutData[player].pollRate, pollbufs[player], 10));
    newData = false;
  }
}

/**
   Set the initial values for the page controls.
*/
void initPage() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      (thresholdInputs[i][j])->setValue(itoa(thresholdData[i].thresholds[j], threshBufs[i][j], 10));
    }
  }
}

/**
   Handle updates coming from the client, to change sensitivity thresholds.
*/
void handleUpdates() {
  bool shouldSave = false;

  for (int i = 0; i < 2; i ++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      int newValue = atoi((thresholdInputs[i][j])->value());
  
      // if the incoming value for this arrow is not what we had saved, then store
      // the value and update EEPROM
      if (thresholdData[i].thresholds[j] != newValue) {
        thresholdData[i].thresholds[j] = newValue;
        shouldSave = true;
      }
    }
  }

  if (shouldSave) {
    saveConfig();
  }
}

/**
   Loads the config values from EEPROM.
*/
void loadConfig() {
  EEPROM.begin(512);

  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]) {
    for (unsigned int t = 0; t < sizeof(storage); t++) {
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
    }
  }

  // update the thresholds with what we read from EEPROM
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      thresholdData[i].thresholds[j] = storage.thresholds[i][j];
    }
  }
}

/**
   Save the config values to EEPROM.
*/
void saveConfig() {
  // update the data we're going to store
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      storage.thresholds[i][j] = thresholdData[i].thresholds[j];
    }
  }

  // write the data
  for (unsigned int t = 0; t < sizeof(storage); t++) {
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
  }

  EEPROM.commit();
}
