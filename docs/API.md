# API Documentation

## MDNS

LUST-motion is discoverable by MDNS services und advertises a `stroking` service. This can be used to identify the device on the local network and establish connections with it. The following service texts provide further information

| Text            | Information                                                                 |
| --------------- | --------------------------------------------------------------------------- |
| FirmwareVersion | Currently used firmware version to adjust for API versions and capabilities |
| DeviceID        | Unique device ID                                                            |

## Stateful Services

### StrokeEngine Control

The main control API to control LUST-motion. Starts and stops the motion, changes the main parameters depth, stroke, speed and sensation. Also allows the selection of a pattern, or if the streaming interfaces should be used as input source for the motion generation.

> Defined in `StrokeEngineControlService.h`

| Method | URL           | Authentication  |
| ------ | ------------- | --------------- |
| GET    | /rest/control | `NONE_REQUIRED` |
| POST   | /rest/control | `NONE_REQUIRED` |
| WS     | /ws/control   | `NONE_REQUIRED` |

### Parameters

| Parameter | Type    | Range           | Info                             | Failure Mode         |
| --------- | ------- | --------------- | -------------------------------- | -------------------- |
| go        | boolean | true / false    | Starts & stops the motion        | ignored              |
| depth     | float   | 0.0 - `travel`  | maximum depth of the motion      | truncated into range |
| stroke    | float   | 0.0 - `travel`  | length of the stroke             | truncated into range |
| speed     | float   | 0.0 - `speed`   | speed in strokes per minute      | truncated into range |
| sensation | float   | -100.0 - +100.0 | affects the feeling of a pattern | truncated into range |
| pattern   | string  | -               | maximum depth of the motion      | ignored              |

### JSON

```JSON
{
    "go": true,
    "depth": 120.0,
    "stroke": 80.5,
    "speed": 30.0,
    "sensation": 0.0,
    "pattern": "Deeper"
}
```

### StrokeEngine Safety Limits

This API can be used to restrict the mechanical reach of the machine and limit the maximum speed attainable. These soft limits will be used to truncate any control value coming through the [Control API](#strokeengine-control) or the streaming interface.

> Defined in `StrokeEngineSafetyService.h`

| Method | URL          | Authentication  |
| ------ | ------------ | --------------- |
| GET    | /rest/safety | `NONE_REQUIRED` |
| POST   | /rest/safety | `NONE_REQUIRED` |

### Parameters

| Parameter    | Type  | Range          | Info                             | Failure Mode         |
| ------------ | ----- | -------------- | -------------------------------- | -------------------- |
| depth_limit  | float | 0.0 - `travel` | maximum depth of the motion      | truncated into range |
| stroke_limit | float | 0.0 - `travel` | length of the stroke             | truncated into range |
| speed_limit  | float | 0.0 - `speed`  | affects the feeling of a pattern | truncated into range |

### JSON

```JSON
{
    "depth_limit": 120.0,
    "stroke_limit": 80.5,
    "speed_limit": 30.0
}
```

### StrokeEngine Environment _readonly_

This API will provide the information about the environment like maximum travel or maximum speed. This can be used by the UI to scale UI elements according to the machines real physical properties.

> Defined in `StrokeEngineEnvironmentService.h`

| Method | URL               | Authentication  |
| ------ | ----------------- | --------------- |
| GET    | /rest/environment | `NONE_REQUIRED` |

### Parameters

| Parameter   | Type  | Info                                                     |
| ----------- | ----- | -------------------------------------------------------- |
| travel      | float | maximum travel of the machine. Used for depth and stroke |
| speed_limit | float | maximum speed in FPM that the machine is capable         |

### JSON

```JSON
{
    "travel": 150.0,
    "max_speed": 30.0
}
```
