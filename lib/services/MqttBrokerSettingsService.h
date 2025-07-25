#ifndef MqttBrokerSettingsService_h
#define MqttBrokerSettingsService_h

/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ESP32SvelteKit.h>
#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <MqttSettingsService.h>

#define MQTT_BROKER_SETTINGS_FILE "/config/brokerSettings.json"
#define MQTT_BROKER_SETTINGS_PATH "/rest/brokerSettings"

#ifndef FACTORY_MQTT_STATUS_TOPIC
#define FACTORY_MQTT_STATUS_TOPIC "openlust/motion/status"
#endif // end FACTORY_MQTT_STATUS_TOPIC

class MqttBrokerSettings
{
public:
    String controlTopicPub;
    String controlTopicSub;
    String environmentTopicPub;
    String stateTopicPub;
    String safeStateTopicPub;
    String safeStateTopicSub;

    static void read(MqttBrokerSettings &settings, JsonObject &root)
    {
        root["status_topic"] = settings.stateTopicPub;
        root["control_topic_pub"] = settings.controlTopicPub;
        root["control_topic_sub"] = settings.controlTopicSub;
        root["environment_topic_pub"] = settings.environmentTopicPub;
        root["safestate_topic_pub"] = settings.safeStateTopicPub;
        root["safestate_topic_sub"] = settings.safeStateTopicSub;
    }

    static StateUpdateResult update(JsonObject &root, MqttBrokerSettings &settings)
    {
        settings.controlTopicPub = root["control_topic_pub"] | SettingValue::format("lust-motion/#{unique_id}/control/status");
        settings.controlTopicSub = root["control_topic_sub"] | SettingValue::format("lust-motion/#{unique_id}/control/set");
        settings.environmentTopicPub = root["environment_topic_pub"] | SettingValue::format("lust-motion/#{unique_id}/environment/status");
        settings.stateTopicPub = root["status_topic"] | SettingValue::format(FACTORY_MQTT_STATUS_TOPIC);
        settings.safeStateTopicPub = root["safestate_topic_pub"] | SettingValue::format("lust-motion/#{unique_id}/safestate/status");
        settings.safeStateTopicSub = root["safestate_topic_sub"] | SettingValue::format("lust-motion/#{unique_id}/safestate/set");

        return StateUpdateResult::CHANGED;
    }
};

class MqttBrokerSettingsService : public StatefulService<MqttBrokerSettings>
{
public:
    MqttBrokerSettingsService(PsychicHttpServer *server, ESP32SvelteKit *sveltekit);
    void begin();
    void onConfigUpdated();

private:
    HttpEndpoint<MqttBrokerSettings> _httpEndpoint;
    FSPersistence<MqttBrokerSettings> _fsPersistence;
    MqttSettingsService *_mqttSettingsService;
};

#endif // end MqttBrokerSettingsService_h
