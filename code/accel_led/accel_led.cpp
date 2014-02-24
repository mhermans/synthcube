/*
# ################ #
# SYNTCUBE ROUTINE #
# ################ #

MMA8452Q routines based on example code by Nathan Seidle
(SparkFun Electronics, November 5, 2012)

Hardware setup:

MMA8452 Breakout ------------ Arduino
 3.3V --------------------- 3.3V
 SDA -------^^(330)^^------- A4
 SCL -------^^(330)^^------- A5
 GND ---------------------- GND

The MMA8452 is 3.3V so we recommend using 330 or 1k resistors between
a 5V Arduino and the MMA8452 breakout.The MMA8452 has built in pull-up resistors
for I2C so you do not need additional pull-ups.

*/

// Do not remove the include below
#include "playroutine.h"
#include <Wire.h> // Used for I2C

// Pre-defining functions
void readAccelData(int *destination);
void writeRegister(byte addressToWrite, byte dataToWrite);
void initMMA8452();
void MMA8452Standby();
void MMA8452Active();
void readRegisters(byte addressToRead, int bytesToRead, byte * dest);
byte readRegister(byte addressToRead);
void writeRegister(byte addressToWrite, byte dataToWrite);
void setColor(int red, int green, int blue);


// Select arcellometer, 0x1D if SA0 is high, 0x1C if low
#define MMA8452_ADDRESS 0x1C

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define WHO_AM_I   0x0D
#define CTRL_REG1  0x2A

// Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
#define GSCALE 2

// Output
int redPin = 9;   // Red LED,   connected to digital pin 9
int greenPin = 10;  // Green LED, connected to digital pin 10
int bluePin = 11;  // Blue LED,  connected to digital pin 11


// vars for smoothing
// ------------------

const int numReadings = 50;
int index = 0;                  // the index of the current reading

int y_readings[numReadings];      // the readings from the analog input
float y_total = 0;                  // the running total
float y_average = 0;                // the average

int z_readings[numReadings];
float z_total = 0;
float z_average = 0;

int x_readings[numReadings];
float x_total = 0;
float x_average = 0;

int averages[3];

// vars for debounced freefall detection
// -------------------------------------

int freefalled = 0;
int LEDState = 0;         // the state the led should be: if 1, colors will be set
int fallState = 0;             // the current reading from the freefall detection
int lastFallState = 0;   // the previous reading from the freefall detection

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the freefall was measured/triggered
long debounceDelay = 20;    // the debounce time




// vars for blinking
// -----------------

//int blinking = 1;
//long previousMillis = 0;        // will store last time LED was updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
//long interval = 1000;           // interval at which to blink (milliseconds)



void setup()
{
	pinMode(redPin, OUTPUT);   // sets PWD pins as output for LED
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);

	Serial.begin(57600);
	Serial.println("Start MMA8452 routine");

	Wire.begin(); //Join the bus as a master
	initMMA8452(); //Test and intialize the MMA8452


	// set all readings to 0
	for (int thisReading = 0; thisReading < numReadings; thisReading++) {
		y_readings[thisReading] = 0;
		z_readings[thisReading] = 0;
		x_readings[thisReading] = 0;
	}


}


