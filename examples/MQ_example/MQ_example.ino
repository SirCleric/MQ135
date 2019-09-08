#include <MQ135.h>

//PPM of CO2 in the atmosphere
#define CO2PPM 405.0
#define Ro 10.0

//Init the sensor by calling the function and passing its analog pin
MQ135 mq135(0);
float ppm;

void setup() {

  Serial.begin(9600);

  //You CAN and SHOULD set the reference resistance (aka Ro) in order to get the right Gas concentration
  mq135.setRo(Ro);
}

void loop() {

  //If you need to get the Ro call this function and pass the normal atmospheric concentration of the CO2 in your area
  //once the value has stabilized, change the "Ro" to the new value
  Serial.print("Ro :");
  Serial.println(mq135.mq135.calculateRo(CO2PPM));
  //You can also take into account temperature and humidity
  //Serial.print("Corrected Ro :")
  //Serial.println(mq135.calculateCorrectedRo(CO2PPM,temp,hum));

  //Calculate the PPM based on the Ro
  ppm = mq135.calculatePPM();
  //Calculate the PPM while correcting for temperature and humidity
  //ppm = mq135.calculateCorrectedPPM(temp,hum);
}
