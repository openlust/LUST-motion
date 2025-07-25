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
                                                       ESP32SvelteKit *sveltekit,
                                                       MqttBrokerSettingsService *mqttBrokerSettingsService) : _strokeEngine(strokeEngine),
                                                                                                               _httpEndpoint(StrokeEngineControl::read,
                                                                                                                             StrokeEngineControl::update,
                                                                                                                             this,
                                                                                                                             sveltekit->getServer(),
                                                                                                                             SE_CONTROL_SETTINGS_ENDPOINT_PATH,
                                                                                                                             sveltekit->getSecurityManager(),
                                                                                                                             AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                                               _mqttEndpoint(StrokeEngineControl::read, StrokeEngineControl::update, this, sveltekit->getMqttClient()),
                                                                                                               _eventEndpoint(StrokeEngineControl::read,
                                                                                                                              StrokeEngineControl::update,
                                                                                                                              this,
                                                                                                                              sveltekit->getSocket(),
                                                                                                                              SE_CONTROL_SETTINGS_EVENT),
                                                                                                               _mqttClient(sveltekit->getMqttClient()),
                                                                                                               _mqttBrokerSettingsService(mqttBrokerSettingsService)
{
    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(originId); },
                     false);
}

void StrokeEngineControlService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();

    String controlTopicPub;
    String controlTopicSub;
    _mqttBrokerSettingsService->read([&](MqttBrokerSettings &settings)
                                     {  controlTopicPub = settings.controlTopicPub;
                                        controlTopicSub = settings.controlTopicSub; });
    _mqttEndpoint.configureTopics(controlTopicPub.c_str(), controlTopicSub.c_str());

    _state.command = "STOP";
    _state.depth = _strokeEngine->getParameter(StrokeParameter::DEPTH);
    _state.stroke = _strokeEngine->getParameter(StrokeParameter::STROKE);
    _state.rate = _strokeEngine->getParameter(StrokeParameter::RATE);
    _state.sensation = _strokeEngine->getParameter(StrokeParameter::SENSATION);
    _state.pattern = _strokeEngine->getCurrentPatternName();

    _strokeEngine->onNotify(std::bind(&StrokeEngineControlService::onStrokeEngineChanged, this, std::placeholders::_1));
}

void StrokeEngineControlService::onConfigUpdated(String originId)
{
    if (originId == "onConfigUpdated")
    {
        ESP_LOGI("StrokeEngineControlService", "onConfigUpdated 2nd round trip - skipping update");
        return;
    }

    ESP_LOGI("StrokeEngineControlService", "Config updated");
    bool sanitized = false;

    // Update stroke parameters and propagate changes to StatefulService
    float depth = _strokeEngine->setParameter(StrokeParameter::DEPTH, _state.depth);
    if (depth != _state.depth)
    {
        _state.depth = depth;
        sanitized = true;
    }
    float stroke = _strokeEngine->setParameter(StrokeParameter::STROKE, _state.stroke);
    if (stroke != _state.stroke)
    {
        _state.stroke = stroke;
        sanitized = true;
    }
    float rate = _strokeEngine->setParameter(StrokeParameter::RATE, _state.rate);
    if (rate != _state.rate)
    {
        _state.rate = rate;
        sanitized = true;
    }
    float sensation = _strokeEngine->setParameter(StrokeParameter::SENSATION, _state.sensation);
    if (sensation != _state.sensation)
    {
        _state.sensation = sensation;
        sanitized = true;
    }

    // Apply new values immediately
    _strokeEngine->applyChangesNow();

    // only update pattern, if it has changed
    if (_strokeEngine->getCurrentPatternName() != _state.pattern)
    {
        _strokeEngine->setPattern(_state.pattern, true);
    }

    // Change running state of the stroke engine
    if ((_state.command.equalsIgnoreCase("playpattern")) && (_strokeEngine->isActive() == false))
    {
        _strokeEngine->runCommand(StrokeCommand::PATTERN);
    }
    else if ((_state.command.equalsIgnoreCase("STOP")))
    {
        _strokeEngine->runCommand(StrokeCommand::STOP);
    }
    else if (_state.command.equalsIgnoreCase("retract"))
    {
        _strokeEngine->runCommand(StrokeCommand::RETRACT);
    }
    else if (_state.command.equalsIgnoreCase("stroke"))
    {
        _strokeEngine->runCommand(StrokeCommand::STROKE);
    }
    else if (_state.command.equalsIgnoreCase("depth"))
    {
        _strokeEngine->runCommand(StrokeCommand::DEPTH);
    }

    // propagate sanitized changes to StatefulService if necessary but prevent infinite loop
    if (sanitized && originId != "onConfigUpdated")
    {
        ESP_LOGI("StrokeEngineControlService", "Sanitized control settings");
        update([&](StrokeEngineControl &state)
               { return StateUpdateResult::CHANGED; },
               "onConfigUpdated");
    }
}

void StrokeEngineControlService::onStrokeEngineChanged(String reason)
{
    update([&](StrokeEngineControl &state)
           {
               StrokeEngineControl newSettings = state;
               newSettings.command = strokeCommandTable[(int)_strokeEngine->getCommand()];
               newSettings.depth = _strokeEngine->getParameter(StrokeParameter::DEPTH);
               newSettings.stroke = _strokeEngine->getParameter(StrokeParameter::STROKE);
               newSettings.rate = _strokeEngine->getParameter(StrokeParameter::RATE);
               newSettings.sensation = _strokeEngine->getParameter(StrokeParameter::SENSATION);
               newSettings.pattern = _strokeEngine->getCurrentPatternName();

               if (newSettings == state)
               {
                   return StateUpdateResult::UNCHANGED;
               }
               else
               {
                   state = newSettings;
                   return StateUpdateResult::CHANGED;
               }
               return StateUpdateResult::ERROR; },
           "StrokeEngine");
}
