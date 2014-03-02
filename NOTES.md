# Project Synthcube

## 3D cube design

* [Lockable Box Cube v4](https://www.youmagine.com/designs/lockable-box-cube-v4), [Making a good cube](http://flashgamer.com/arduino/comments/making-a-good-cube), [color cube fades](http://www.flickr.com/photos/flashmagazine/sets/72157629907398057/detail/)

Thingverse?

## Input

Full positioning sensor is obtained by a IMU ([Inertial measurement unit](http://en.wikipedia.org/wiki/Inertial_measurement_unit)]), consisting of

* An accelerometer
* A gyroscope
* A magnetometer

During the jam only a accelerometer was used (MMA8452Q), with basic moving average smoothing and debouncing to correct for irregular input.

The accelerometer can provide different types of input:

* Tapping
* Orientation sensing: either continuos or discrete (e.g. landscape/portrait mode)
* Freefall detection

Currently freefall detection is used to toggle on/off (LED) while orientation is used to transition/set the color of the LED.

### Sensor fusion

Articles:

* [Kalman filter vs Complementary filter](http://robottini.altervista.org/kalman-filter-vs-complementary-filter)
* [Reading a IMU Without Kalman: The Complementary Filter ](http://www.pieter-jan.com/node/11)


### Interesting ICs

#### MMA8452Q Triple Axis Accelerometer

Sparkfun has [example code](https://github.com/sparkfun/MMA8452_Accelerometer).

### ADXL345 3-axis accelerometer  

Arduino libraries: [Adafruit](https://github.com/adafruit/Adafruit_ADXL345), [Python](https://github.com/pimoroni/adxl345-python), [Sparkfun](https://github.com/sparkfun/ADXL345_Breakout)

[ADXL362](https://www.sparkfun.com/products/11446) is a low power variant with wake on shake functionality.

### L3GD20 3-axis Gyroscope

Arduino library: [Arduino library for Pololu L3G boards](https://github.com/pololu/l3g-arduino)

This IC replaces [L3G4200D](http://www.pololu.com/product/1272), and is currently being replaced by [L3GD20H](http://www.pololu.com/product/2129). This last one is not yet easy/cheap to source.

### LSM303DLHC 3 Axis Magnetometer + 3 Axis Accelerometer

Arduino libraries: included in [Pololu-library](https://github.com/pololu/lsm303-arduino) and the [Adafruit library](https://github.com/adafruit/Adafruit_LSM303DLHC).

Latest version of this line is [LSM303D](http://www.pololu.com/product/2127), which is included in the Pololu library, but not yet easy/cheap to source.

## Output: light

* [3mm LED diffuser (Cube and Rouded)](http://www.thingiverse.com/thing:180982)

## Output: sound


## Similar projects

* [GyroSynth](http://www.youtube.com/watch?v=d9t7jMlkuXw): iPhone 4 app that uses internal gyroscope as input for an external synthesiser
