/**
 *   Copyright (C) 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the MIT license. See the LICENSE file for details.
 **/

#include <IdentificationService.h>

IdentificationService::IdentificationService(PsychicHttpServer *server,
                                             ESP32SvelteKit *sveltekit,
                                             MqttBrokerSettingsService *mqttBrokerSettingsService) : _server(server),
                                                                                                     _securityManager(sveltekit->getSecurityManager()),
                                                                                                     _mqttClient(sveltekit->getMqttClient()),
                                                                                                     _mqttBrokerSettingsService(mqttBrokerSettingsService)
{
}

void IdentificationService::begin()
{
    _server->on(IDENTIFICATION_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&IdentificationService::identification, this, std::placeholders::_1),
                                              AuthenticationPredicates::IS_AUTHENTICATED));

    // publish the identification on every change of a broker setting
    _mqttBrokerSettingsService->addUpdateHandler([&](const String &originId)
                                                 { identify(); },
                                                 false);

    // publish identification on connect
    _mqttClient->onConnect(std::bind(&IdentificationService::mqttPublishIdentification, this, std::placeholders::_1));
}

void IdentificationService::mqttPublishIdentification(bool session)
{
    identify();
}

bool IdentificationService::identify()
{
    if (!_mqttClient->connected())
    {
        return false;
    }

    // create JSON object
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    createIdentificationJson(root);

    String payloadString;
    serializeJson(doc, payloadString);

    // get the identification topic
    String identificationTopic;
    _mqttBrokerSettingsService->read([&](MqttBrokerSettings &settings)
                                     { identificationTopic = settings.identificationTopicPub; });

    // publish the identification
    _mqttClient->publish(identificationTopic.c_str(), 1, true, payloadString.c_str());

    return true;
}

void IdentificationService::createIdentificationJson(JsonObject root)
{
    root["UniqueID"] = SettingValue::format("#{unique_id}");
    root["UniqueName"] = SettingValue::format("LUST-motion-#{unique_id}");
    root["IP"] = WiFi.localIP().toString();
    root["MAC"] = WiFi.macAddress();
    root["FirmwareVersion"] = APP_VERSION;
}

esp_err_t IdentificationService::identification(PsychicRequest *request)
{
    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject root = response.getRoot();
    createIdentificationJson(root);
    return response.send();
}
