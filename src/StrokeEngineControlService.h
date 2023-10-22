#pragma once

/**
 *   LUST-motino
 *
 *   Copyright (C) 2023 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the MIT license. See the LICENSE file for details.
 **/

#include <LightMqttSettingsService.h>

#include <MqttPubSub.h>
#include <WebSocketServer.h>
#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <JsonUtils.h>

#include <StrokeEngine.h>

#ifndef MOTION_FACTORY_TRAVEL
#define MOTION_FACTORY_TRAVEL 150.0
#endif

#ifndef MOTION_FACTORY_STROKE
#define MOTION_FACTORY_STROKE 80.0
#endif

#ifndef MOTION_FACTORY_RATE
#define MOTION_FACTORY_RATE 30.0
#endif

#ifndef MOTION_FACTORY_SENSATION
#define MOTION_FACTORY_SENSATION 0.0
#endif

#ifndef MOTION_FACTORY_PATTERN
#define MOTION_FACTORY_PATTERN "PoundingTeasing"
#endif

#ifndef MOTION_FACTORY_VIBRATION_AMPLITUDE
#define MOTION_FACTORY_VIBRATION_AMPLITUDE 2.0
#endif

#ifndef MOTION_FACTORY_VIBRATION_FREQUENCY
#define MOTION_FACTORY_VIBRATION_FREQUENCY 30.0
#endif

#define SE_CONTROL_SETTINGS_ENDPOINT_PATH "/rest/control"
#define SE_CONTROL_SETTINGS_SOCKET_PATH "/ws/control"

class StrokeEngineControl
{
public:
    bool go;
    float depth;
    float stroke;
    float rate;
    float sensation;
    String pattern;
    bool vibrationOverride;
    float vibrationAmplitude;
    float vibrationFrequency;

    bool operator==(const StrokeEngineControl &settings) const
    {
        return go == settings.go && depth == settings.depth && stroke == settings.stroke &&
               rate == settings.rate && sensation == settings.sensation && pattern == settings.pattern &&
               vibrationOverride == settings.vibrationOverride && vibrationAmplitude == settings.vibrationAmplitude && vibrationFrequency == settings.vibrationFrequency;
    }

    static void read(StrokeEngineControl &settings, JsonObject &root)
    {
        root["go"] = settings.go;
        root["depth"] = settings.depth;
        root["stroke"] = settings.stroke;
        root["rate"] = settings.rate;
        root["sensation"] = settings.sensation;
        root["pattern"] = settings.pattern;
        root["vibration_override"] = settings.vibrationOverride;
        root["vibration_amplitude"] = settings.vibrationAmplitude;
        root["vibration_speed"] = settings.vibrationFrequency;
    }

    static StateUpdateResult update(JsonObject &root, StrokeEngineControl &settings)
    {
        StrokeEngineControl newSettings = {};
        // All parameters will be cropped or sanitized inside StrokeEngine
        // TODO: Now the potentially out of boundary parameters are propagated instead of the sanitized values.
        newSettings.go = root["go"] | false;
        newSettings.depth = root["depth"] | MOTION_FACTORY_TRAVEL;
        newSettings.stroke = root["stroke"] | MOTION_FACTORY_STROKE;
        newSettings.rate = root["rate"] | MOTION_FACTORY_RATE;
        newSettings.sensation = root["sensation"] | MOTION_FACTORY_SENSATION;
        newSettings.pattern = root["pattern"] | MOTION_FACTORY_PATTERN; // TODO: ignore when out available instead of default
        newSettings.vibrationOverride = root["vibration_override"] | false;
        newSettings.vibrationAmplitude = root["vibration_amplitude"] | MOTION_FACTORY_VIBRATION_AMPLITUDE;
        newSettings.vibrationFrequency = root["vibration_frequency"] | MOTION_FACTORY_VIBRATION_FREQUENCY;

        if (newSettings == settings)
        {
            return StateUpdateResult::UNCHANGED;
        }
        settings = newSettings;
        return StateUpdateResult::CHANGED;
    }
};

class StrokeEngineControlService : public StatefulService<StrokeEngineControl>
{
public:
    StrokeEngineControlService(StrokeEngine *strokeEngine,
                               AsyncWebServer *server,
                               SecurityManager *securityManager,
                               AsyncMqttClient *mqttClient,
                               LightMqttSettingsService *lightMqttSettingsService);

    void begin();

private:
    HttpEndpoint<StrokeEngineControl> _httpEndpoint;
    MqttPubSub<StrokeEngineControl> _mqttPubSub;
    WebSocketServer<StrokeEngineControl> _webSocketServer;
    // WebSocketClient<StrokeEngineControl> _webSocketClient;
    AsyncMqttClient *_mqttClient;
    LightMqttSettingsService *_lightMqttSettingsService;
    StrokeEngine *_strokeEngine;

    void registerConfig();
    void onConfigUpdated();
};
