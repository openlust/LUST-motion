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

#include <StrokeEngineControlService.h>

StrokeEngineControlService::StrokeEngineControlService(StrokeEngine *strokeEngine,
                                                       AsyncWebServer *server,
                                                       SecurityManager *securityManager,
                                                       AsyncMqttClient *mqttClient) : _strokeEngine(strokeEngine),
                                                                                      _httpEndpoint(StrokeEngineControl::read,
                                                                                                    StrokeEngineControl::update,
                                                                                                    this,
                                                                                                    server,
                                                                                                    SE_CONTROL_SETTINGS_ENDPOINT_PATH,
                                                                                                    securityManager,
                                                                                                    AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                      _mqttPubSub(StrokeEngineControl::read, StrokeEngineControl::update, this, mqttClient),
                                                                                      _webSocketServer(StrokeEngineControl::read,
                                                                                                       StrokeEngineControl::update,
                                                                                                       this,
                                                                                                       server,
                                                                                                       SE_CONTROL_SETTINGS_SOCKET_PATH,
                                                                                                       securityManager,
                                                                                                       AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                      _mqttClient(mqttClient)
/*  _webSocketClient(StrokeEngineControl::read,
StrokeEngineControl::update,
this,
SE_CONTROL_SETTINGS_SOCKET_PATH)*/
{
    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void StrokeEngineControlService::begin()
{
    _state.go = false;
    _state.depth = _strokeEngine->getParameter(StrokeParameter::DEPTH);
    _state.stroke = _strokeEngine->getParameter(StrokeParameter::STROKE);
    _state.rate = _strokeEngine->getParameter(StrokeParameter::RATE);
    _state.sensation = _strokeEngine->getParameter(StrokeParameter::SENSATION);
    _state.pattern = _strokeEngine->getCurrentPatternName();
    _state.vibrationOverride = false;
    _state.vibrationAmplitude = MOTION_FACTORY_VIBRATION_AMPLITUDE;
    _state.vibrationFrequency = MOTION_FACTORY_VIBRATION_FREQUENCY;
}

void StrokeEngineControlService::onConfigUpdated()
{
    ESP_LOGI("StrokeEngineControlService", "Config updated");
    bool sanitized = false;

    // Update stroke parameters and propagate changes to StatefulService
    float depth = _strokeEngine->setParameter(StrokeParameter::DEPTH, _state.depth, true);
    if (depth != _state.depth)
    {
        _state.depth = depth;
        sanitized = true;
    }
    float stroke = _strokeEngine->setParameter(StrokeParameter::STROKE, _state.stroke, true);
    if (stroke != _state.stroke)
    {
        _state.stroke = stroke;
        sanitized = true;
    }
    float rate = _strokeEngine->setParameter(StrokeParameter::RATE, _state.rate, true);
    if (rate != _state.rate)
    {
        _state.rate = rate;
        sanitized = true;
    }
    float sensation = _strokeEngine->setParameter(StrokeParameter::SENSATION, _state.sensation, true);
    if (sensation != _state.sensation)
    {
        _state.sensation = sensation;
        sanitized = true;
    }

    // only update pattern, if it has changed
    if (_strokeEngine->getCurrentPatternName() != _state.pattern)
    {
        _strokeEngine->setPattern(_state.pattern, true);
    }

    // Change running state of the stroke engine
    if ((_state.go == true) && (_strokeEngine->isActive() == false))
    {
        _strokeEngine->startPattern();
    }
    else if ((_state.go == false) && (_strokeEngine->isActive() == true))
    {
        _strokeEngine->stopMotion();
    }

    // propagate sanitized changes to StatefulService if necessary
    if (sanitized)
    {
        ESP_LOGI("StrokeEngineControlService", "Sanitized control settings");
        update([&](StrokeEngineControl &state)
               { return StateUpdateResult::CHANGED; },
               "onConfigUpdated");
    }
}
