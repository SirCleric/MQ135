#ifndef MQ135_h
#define MQ135_h
#include "Arduino.h"



class MQ135 {
    
    public:
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

    private:
        float _Rl = 1.0, //Resistor on the MQ135
              _Ro = 1.0, //Known sensor resistance 
              A = 121.4517,
              B = -2.78054;

        int _Pin; //Pin connected to sensor
        float calculateCorrectionFactor(float temp, float hum);

};

#endif