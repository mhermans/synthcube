// Output
int redPin = 9;   // Red LED,   connected to digital pin 9
int grnPin = 10;  // Green LED, connected to digital pin 10
int bluPin = 11;  // Blue LED,  connected to digital pin 11

// Color arrays
int black[3]  = { 0, 0, 0 };
int white[3]  = { 100, 100, 100 };
int red[3]    = { 100, 0, 0 };
int green[3]  = { 0, 100, 0 };
int blue[3]   = { 0, 0, 100 };
int yellow[3] = { 40, 95, 0 };
int dimWhite[3] = { 30, 30, 30 };
// etc.

// Set initial color
int redVal = black[0];
int grnVal = black[1]; 
int bluVal = black[2];



int DEBUG = 1;      // DEBUG counter; if set to 1, will write values back via serial

// Initialize color variables
int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;

// Set up the LED outputs
void setup()
{
  pinMode(redPin, OUTPUT);   // sets the pins as output
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT); 

  if (DEBUG) {           // If we want to see values for debugging...
    Serial.begin(9600);  // ...set up the serial ouput 
  }
}

void loop()
{
     analogWrite(redPin, 100);   // Write current values to LED pins
    analogWrite(grnPin, 0);      
    analogWrite(bluPin, 0); 
    
    delay(250);               // wait for a second

     analogWrite(redPin, 0);   // Write current values to LED pins
    analogWrite(grnPin, 0);      
    analogWrite(bluPin, 0); 
    
    delay(250);               // wait for a second

   
 //          Serial.print("Loop/RGB: #");
 //       Serial.print(redVal);
 //       Serial.print(grnVal);
 //       Serial.println(bluVal);  
}
