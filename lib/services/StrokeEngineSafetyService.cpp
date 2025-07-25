/**
 *   LUST-motion
 *
 *   Copyright (C) 2025 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the MIT license. See the LICENSE file for details.
 **/

#include <StrokeEngineSafetyService.h>

StrokeEngineSafetyService::StrokeEngineSafetyService(StrokeEngine *stroker,
                                                     ESP32SvelteKit *sveltekit,
                                                     SafeStateService *safeStateService) : _strokeEngine(stroker),
                                                                                           _httpEndpoint(StrokeEngineSafety::read,
                                                                                                         StrokeEngineSafety::update,
                                                                                                         this,
                                                                                                         sveltekit->getServer(),
                                                                                                         SAFETY_CONFIG_PATH,
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::NONE_REQUIRED),
                                                                                           _fsPersistence(StrokeEngineSafety::read, StrokeEngineSafety::update, this, sveltekit->getFS(), SAFETY_CONFIG_FILE),
                                                                                           _safeStateService(safeStateService),
                                                                                           _socket(sveltekit->getSocket())
{
    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(originId); },
                     false);
}

void StrokeEngineSafetyService::begin()
{
    // read from FS
    _fsPersistence.readFromFS();
    ESP_LOGI("StrokeEngineSafetyService", "Read safety settings from FS");

    _httpEndpoint.begin();
    _socket->registerEvent("heartbeat");

    // Sanity check of values just read from FS
    if (_state.depthLimit < 0.0 || _state.strokeLimit < 0.0)
    {
        // update stroke engine safety settings with default values
        update([&](StrokeEngineSafety &state)
               { 
                state.depthLimit = _strokeEngine->getMotor()->getMaxPosition();
                state.strokeLimit = _strokeEngine->getMotor()->getMaxPosition();
                return StateUpdateResult::CHANGED; },
               "initialization");
    }
    else
    {
        // update stroke engine safety settings
        onConfigUpdated("begin");
    }
}

void StrokeEngineSafetyService::onConfigUpdated(String originId)
{

    ESP_LOGI("StrokeEngineSafetyService", "Update safety settings by %s", originId.c_str());
    boolean sanitized = false;

    // update stroke engine safety settings
    float depthLimit = _strokeEngine->setLimit(StrokeLimit::DEPTH, _state.depthLimit);
    if (depthLimit != _state.depthLimit)
    {
        _state.depthLimit = depthLimit;
        sanitized = true;
    }

    float strokeLimit = _strokeEngine->setLimit(StrokeLimit::STROKE, _state.strokeLimit);
    if (strokeLimit != _state.strokeLimit)
    {
        _state.strokeLimit = strokeLimit;
        sanitized = true;
    }

    float velocityLimit = _strokeEngine->setLimit(StrokeLimit::VELOCITY, _state.velocityLimit);
    if (velocityLimit != _state.velocityLimit)
    {
        _state.velocityLimit = velocityLimit;
        sanitized = true;
    }

    float rateLimit = _strokeEngine->setLimit(StrokeLimit::RATE, _state.rateLimit);
    if (rateLimit != _state.rateLimit)
    {
        _state.rateLimit = rateLimit;
        sanitized = true;
    }

    // Apply new values immediately
    _strokeEngine->applyChangesNow();

    // _strokeEngine->setEaseInSpeed(_state.easeInSpeed);

    // update stroke engine control service
    _safeStateService->setHeartbeatMode(_state.heartbeatMode);
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    root["mode"] = _state.heartbeatMode;
    _socket->emitEvent("heartbeat", root);

    // Propagate sanitized changes to StatefulService, but prevent infinite loop
    if (sanitized && originId != "onConfigUpdated")
    {
        ESP_LOGW("StrokeEngineSafetyService", "Sanitized safety settings");
        update([&](StrokeEngineSafety &state)
               { return StateUpdateResult::CHANGED; },
               "onConfigUpdated");
    }
}