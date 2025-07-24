#pragma once
/**
 *   Copyright (C) 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the MIT license. See the LICENSE file for details.
 **/

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <PsychicMqttClient.h>
#include <SecurityManager.h>
#include <MqttBrokerSettingsService.h>
#include <SettingValue.h>
#include <WiFi.h>

#define IDENTIFICATION_SERVICE_PATH "/rest/identify"

class IdentificationService
{
public:
    IdentificationService(PsychicHttpServer *server,
                          ESP32SvelteKit *sveltekit,
                          MqttBrokerSettingsService *mqttBrokerSettingsService);

    void begin();

    bool identify();

private:
    PsychicHttpServer *_server;
    SecurityManager *_securityManager;
    PsychicMqttClient *_mqttClient;
    MqttBrokerSettingsService *_mqttBrokerSettingsService;

    void mqttPublishIdentification(bool session = true);
    void createIdentificationJson(JsonObject root);
    esp_err_t identification(PsychicRequest *request);
};
