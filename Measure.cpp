#include "Measure.h"

Measure::Measure(int f_pin, Log* f_logger, const char* f_location){
  location = f_location;
  logger = f_logger;
  pin=f_pin;
  dht = new DHT(pin, DHTTYPE);
  dht->begin();
}

void Measure::measureDHT() {
  //delay(2000);
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 
    float humidityBuff = dht->readHumidity();
    float airTempBuff = dht->readTemperature();
    logger->writeLog("DHT read on pin: ", pin, 3);
    logger->writeLog("Air temp: ", airTemp, 2);
    logger->writeLog("Humidity: ", humidity, 2);
    //logger->updateState("airTemp", location, airTemp);
    //logger->updateState("humidity", location, humidity);
    //delay(dht.getMinimumSamplingPeriod());
    
    /*float temps[] = {21.6, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.8, 21.8, 21.8, 21.9, 22, 22, 22, 22, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22.1, 22, 22, 22, 22, 22, 22, 21.9, 21.9, 21.9, 21.9, 21.9, 21.9, 21.9, 21.8, 21.8, 21.8, 21.8, 21.8, 21.8, 21.8, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.7, 21.8, 21.8, 21.8, 21.8, 21.8, 21.8, 21.9, 21.9, 21.9, 21.9, 21.9, 22, 22, 22, 22, 22.1, 22.1, 22.1, 22.1, 22.1, 22.2 };
    airTemp = temps[i];
    humidity = 50;
    i++;
    if(i==sizeof(temps)/sizeof(temps[0]))
      i=0;
      */
    if (isnan(humidityBuff) || isnan(airTempBuff)) {
      logger->writeLog("Failed to read from DHT sensor: ", (float)pin, 1);
      //humidity = 100.0;
      //airTemp = 100.0;
    } else {
      humidity = humidityBuff;
      airTemp = airTempBuff;
    }
  }
}

float Measure::getTemp() {
  return airTemp;
}

float Measure::getHum() {
  return humidity;
}
