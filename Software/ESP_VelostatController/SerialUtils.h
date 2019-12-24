#ifndef __SERIAL_UTILS_H__
#define __SERIAL_UTILS_H__

#define START_MARKER 0x3C
#define END_MARKER 0x3E
#define SUBSTITUTE_BYTE 0x3D

const byte numBytes = 32;
byte receivedBytes[numBytes];
byte numReceived = 0;
boolean newData = false;
byte convBuf[2];

/**
 * Receive bytes over serial, setting a flag once we've received a whole data message.
 */
void recvSerialBytes() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    byte rb;
   
    while (Serial.available() > 0 && !newData) {
        rb = Serial.read();

        if (recvInProgress) {
            if (rb != END_MARKER) {
                receivedBytes[ndx] = rb;
                ndx++;

                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            } else {
                receivedBytes[ndx] = '\0';
                recvInProgress = false;
                numReceived = ndx;
                ndx = 0;
                newData = true;
            }
        } else if (rb == START_MARKER) {
            recvInProgress = true;
        }
    }
}

/**
 * Sends an integer over serial.
 */
void sendSerialInt(uint16_t val) {
    // make sure we don't send the markers accidentally
    if (val == START_MARKER ||
            val == END_MARKER) {
        val = SUBSTITUTE_BYTE;
    }

    convBuf[0] = val & 0xFF;
    convBuf[1] = (val >> 8)  & 0xFF;

    Serial.write(convBuf, sizeof(convBuf));
}

#endif