#include "Heater.h"

Heater::Heater(){
}

Heater::Heater(const char* f_location, int f_relayPin, int f_sensorPin, const uint8_t* f_deviceAddress, Log* f_logger)
{
  // initialize sensor
  relayPin = f_relayPin;
  sensorPin = f_sensorPin;
  deviceAddress = f_deviceAddress;
  OneWire oneWire(sensorPin);
  sensor = DallasTemperature(&oneWire);
  sensor.begin();
  sensor.setResolution(deviceAddress, 12);
  logger = f_logger;
  location = f_location;
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
}

float Heater::readTemp()
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 
    sensor.requestTemperatures();
    float tempC = sensor.getTempC(deviceAddress);
    /*
    float tempC = -127.0f;
    if(relayPin==5) {
      float temps[] = {27.0f, 27.0f, 27.0f, 27.0f, 31.0f, 34.0f, 35.0f, 34.0f, 30.0f, 30.0f, 30.0f, 33.0f};
      tempC = temps[i];
      if(i==sizeof(temps)/sizeof(temps[0]))
         i=0;
    }
    else if (relayPin==4) {
      float temps[] = {27.0f, 31.0f, 34.0f, 35.0f, 34.0f, 32.0f, 32.0f, 30.0f, 33.0f, 35.0f, 34.0f, 30.0f};
      tempC = temps[i];  
      if(i==sizeof(temps)/sizeof(temps[0]))
         i=0;
    }
    i++;
    */
  
    if (tempC == -127.00 || isnan(tempC) || tempC == -7040 || tempC == 85.00)
      {
        logger->writeLog("Failed to read floor sensor in ", location, 2);
        errors++;
        if (errors > 5) {
          temp = 100.0;
          if (errors > 10) {
            //ESP.restart();
          }
        }
        return 100.0;
      }
    else
    {
      temp = tempC;
      char buffer[32] = {};
      snprintf(buffer, 32, "%f in %s", temp, location);
      logger->writeLog("Floor temperature is ", buffer, 2);
      logger->updateState("floorTemp", location, temp);
      return temp;
    }
  }
}

void Heater::relayOn()
{
  if(!isOn())
  {
    digitalWrite(relayPin, LOW);
    logger->writeLog("Relay turned on in ", location, 3);
    logger->updateState("relay", location, 1);
  } else {
    logger->writeLog("Relay is already on in ", location, 3);
  }
}

void Heater::relayOff()
{
  if(isOn())
  {
    digitalWrite(relayPin, HIGH);
    logger->writeLog("Relay turned off in ", location, 3);
    logger->updateState("relay", location, 0);
  } else {
    logger->writeLog("Relay is already off in ", location, 3);
  }
}

void Heater::turnOn()
{
  if(!isOn()) {
    relayOn();
    heatingUp = true;
  logger->writeLog("Heater turned on in ", location, 1);
  }
}

void Heater::turnOff()
{ 
  if(isOn()) {
    relayOff();
    heatingUp = false;
  logger->writeLog("Heater turned off in ", location, 1);
  }
}

bool Heater::isOn() {
  delay(200);
  if (digitalRead(relayPin) == LOW)
    return true;
  else
    return false;
}

bool Heater::wantsToBeOn() {
  if (temp < (maxFloor - offsetFloor)) {
    logger->writeLog("Heater wants to turn on in ", location, 3);
    return true;
  } else if (temp < maxFloor && heatingUp) {
    logger->writeLog("Heater wants to turn on in ", location, 3);
    return true;
  } else if (temp >= maxFloor) {
    logger->writeLog("Heater don't want to turn on in ", location, 3);
    return false;
  } else {
    logger->writeLog("Heater don't want to turn on in ", location, 3);
    return false;
  }
}

float Heater::getTemp() {
  return temp;
}
