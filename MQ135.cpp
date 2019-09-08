#include "Arduino.h"
#include "MQ135.h"

//Public functions
MQ135::MQ135(int Pin) {
    _Pin = Pin;
}

void MQ135::setRo(float Ro) {
    _Ro = Ro;
}

float MQ135::getRo() {
    return _Ro;
}

void MQ135::setRl(float Rl) {
    _Rl = Rl;
}

float MQ135::getRl() {
    return _Rl;
}

float MQ135::calculateRo(float PPM) {
    return (calculateRs() * (pow(A/PPM, 1/B)));
}

float MQ135::calculateRs() {
    analogRead(_Pin); //Inprecise value
    analogRead(_Pin); //Inprecise value
    return (((1024.0 / (analogRead(_Pin) + 0.5)) - 1.0) * _Rl);
}

float MQ135::calculatePPM() {
    return (A * pow(calculateRs() / _Ro, B));
}

float MQ135::calculateCorrectedRo(float PPM, float temp, float hum) {
    return (calculateCorrectedRs(temp, hum) * (pow(A/PPM, 1/B)));
}

float MQ135::calculateCorrectedRs(float temp, float hum) {
    return (calculateRs() / calculateCorrectionFactor(temp, hum));
}

float MQ135::calculateCorrectedPPM(float temp, float hum) {
    return (A * pow(calculateCorrectedRs(temp, hum) / _Ro, B));
}

//Private functions
float MQ135::calculateCorrectionFactor(float temp, float hum) {
    return (1.6979 - (0.012 * temp) - (0.00612 * hum));
}