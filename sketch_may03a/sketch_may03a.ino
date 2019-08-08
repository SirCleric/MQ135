static int vccRead(byte us=500) {
  analogRead(6);
  bitSet(ADMUX, 3);
  delayMicroseconds(us);
  bitSet(ADCSRA,ADSC);
  while(bit_is_set(ADCSRA, ADSC))
    ;
  word x = ADC;
  return x ? (1100L * 1023.0) / x : -1;
}

#include <SoftwareSerial.h>

String message; //string that stores the incoming message
SoftwareSerial blueSerial(2,3);
#define FACTOR 0.89
float dustDensity = 0;

#include <Wire.h>

//#include <BMP180.h>

//BMP180 bmp180;

//#include <SFE_BMP180.h>

//SFE_BMP180 pressure;

#define A 300.0

float tmp = 27,atm = 1005.0,hum = 60.0;

char status;
double T,P,p0,a;

float aa = 121.4517;
float bb = -2.78054;

#define SIZE 10

#define Rl 1.0
#define zeroCon 397.13

float Ro = 21.0;
int pin = A0;

int analogValue = 0.0;
float vcc = 0.0;

void getAnalogValue() {
  analogValue = (analogRead(pin) + 0.5);
  analogValue = (analogRead(pin) + 0.5);
}

void getVcc() {
  vcc = (vccRead() / 1000.0);
  vcc = (vccRead() / 1000.0);
}

float Vout() {
  return ((analogValue * vcc) / 1024.0);
}

float Rs() {
  return(((vcc / Vout()) -1.0)* Rl);
}

float RsSempl() {
  return(((1024.0 / analogValue)-1.0)* Rl);
}

float getPPM() {
  return (aa * pow((Rs() / Ro), bb));
}

float getCorrectedPPM(float t,float h) {
  return (aa * pow((getCorrectedRs(t,h) / Ro), bb));
}

float getRo() {
  return(Rs() * pow(aa/zeroCon,1/bb));
}

float getCorrectedRo(float t,float h) {
  return(getCorrectedRs(t,h) * pow(aa/zeroCon,1/bb));
}

float getRs_Scaling_Factor(float t,float h) {
  return (1.6979 - (0.012 * t) - (0.00612 * h));
}

float getCorrectedRs(float t,float h) {
  return(Rs() / getRs_Scaling_Factor(t,h));
}

float rZeroArray[SIZE] = {0};
float ppmArray[SIZE] = {0};

float rzero = 0,tempRZero = 0,meanRZero = 0,PPM = 0,tempPPM = 0,meanPPM = 0;
int i;

 
void setup() {
 Serial.begin(9600);
 Wire.begin();
  
 blueSerial.begin(9600);
}

void loop() {

  //tmp = bme.temp();
  //bmp180.getTemperature();
  //hum = bme.readHumidity();
  //atm = bme.readPressure();


  tempRZero = 0;
  tempPPM = 0;
  
  for (i=0;i<SIZE;i++) {
    
    rzero = getCorrectedRo(tmp,hum);
    rZeroArray[i] = rzero;
    tempRZero += rzero;

    PPM = getCorrectedPPM(tmp,hum);
    ppmArray[i] = PPM;
    tempPPM += PPM;
    
    delay(2500 / SIZE);
  }
    
  //rzero = getCorrectedRo(tmp,hum);
  //PPM = getCorrectedPPM(tmp,hum);
  
  //meanRZero = rzero;// / SIZE;
  //meanPPM = PPM;// / SIZE;

  meanRZero = tempRZero / SIZE;
  meanPPM = tempPPM / SIZE;
  
  Serial.print("RZero: ");
  Serial.println(meanRZero);
  Serial.print("PPM: ");
  Serial.println(meanPPM);
  Serial.print("TMP: ");
  Serial.println(tmp);

  Serial.println("--------------------------------------------");
  getAnalogValue();
  getVcc();
  Serial.print("Vcc: ");
  Serial.println(vcc);
  Serial.print("Analog: ");
  Serial.println(analogValue);
  Serial.println("--------------------------------------------");
  Serial.print("RsWithoutSemp: ");
  Serial.println(Rs());
  Serial.println("--------------------------------------------");
  Serial.print("RsWithSemp: ");
  Serial.println(RsSempl());
  Serial.println("--------------------------------------------");

  

  while(blueSerial.available())
  {//while there is data available on the serial monitor
    message+=char(blueSerial.read());//store string from serial command
  }

  while(blueSerial.available())
  {//while there is data available on the serial monitor
    message+=char(blueSerial.read());//store string from serial command
  }
  if(!blueSerial.available())
  {
    if(message.substring(0,3) == "ANA")
    {//if data is available
      blueSerial.println("Here"); //show the data
      message=""; //clear the data
    }else if (message.substring(0,3) == "CRO") {
      //Ro = message.substring(3).toFloat();
      Ro = meanRZero;
      
      message="";
    } else {
      blueSerial.print("REF");
      blueSerial.print(tmp);
      blueSerial.print(",");
      blueSerial.print(hum);
      blueSerial.print(",");
      blueSerial.print(atm);
      blueSerial.print(",");
      blueSerial.print(meanPPM);
      blueSerial.print(",");
      blueSerial.print(dustDensity,2);
      blueSerial.print(",");
      blueSerial.print(dustDensity / FACTOR,2);
      blueSerial.print(",");
      blueSerial.print(";");
      Serial.print("REF");
      Serial.print(tmp);
      Serial.print(";");
      Serial.print(hum);
      Serial.print(";");
      Serial.print(atm);
      Serial.print(";");
      Serial.print(meanPPM);
      Serial.print(";");
      Serial.print(dustDensity,2);
      Serial.print(";");
      Serial.print(dustDensity / FACTOR,2);
      Serial.print(";");

      message = "";
      
    }    
  }

  delay(1000);
}
