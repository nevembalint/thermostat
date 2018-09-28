#ifndef MEASURE_H
#define MEASURE_H
#include "DHT.h" //https://github.com/markruys/arduino-DHT
#include "Log.h"
#define DHTTYPE DHT22 

class Measure {
  private:
    const char* location;
    Log* logger;
    DHT* dht;
    int lastMeasured = 0;
    int pin;
    float airTemp;
    float humidity;
    int i = 0;
    long previousMillis = 0;
    long interval = 2000;  
  public:
    Measure(int f_pin, Log* f_logger, const char* location);
    void measureDHT();
    float getTemp();
    float getHum();
};

#endif
