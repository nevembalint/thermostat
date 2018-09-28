#include "Room.h"
Room::Room(){
}

Room::Room(const char* f_location, int f_relayPin, int f_floorSensorPin, Measure* f_airSensor, float f_maxAirTemp, const uint8_t* f_deviceAddress, Log* f_logger)
{
  relayPin = f_relayPin;
  maxAirTemp = f_maxAirTemp;
  logger = f_logger;
  location = f_location;
  heater = new Heater(location, relayPin, f_floorSensorPin, f_deviceAddress, logger);
  airSensor = f_airSensor;
}

void Room::readTemp() {
  airSensor->measureDHT();
  airTemp = airSensor->getTemp();
  humidity = airSensor->getHum();
  char buffer[32] = {};
  snprintf(buffer, 32, "%f in %s", airTemp, location);
  logger->writeLog("Air temperature is ", buffer, 3);
  buffer[0] = '\0';
  snprintf(buffer, 32, "%f in %s", 50.0f, location);
  logger->writeLog("Humidity is ", buffer, 3);
}

void Room::heatOn() {
  //logger->writeLog("Attempting to turn on heat in ", location, 3);
  
  if (!onState)
  {
    onState = true;
    logger->writeLog("Room turned on: ", location, 1);
    logger->writeLog("Heating up in ", location, 3);
    logger->updateState("heat", location, 1);
    heatingUp = true;
  } else {
    //logger->writeLog("Heat is already on in ", location, 3);
  }
}

void Room::heatOff() {
  //logger->writeLog("Attempting to turn off heat in ", location, 3);
  
  if (onState)
  {
    onState = false;
    logger->writeLog("Room turned off in ", location, 1);
    logger->writeLog("Not heating up in ", location, 3);
    logger->updateState("heat", location, 0);
    heatingUp = false;
  } else {
    //logger->writeLog("Heat is already off in ", location, 3);
  }
}

void Room::handleHeating() {
  if (onState) {
    heater->turnOn();
  } else {
    heater->turnOff();
  }
  heater->handleHeating();
}

Heater* Room::getHeater() {
  return heater;
}

int Room::getRelayPin() {
  return relayPin;
}

float Room::getAirTemp() {
  return airTemp;
}

float Room::getHum() {
  return humidity;
}

float Room::getFloorTemp() {
  return heater->getTemp();
}

float Room::getMaxAirTemp() {
  return maxAirTemp;
}

const char* Room::getLocation() {
  return location;
}

bool Room::isOn() {
  return heater->isOn();
}

bool Room::isHeatingUp() {
  return heatingUp;
}

void Room::setHeatingUp(bool value) {
  heatingUp = value;
}

void Room::setMaxAirTemp(float f_temp) {
  maxAirTemp = f_temp;
}

bool Room::getOnState() {
  return onState;
}
