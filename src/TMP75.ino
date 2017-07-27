// Arduino Sketch to read the Texas Instruments
// TMP75 Temperature Sensor
//  It uses the I2C in the Wire Library
//  Written by J.Park jim(_dot_)buzz(_at_)gmail(_dot_)com
//  23 April 2012 reference


void TMP75Initialize(){
  delay(100);
 // Serial.println("Initializing TMP75");
  Wire.begin();                      // Join the I2C bus as a master
  Wire.beginTransmission(TMP75_Address);       // Address the TMP75 sensor
  Wire.write(configReg);                       // Address the Configuration register
  Wire.write(bitConv);                         // Set the temperature resolution
  Wire.endTransmission();                      // Stop transmitting
  Wire.beginTransmission(TMP75_Address);       // Address the TMP75 sensor
  Wire.write(rdOnly);                          // Address the Temperature register
  Wire.endTransmission();                      // Stop transmitting
}


void TMP75Read(){
  temp = readTemp();             // Read the temperature now
}

// Begin the reading the TMP75 Sensor
float readTemp(){
  // Now take a Temerature Reading
  Wire.requestFrom(TMP75_Address,numOfBytes);  // Address the TMP75 and set number of bytes to receive
  byte MostSigByte = Wire.read();              // Read the first byte this is the MSB
  byte LeastSigByte = Wire.read();             // Now Read the second byte this is the LSB

  // Being a 12 bit integer use 2's compliment for negative temperature values
  int TempSum = (((MostSigByte << 8) | LeastSigByte) >> 4);
  // From Datasheet the TMP75 has a quantisation value of 0.0625 degreesC per bit
  float temp = (TempSum*0.0625);
  //Serial.println(MostSigByte, BIN);   // Uncomment for debug of binary data from Sensor
  //Serial.println(LeastSigByte, BIN);  // Uncomment for debug  of Binary data from Sensor
  return temp;                           // Return the temperature value
}
