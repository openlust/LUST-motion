# Firmware Structure

## Block Diagram

## Build Flags & Factory Settings

platformio.ini

    ; Uncomment to use JSON instead of MessagePack for event messages. Default is MessagePack.
    ; -D EVENT_USE_JSON=1

    ; Uncomment to enable generate pulse graph data through Serial for Teleplot. It will also
    ; disable the high voltage generation to safe the PC.
    ; -D TELEPLOT_PULSE

    ; Uncomment to disable the high voltage generation to simulate the devices without
    ; risking getting shocked
    ; -D DISABLE_HIGH_VOLTAGE

factory_settings.ini

-D STIM_FACTORY_MIN_VOLTAGE=10.0F
-D STIM_FACTORY_MAX_VOLTAGE=160.0F
-D STIM_FACTORY_SENSATION=0.0
-D STIM_FACTORY_VOLTAGE_RAMP=3.0F
-D STIM_FACTORY_DEADTIME_MS=500U
-D STIM_FACTORY_BURST_DURATION=3.0
-D STIM_FACTORY_BURST_INTENSITY=105.0
-D STIM_FACTORY_BURST_VOLTAGE=8.0
-D STIM_FACTORY_MIN_PULSE_SPACING_MS=10U
-D STIM_FACTORY_BURST_QUEUE_SIZE=5
-D STIM_FACTORY_STREAM_QUEUE_SIZE=25
-D STIM_FACTORY_MAX_TASK_WAIT_TIME_MS=100
-D STIM_FACTORY_SOFT_START_SEC=3.0
-D STIM_FACTORY_LOWER_DYNAMIC_RANGE=50.0F ; 50% of the max pulse width the increase dynamic range on the low end
-D STIM_FACTORY_LOWER_DYNAMIC_CUTOFF=30.0F ; cut-off voltage for which the reduced pulse with is fully blended in. Set to 0.0F to disable increased dynamic range at the lower end.
-D STIM_FACTORY_IDENTIFICATION_WINDOW_MS=30000U ; 30 seconds to identify the device over MQTT by pressing the button
-D STIM_FACTORY_ENCODER_ACC=250 ; Acceleration of the encoder in steps per second per second
-D STIM_FACTORY_WATCHDOG_TIMEOUT_MS=1500 ; 1.5 seconds to wait for the next command before the watchdog kicks in

## StimEngine
