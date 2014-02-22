/* 
 MMA8452Q Basic Example Code
 Nathan Seidle
 SparkFun Electronics
 November 5, 2012
 
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 This example code shows how to read the X/Y/Z accelerations and basic functions of the MMA5842. It leaves out
 all the neat features this IC is capable of (tap, orientation, and inerrupts) and just displays X/Y/Z. See 
 the advanced example code to see more features.
 
 Hardware setup:
 MMA8452 Breakout ------------ Arduino
 3.3V --------------------- 3.3V
 SDA -------^^(330)^^------- A4
 SCL -------^^(330)^^------- A5
 GND ---------------------- GND
 
 The MMA8452 is 3.3V so we recommend using 330 or 1k resistors between a 5V Arduino and the MMA8452 breakout.
 
 The MMA8452 has built in pull-up resistors for I2C so you do not need additional pull-ups.
 */

#include <Wire.h> // Used for I2C

#define MMA8452_ADDRESS 0x1C  // 0x1D if SA0 is high, 0x1C if low

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define WHO_AM_I   0x0D
#define CTRL_REG1  0x2A

#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.


// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
  const int numReadings = 2;
  int index = 0;                  // the index of the current reading  
  
  int y_readings[numReadings];      // the readings from the analog input
  float y_total = 0;                  // the running total
  float y_average = 0;                // the average

  int z_readings[numReadings];      // the readings from the analog input
  float z_total = 0;                  // the running total
  float z_average = 0;                // the average

  int x_readings[numReadings];      // the readings from the analog input
  float x_total = 0;                  // the running total
  float x_average = 0;                // the average

void setup()
{
  Serial.begin(57600);
  Serial.println("MMA8452 Basic Example");

  Wire.begin(); //Join the bus as a master

  initMMA8452(); //Test and intialize the MMA8452
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    y_readings[thisReading] = 0;
    z_readings[thisReading] = 0;    
    x_readings[thisReading] = 0;    
  }
  
}

void loop()
{  
 
 
  int accelCount[3];  // Stores the 12-bit signed value
  readAccelData(accelCount);  // Read the x/y/z adc values

  // Now we'll calculate the accleration value into actual g's
  float accelG[3];  // Stores the real accel value in g's
  for (int i = 0 ; i < 3 ; i++)
  {
    accelG[i] = (float) accelCount[i] * 10 / ((1<<12)/(2*GSCALE));  // get actual g value, this depends on scale being set
  }

  // Print out values
  for (int i = 0 ; i < 3 ; i++)
  {
//    Serial.print(accelG[i], 4);  // Print g values
//    Serial.print("\t");  // tabs in between axes
  }
//  Serial.println();

  //float averages[3];
  //float t[3];
  //for (int i = 0 ; i < 3 ; i++)
  //{
  //  
  //}


  // subtract the last reading:
  y_total= y_total - y_readings[index];        
  // read from the sensor:  
  y_readings[index] = accelG[1];
  // add the reading to the total:
  y_total= y_total + y_readings[index];      


  // subtract the last reading:
  z_total= z_total - z_readings[index];        
  // read from the sensor:  
  z_readings[index] = accelG[2];
  // add the reading to the total:
  z_total= z_total + z_readings[index];      

  // subtract the last reading:
  x_total= x_total - x_readings[index];        
  // read from the sensor:  
  x_readings[index] = accelG[0];
  // add the reading to the total:
  x_total= x_total + x_readings[index];      


  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning:
    index = 0;                          

  // calculate the average:
  y_average = y_total / numReadings;        
  z_average = z_total / numReadings;        
  x_average = x_total / numReadings;        
//  Serial.println(total);     
//  Serial.println(numReadings);
//  Serial.println(average);     
  
  if ( y_average >= 9.5 ) {
    Serial.println("SIDE A UP");
  }

  if ( y_average <= -9.0 ) {
    Serial.println("SIDE B UP");
  }

  if ( z_average >= 9.5 ) {
    Serial.println("SIDE C UP");
  }

  if ( z_average <= -9.0 ) {
    Serial.println("SIDE D UP");
  }

  if ( x_average >= 9.0 ) {
    Serial.println("SIDE E UP");
  }

  if ( x_average <= -9.0 ) {
    Serial.println("SIDE F UP");
  }


  delay(100);  // Delay here for visibility
}

void readAccelData(int *destination)
{
  byte rawData[6];  // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, 6, rawData);  // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  for(int i = 0; i < 3 ; i++)
  {
    int gCount = (rawData[i*2] << 8) | rawData[(i*2)+1];  //Combine the two 8 bit registers into one 12-bit number
    gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

    // If the number is negative, we have to make it so manually (no 12-bit data type)
    if (rawData[i*2] > 0x7F)
    {  
      gCount = ~gCount + 1;
      gCount *= -1;  // Transform into negative 2's complement #
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }
}

// Initialize the MMA8452 registers 
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452()
{
  byte c = readRegister(WHO_AM_I);  // Read WHO_AM_I register
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {  
    Serial.println("MMA8452Q is online...");
  }
  else
  {
    Serial.print("Could not connect to MMA8452Q: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }

  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  byte fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active();  // Set to active to start reading
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, bytesToRead); //Ask for bytes, once done, bus is released by default

  while(Wire.available() < bytesToRead); //Hang out until we get the # of bytes we expect

  for(int x = 0 ; x < bytesToRead ; x++)
    dest[x] = Wire.read();    
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, 1); //Ask for 1 byte, once done, bus is released by default

  while(!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one byte
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}

