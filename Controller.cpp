#include "Controller.h"

void callbackController(char* topic, byte* payload, unsigned int length);

Controller::Controller(Thermostat* f_thermostat, Log* f_logger)
{
    /*
     * MIGHT NEED *
    Serial.begin(11520);
    Serial.setTimeout(2000);
    */
    mqtt.setClient(espClient);
    mqtt.setServer("192.168.88.15", 1883);
    randomSeed(micros());
    mqtt.setCallback(callbackController);
    thermostat = f_thermostat;
    logger = f_logger;
}

void Controller::callback(char* topic, byte* payload, unsigned int length)
{
    char buffer[128] = {};
    snprintf(buffer, 128, "Message arrived to %s: ", topic);
    char message_buff[100];
    int i;
    for (i = 0; i < length; i++)
    {
        // Serial.print((char)payload[i]);
        message_buff[i] = payload[i];
    }
    message_buff[i] = '\0';
    logger->writeLog(buffer, message_buff, 2);
    const char* maxAirTempTopic = "thermostat/set/maxAirTemp";
    if (strcmp(topic, maxAirTempTopic) == 0)
    {
        float temperature = atof(message_buff);
        logger->writeLog("Temperature is set to: ", temperature, 1);
        float tempInEEPROM;
        EEPROM.get(thermostat->maxAirTempMemLoc, tempInEEPROM);
        logger->writeLog("Temperature in EEPROM is: ", tempInEEPROM, 3);
        thermostat->setMaxAirTemp(temperature);
        logger->updateState("maxAirTemp", "all", temperature);
        if (temperature != tempInEEPROM)
        {
            EEPROM.put(thermostat->maxAirTempMemLoc, temperature);
            EEPROM.commit();
            logger->writeLog("Wrote temperature to EEPROM: ", temperature, 2);
        }
        else
        {
            logger->writeLog("Set temperature is the same as in EEPROM, no writing happened ", "", 3);
        }
    }
    const char* energySaverTopic = "thermostat/set/energySaver";
    if (strcmp(topic, energySaverTopic) == 0)
    {
        int energySaverOption = 0;
        if(strcmp("on", message_buff)==0) {
          energySaverOption = 1;
        }
        logger->writeLog("Energy saver is set to: ", (float)energySaverOption, 1);
        int energySaverInEEPROM;
        EEPROM.get(thermostat->energySaverMemLoc, energySaverInEEPROM);
        logger->writeLog("EnergySaver in EEPROM is: ", (float)energySaverInEEPROM, 3);
        thermostat->setEnergySaver(energySaverOption);
        if ((float)energySaverOption != energySaverInEEPROM)
        {
            EEPROM.put(thermostat->energySaverMemLoc, energySaverOption);
            EEPROM.commit();
            logger->writeLog("EnergySaver option to EEPROM: ", energySaverOption, 2);
        }
        else
        {
            logger->writeLog(
                "Set EnergySaver option is the same as in EEPROM, no writing happened ", "", 3);
        }
    }
    const char* powerTopic = "thermostat/set/POWER";
    if (strcmp(topic, powerTopic) == 0)
    {
        int powerOption = 0;
        if(strcmp("on", message_buff)==0) {
          powerOption = 1;
        }
        logger->writeLog("Power is set to: ", (float)powerOption, 1);
        int powerOptionInEEPROM;
        EEPROM.get(thermostat->powerMemLoc, powerOptionInEEPROM);
        logger->writeLog("Power in EEPROM is: ", (float)powerOptionInEEPROM, 3);
        thermostat->setPower(powerOption);
        if ((float)powerOption != powerOptionInEEPROM)
        {
            EEPROM.put(thermostat->powerMemLoc, powerOption);
            EEPROM.commit();
            logger->writeLog("Power option to EEPROM: ", powerOption, 2);
        }
        else
        {
            logger->writeLog(
                "Set Power option is the same as in EEPROM, no writing happened ", "", 3);
        }
    }
    const char* logLevelTopic = "thermostat/set/logLevel";
    if (strcmp(topic, logLevelTopic) == 0)
    {
        int logLevelOption = atoi(message_buff);
        if (logLevelOption >= 0 && logLevelOption <= 3)
        {
            logger->writeLog("LogLevel is set to: ", (float)logLevelOption, 1);
            int logLevelOptionInEEPROM;
            EEPROM.get(thermostat->logLevelMemLoc, logLevelOptionInEEPROM);
            logger->writeLog("LogLevel in EEPROM is: ", (float)logLevelOptionInEEPROM, 3);

            logger->setlogLevel(logLevelOption);
            if ((float)logLevelOption != logLevelOptionInEEPROM)
            {
                EEPROM.put(thermostat->logLevelMemLoc, logLevelOption);
                EEPROM.commit();
                logger->writeLog("LogLevel option to EEPROM: ", logLevelOption, 2);
            }
            else
            {
                logger->writeLog(
                    "Set LogLevel option is the same as in EEPROM, no writing happened ", "", 3);
            }
        }
    }
}

void Controller::reconnect()
{
    // Loop until we're reconnected
    while (!mqtt.connected())
    {
        logger->writeLog("Controller is attempting MQTT connection...", "", 1);
        // Create a random client ID
        String clientId = "Thermostat-controller";
        // Attempt to connect
        if (mqtt.connect(clientId.c_str()))
        {
            logger->writeLog("Controller connected to MQTT!", "", 1);
            mqtt.subscribe("thermostat/set/#");
            logger->writeLog("Controller subscribed to thermostat/set/#", "", 1);
        }
        else
        {
            Serial.print("Controller MQTT: failed, rc=");
            float mqttState = (float)mqtt.state();
            logger->writeLog("Controller MQTT: failed, rc=", mqttState, 1);
            logger->writeLog("Controller MQTT: try again next time", "", 1);
        }
    }
}

void Controller::mqttLoop()
{
    if (!mqtt.connected())
    {
        reconnect();
    }
    mqtt.loop();
}
