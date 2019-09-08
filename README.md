## **MQ135 Library**

This library allows the user to calculate CO2 concentration in air using the MQ135 sensor while correcting for temperature and humidity

The equations used come from "Influence of Temperature and Humidity on the Output Resistance Ratio of the MQ-135 Sensor" published in "International Journal of Computer Science and Software Engineering" by Vandana Kalra, Chaitanya Baweja, Dr Simmarpreet and Supriya Chopra.

## **Calibration**

Before doing anything, using the "setRl(float Rl)", set the load resistance of your specific sensor in kohm (usually 20kohm).

For the calibration, leave the sensor in an open and pollution-free space for 24-48 hours(warming up) (or at least after the warming up, leave it for an hour in the aforementioned space).
Once the 24-48 hours have passed, call the "calculateRo(float PPM)" and pass it the CO2 concentration in the atmosphere (about 400 depending on your area) and remember that value.
Take the value you just got and pass it to the "setRo(float Ro)", this will be the reference resistance used to calculate the PPM.
Now you can call the "calculatePPM()" which will give you the calculated CO2 concentration.

If you have a temperature and humidity sensor too, you can call instead "calculateCorrectedRo(float PPM, float temp, float hum)" and "calculateCorrectedPPM(float temp, float hum)"

## **Exposed functions**

MQ135(int Pin);

void setRo(float Ro);

float getRo();

void setRl(float Rl);

float getRl();

float calculateRo(float PPM);

float calculateRs();

float calculatePPM();

float calculateCorrectedRo(float PPM, float temp, float hum);

float calculateCorrectedRs(float temp, float hum);

float calculateCorrectedPPM(float temp, float hum);



For a basic project check out the example provided