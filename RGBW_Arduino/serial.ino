void setup_serial() {
  #ifdef SERIAL_DEBUG
    // initialize serial communication:
    while (!Serial);  // for Leonardo/Micro/Zero
    Serial.begin(74880);
    debug("Starting.");
    delay(100);
  #endif
}

void debug (String msg) {
  #ifdef SERIAL_DEBUG
    Serial.println(msg);
  #endif
}
