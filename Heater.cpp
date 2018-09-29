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
    
    /*float tempC = -127.0f;
    if(relayPin==13) {
      float temps[] = {34, 33.25, 33, 33, 33, 32.75, 32.5, 32.5, 32.25, 32.25, 32, 32, 31.75, 31.75, 31.75, 31.5, 31.5, 31.25, 31.25, 31, 31, 31, 30.75, 30.75, 30.75, 31, 31, 31, 31.25, 31.25, 31.5, 31.75, 31.75, 32, 32, 32.25, 32.5, 32.5, 32.75, 32.75, 33, 33, 33.25, 33.25, 33.5, 33.75, 33.75, 34, 34, 34.25, 34.25, 34.25, 34.5, 34.5, 34.75, 34.75, 35, 35, 35, 35, 34.75, 34.75, 34.5, 34.5, 34.25, 34.25, 34, 34, 33.75, 33.5, 33.5, 33.25, 33.25, 33, 33, 32.75, 32.5, 32.5, 32.25, 32.25, 32, 32, 31.75, 31.75, 31.75, 31.5, 31.5, 31.25, 31.25};
      tempC = temps[i];
      if(i==sizeof(temps)/sizeof(temps[0]))
         i=0;
    }
    else if (relayPin==12) {
      float temps[] = {29.75, 30.5, 30.75, 30.75, 31, 31, 31.25, 31.25, 31.5, 31.75, 31.75, 32, 32, 32.25, 32.25, 32.5, 32.5, 32.75, 32.75, 33, 33, 33.25, 33.25, 33.25, 33.25, 33.25, 33.25, 33.25, 33.25, 33.25, 33, 33, 33, 33, 32.75, 32.75, 32.5, 32.5, 32.5, 32.25, 32.25, 32, 32, 32, 31.75, 31.75, 31.75, 31.5, 31.5, 31.5, 31.25, 31.25, 31, 31, 31, 30.75, 30.75, 30.75, 30.75, 30.75, 30.75, 30.75, 31, 31, 31.25, 31.25, 31.5, 31.5, 31.75, 31.75, 32, 32, 32.25, 32.25, 32.5, 32.5, 32.75, 33, 33, 33.25, 33.25, 33.5, 33.5, 33.75, 33.75, 34, 34, 34.25, 34.25};
      tempC = temps[i];  
      if(i==sizeof(temps)/sizeof(temps[0]))
         i=0;
    }
    i++;*/
  
    if (tempC == -127.00 || isnan(tempC) || tempC == -7040 || tempC == 85.00)
      {
        logger->writeLog("Failed to read floor sensor in ", location, 2);
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
  logger->writeLog("Attempting to turn on relay in ", location, 3);
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
  //logger->writeLog("Attempting to turn off relay in ", location, 3);
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
  if(!onState) {
  onState = true;
  logger->writeLog("Heater turned on in ", location, 1);
  }
}

void Heater::turnOff()
{ 
  if(onState) {
  onState = false;
  logger->writeLog("Heater turned off in ", location, 1);
  }
}

void Heater::handleHeating() {
  readTemp();
  if (onState) {
    if (temp < (maxFloor - offsetFloor)) {
      if (!isOn())
        relayOn();
    } else if (temp >= maxFloor) {
      if (isOn())
        relayOff();
    }
  } else if(isOn()) {
    relayOff();
  }
}


bool Heater::isOn() {
  if (digitalRead(relayPin) == LOW)
    return true;
  else
    return false;
}

bool Heater::wantsToTurnOn() {
  if (temp < (maxFloor - offsetFloor))
  {
    logger->writeLog("Heater wants to turn on in ", location, 3);
    return true;
  }
  else {
    logger->writeLog("Heater don't want to turn on in ", location, 3);
    return false;
  }
}

float Heater::getTemp() {
  return temp;
}
