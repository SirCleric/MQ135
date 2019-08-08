#include <DS1307.h>
#include <Wire.h>

DS1307 clock;
//---------------------------------------|Bluetooth|--------------------------------

#include <SoftwareSerial.h>

String message; //string that stores the incoming message
SoftwareSerial blueSerial(2,3);

//---------------------------------------|SD|---------------------------------------

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

File dataPtr;
File settingsPtr;

char _buffer[20];

//---------------------------------------|CO2|--------------------------------------

float aa = 121.4517;
float bb = -2.78054;

#define SIZE 25

#define Rl 1.0
#define zeroCon 397.13

float Ro = 16.0;
int pin = A0;

float Rs() {
  analogRead(pin); //discard to stabilize value
  return((1023 / analogRead(pin)) - 1.0) * Rl;
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

//---------------------------------------|Dust|---------------------------------------

#define FACTOR 0.89

int measurePin = 1;
int ledPower = 8;
 
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

//---------------------------------------|ATM && TMP|---------------------------------------

#include <BMP180.h>

BMP180 bmp180;

//#include <SFE_BMP180.h>

//SFE_BMP180 pressure;

#define A 300.0

float tmp = 0,atm = 0;

char status;
double T,P,p0,a;

//---------------------------------------|HUM && TMP|---------------------------------------

#include <SimpleDHT.h>
int dhtPin = 9;
SimpleDHT22 dht(dhtPin);

float hum = 0.0;
float tmp2 = 0.0;

//---------------------------------------|Setup|---------------------------------------

void setup() {

  Wire.begin();
  
  Serial.begin(9600);
  bmp180.init();

  clock.begin();
  clock.fillByYMD(2019,3,9);//Jan 19,2013
  clock.fillByHMS(21,41,00);//15:28 30"
  clock.fillDayOfWeek(SAT);//Saturday
  clock.setTime();//write time to the RTC chip

  blueSerial.begin(9600);
  
  pinMode(ledPower,OUTPUT);

  if (SD.begin(4)) {
    if (SD.exists("settings.txt")) {
      Serial.println("Loading settings...");
      settingsPtr = SD.open("settings.txt", FILE_READ);
      Ro = settingsPtr.parseFloat();
      settingsPtr.close();
      Serial.println("Done");
      Serial.print("Current value Ro: ");
      Serial.println(Ro);
      Serial.println("----------------------------");
    }else {
      Serial.println("Generating settings...");
      settingsPtr = SD.open("settings.txt", FILE_WRITE);
      settingsPtr.write(dtostrf(Ro, 4, 2, _buffer));
      settingsPtr.close();
      Serial.println("Done");
      Serial.println("----------------------------");
    }
  }else {
    Serial.println("Could not load SD card...");
    Serial.println("----------------------------");
  }
}

void loop() {

  clock.getTime();

  Serial.println(clock.hour);
  
  //---------------------------------------|pm 2.5|---------------------------------------
  
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  analogRead(measurePin); //discard to stabilize value
  voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
 
  calcVoltage = voMeasured * (5.0 / 1024.0);
 
  dustDensity = (0.17 * calcVoltage) * 1000.0;

 
  /*Serial.print("Dust Density(2.5): ");
  Serial.println(dustDensity,2);
  Serial.print("Dust Density(10.0): ");
  Serial.println(dustDensity / FACTOR,2);*/
  
  //---------------------------------------|ATM && TMP|---------------------------------------

  tmp = bmp180.getTemperature();
  atm = bmp180.getPressure();

  atm = atm/pow(1-(A/44330.0),5.255);

  /*Serial.print("Temperature: ");
  Serial.print(tmp);
  Serial.println(" C");
  
  Serial.print("Pressure: ");
  Serial.print(atm);
  Serial.println(" hPa");*/

  //---------------------------------------|HUM && TMP|---------------------------------------
  dht.read2(dhtPin,&tmp2,&hum,NULL);
  Serial.print((float)hum); 
  Serial.print("%");
  Serial.print((float)tmp2);
 
  //---------------------------------------|CO2|---------------------------------------
  
  tempRZero = 0;
  tempPPM = 0;
  
  for (i=0;i<SIZE;i++) {
    
    rzero = getCorrectedRo(tmp,hum);
    rZeroArray[i] = rzero;
    tempRZero += rzero;

    PPM = getCorrectedPPM(tmp,hum);
    ppmArray[i] = PPM;
    tempPPM += PPM;
    
    delay(10000 / SIZE);
  }

Serial.println("ok");
  
  meanRZero = tempRZero / SIZE;
  meanPPM = tempPPM / SIZE;
  
  
  /*Serial.print("meanRZero: ");
  Serial.print(meanRZero);
  Serial.print("\n");
  
  Serial.print("meanPPM: ");
  Serial.print(meanPPM);
  Serial.print("\n\n");
  

  Serial.print("RZERO: ");
  Serial.print(rzero);
  Serial.print("\n");
  
  Serial.print("PPM: ");
  Serial.print(PPM);
  Serial.print("\n");
  
  Serial.println("----------------------------");*/

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

      if (SD.exists("settings.txt")) {
        
        Serial.println("Saving settings...");
        SD.remove("settings.txt");
        settingsPtr = SD.open("settings.txt", FILE_WRITE);
        settingsPtr.write(dtostrf(Ro, 4, 2, _buffer));
        settingsPtr.close();
        Serial.println("Done");
        
      }else {
        Serial.println("Could not save the new settings...");
      }
      
      message="";
    } else {
      Serial.println("ok");
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
      Serial.println("ok");

      message = "";
      
    }    
  }
/*
  dataPtr = SD.open("data.txt", FILE_WRITE);
  
  dataPtr.print("Temperature: ");
  dataPtr.print(tmp);
  dataPtr.println(" C");
  dataPtr.print("Humidity: ");
  dataPtr.print(hum);
  dataPtr.println(" %");
  dataPtr.print("Pressure: ");
  dataPtr.print(atm);
  dataPtr.println(" hPa");
  dataPtr.print("meanRZero: ");
  dataPtr.println(meanRZero);   
  dataPtr.print("meanPPM: ");
  dataPtr.println(meanPPM);      
  dataPtr.print("Dust Density: ");
  dataPtr.println(dustDensity,2);

  dataPtr.println("----------------------------"); 

  dataPtr.close();*/
}
