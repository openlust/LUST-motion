#ifndef CoreDump_h
#define CoreDump_h

/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2025 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ArduinoJson.h>
#include <ESPFS.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>
#include <WiFi.h>

#define CORE_DUMP_SERVICE_PATH "/rest/coreDump"

class CoreDump
{
public:
    CoreDump(PsychicHttpServer *server, SecurityManager *securityManager);

    void begin();

private:
    PsychicHttpServer *_server;
    SecurityManager *_securityManager;
    esp_err_t coreDump(PsychicRequest *request);
};

#endif // end CoreDump_h