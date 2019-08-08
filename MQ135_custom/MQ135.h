class MQ135 {
    private:
        float _Rl = 0.0, //Resistor on the MQ135
              _Ro = 0.0, //Known sensor resistance 
              _Rs = 0.0, //Current sensor resistance
              _PPM = 0.0; //Current PPM based on Rs and Ro

        int _Pin; //Pin connected to sensor

    public:

}