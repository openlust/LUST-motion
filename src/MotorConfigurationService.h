#pragma once

/**
 *   LUST-motion
 *
 *   Copyright (C) 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the MIT license. See the LICENSE file for details.
 **/

#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <StrokeEngine.h>
#include <NotificationEvents.h>
#include <RestartService.h>

#include <motor/motor.h>
#include <motor/virtualMotor.h>
#include <motor/genericStepper.h>
#include <motor/OSSMRefBoardV2.h>
#include <motor/ihsvServoV6.h>

#ifdef OSSM_REF
#include <boards/OSSMReferenceBoard.h>
// else throw compile error
#else
#error "No board defined"
#endif

#ifndef MOTION_FACTORY_TRAVEL
#define MOTION_FACTORY_TRAVEL 150.0
#endif

#define MOTOR_CONFIG_FILE "/config/motorConfig.json"
#define MOTOR_CONFIG_PATH "/rest/motorConfig"

// enum for the different drivers that can be used
enum MotorDriver
{
    VIRTUAL,
    GENERIC_STEPPER,
    OSSM_REF_BOARD_V2,
    IHSV_SERVO_V6
};

static motorProperties genericMotorProperties{
    .enableActiveLow = ENABLE_ACTIVE_LOW,
    .stepPin = STEP_PIN,
    .directionPin = DIRECTION_PIN,
    .enablePin = ENABLE_PIN,
};

static OSSMRefBoardV2Properties OSSMMotorProperties{
    .enableActiveLow = ENABLE_ACTIVE_LOW,
    .stepPin = STEP_PIN,
    .directionPin = DIRECTION_PIN,
    .enablePin = ENABLE_PIN,
    .alarmPin = ALARM_PIN,
    .inPositionPin = IN_POSITION_PIN,
    .ADCPinCurrent = ADC_PIN_CURRENT,
    .AmperePermV = AMPERE_PER_MILLIVOLT,
    .AmpereOffsetInmV = AMPERE_OFFSET_IN_MILLIVOLT,
    .ADCPinVoltage = ADC_PIN_VOLTAGE,
    .VoltPermV = VOLT_PER_MILLIVOLT,
};

static iHSVServoV6Properties iHSVV6MotorProperties{
    .enableActiveLow = ENABLE_ACTIVE_LOW,
    .stepPin = STEP_PIN,
    .directionPin = DIRECTION_PIN,
    .enablePin = ENABLE_PIN,
    .alarmPin = ALARM_PIN,
    .inPositionPin = IN_POSITION_PIN,
    .modbusRxPin = MODBUS_RX_PIN,
    .modbusTxPin = MODBUS_TX_PIN,
};

class MotorConfiguration
{
public:
    MotorDriver driver;
    int stepPerRev;
    int maxRPM;
    int maxAcceleration;
    int pulleyTeeth;
    boolean invertDirection;
    boolean measureTravel;
    boolean home;
    float travel;
    float keepout;
    float sensorlessTrigger = SENSORLESS_TRIGGER;

    static void read(MotorConfiguration &settings, JsonObject &root)
    {
        // translate the enum to a string
        switch (settings.driver)
        {
        case VIRTUAL:
            root["driver"] = "VIRTUAL";
            break;
        case GENERIC_STEPPER:
            root["driver"] = "GENERIC_STEPPER";
            break;
        case OSSM_REF_BOARD_V2:
            root["driver"] = "OSSM_REF_BOARD_V2";
            break;
        case IHSV_SERVO_V6:
            root["driver"] = "IHSV_SERVO_V6";
            break;
        default:
            root["driver"] = "VIRTUAL";
            break;
        }
        root["stepPerRev"] = settings.stepPerRev;
        root["maxRPM"] = settings.maxRPM;
        root["maxAcceleration"] = settings.maxAcceleration;
        root["pulleyTeeth"] = settings.pulleyTeeth;
        root["invertDirection"] = settings.invertDirection;
        root["measureTravel"] = false; // this is only for POST to start measuring cycle
        root["home"] = false;
        root["travel"] = settings.travel;
        root["keepout"] = settings.keepout;
        root["sensorlessTrigger"] = settings.sensorlessTrigger;
    }

    static StateUpdateResult update(JsonObject &root, MotorConfiguration &settings)
    {
        settings.stepPerRev = root["stepPerRev"] | STEP_PER_REV;
        settings.maxRPM = root["maxRPM"] | MAX_RPM;
        settings.maxAcceleration = root["maxAcceleration"] | MAX_ACCELERATION;
        settings.pulleyTeeth = root["pulleyTeeth"] | PULLEY_TEETH;
        settings.invertDirection = root["invertDirection"] | INVERT_DIRECTION;
        settings.measureTravel = root["measureTravel"] | false;
        settings.home = root["home"] | false;
        settings.travel = root["travel"] | MOTION_FACTORY_TRAVEL;
        settings.keepout = root["keepout"] | KEEP_OUT;
        settings.sensorlessTrigger = root["sensorlessTrigger"] | SENSORLESS_TRIGGER;

        // translate the string to an enum
        String driver = root["driver"] | "VIRTUAL";
        if (driver == "VIRTUAL")
        {
            settings.driver = VIRTUAL;
        }
        else if (driver == "GENERIC_STEPPER")
        {
            settings.driver = GENERIC_STEPPER;
        }
        else if (driver == "OSSM_REF_BOARD_V2")
        {
            settings.driver = OSSM_REF_BOARD_V2;
        }
        else if (driver == "IHSV_SERVO_V6")
        {
            settings.driver = IHSV_SERVO_V6;
        }
        else
        {
            settings.driver = VIRTUAL;
        }
        return StateUpdateResult::CHANGED;
    }
};

class MotorConfigurationService : public StatefulService<MotorConfiguration>
{
public:
    MotorConfigurationService(StrokeEngine *strokeEngine, AsyncWebServer *server, FS *fs, SecurityManager *securityManager, NotificationEvents *notificationEvent);
    void begin();

    String getDriverName();

private:
    HttpEndpoint<MotorConfiguration> _httpEndpoint;
    FSPersistence<MotorConfiguration> _fsPersistence;
    NotificationEvents *_notificationEvent;
    StrokeEngine *_strokeEngine;
    MotorInterface *_motor;
    MotorDriver _loadedDriver = VIRTUAL;

    void onConfigUpdated(String originId);
};