void loop()
{

	// READ ACCELERATION OBSERVATIONS
	// ==============================

	int accelCount[3];  // Stores the 12-bit signed value
	readAccelData(accelCount);  // Read the x/y/z adc values

	// Now we'll calculate the accleration value into actual g's
	float accelG[3];  // Stores the real accel value in g's
	float conversion[3];  // Stores the real accel value in g's
	int colorValues[3];

	for (int i = 0 ; i < 3 ; i++)
	{
		// get actual g value, this depends on scale being set
		// multiplied by 10 for convenience, gives resting range [-10, 10]
		accelG[i] = (float) accelCount[i] * 10  / ((1<<12)/(2*GSCALE));
	}


	// SMOOTHING OF ACCELERATION OBSERVATIONS USING AVERAGES
	// =====================================================

	x_total= x_total - x_readings[index];
	x_readings[index] = accelG[0];
	x_total= x_total + x_readings[index];

	y_total= y_total - y_readings[index];
	y_readings[index] = accelG[1];
	y_total= y_total + y_readings[index];

	z_total= z_total - z_readings[index];
	z_readings[index] = accelG[2];
	z_total= z_total + z_readings[index];

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

	averages[0] = x_average;
	averages[1] = y_average;
	averages[2] = z_average;


	// CONVERT ACCELLERATION OBSERVATIONS TO RGB VALUES
	// ================================================

	for (int i = 0 ; i < 3 ; i++)
	{
		// Convert to color values in the 0-255 range
		// abs() puts a single "full" colour at each extreme for an axis,
		// intermediate positions give a mix.
		conversion[i] = (float) abs(averages[i]); // abs -10, 10 => (10, 0, 10)
		colorValues[i] = (int) floor(conversion[i]);
		colorValues[i] = map(colorValues[i], 0, 10, 0, 255); // (10, 0, 10) -> (0, 255)

	}



	// DETECT FREEFALL BASED ON SMOOTHED OBSERVATIONS
	// ==============================================

	if ( x_average <= -13.0 | x_average >= 13) {
		Serial.println("FREEFALL ON X-AXIS");
		freefalled = 1;
	}

	if ( y_average <= -13.0 | y_average >= 13) {
		Serial.println("FREEFALL ON Y-AXIS");
		freefalled = 1;
	}

	if ( z_average <= -13.0 | z_average >= 13) {
		Serial.println("FREEFALL ON Z-AXIS");
		freefalled = 1;
	}


	// SET LED ON/OFF COLORS
	// =====================

	if (LEDState == 1) {

		// blinking works, but cant use a delay(),
		// readings & smoothing needs fast main loop.
		/*if (blinking == 1) {
			setColor(0,0,0);
			delay(200);
			setColor(colorValues[0], colorValues[1], colorValues[2]);
			delay(200);
		}*/

		// set the actual colors/LED-output
		setColor(colorValues[0], colorValues[1], colorValues[2]);
		//setColor(0,255,255);
	}
	if (LEDState == 0) {
		// turn of LED
		setColor(0,0,0);
	}



	// DEBOUNCE ROUTINE FOR FREEFALL DETECTION
	// =======================================

	// read the freefall occurence observation into a local variable:
	int reading = freefalled;

	// If the switch changed, due to noise or pressing:
	if (reading != lastFallState) {
		// reset the debouncing timer
		lastDebounceTime = millis();
	}

	if ((millis() - lastDebounceTime) > debounceDelay) {
		// whatever the reading is at, it's been there for longer
		// than the debounce delay, so take it as the actual current state:

		// if the button state has changed:
		if (reading != fallState) {
			fallState = reading;



			// only toggle the LED if the new button state is HIGH
			if (fallState == HIGH) {
				LEDState = !LEDState;
			}
		}
	}

	// save the current reading.  Next time through the loop,
	// it'll be the lastFallState:
	lastFallState = reading;



	//freefalled  = 1;
	freefalled  = 0; // reset freefall observation, change to 1 for testing LED


	//*
	// Print out values
	for (int i = 0 ; i < 3 ; i++)
	{
		Serial.print(accelG[i], 4);  // Print g values
		Serial.print("\t");  // tabs in between axes
	}

	Serial.print(" --> ");
	for (int i = 0 ; i < 3 ; i++)
	{
		Serial.print(colorValues[i]);  // Print color values
		Serial.print("\t");  // tabs in between axes
	}
	Serial.println();
	//*/



}

void setColor(int red, int green, int blue)
{
	analogWrite(redPin, red);
	analogWrite(greenPin, green);
	analogWrite(bluePin, blue);
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


