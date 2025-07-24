# Device Configuration

Under `System` > `Configuration` in the side menu one can find the devices hardware configuration settings.

## Power Off Settings

The power off settings control how the device can be switched off and wether it goes switches off by itself after a longer idle period without stimulation to save battery.

- **Auto Power Off**: Enable this setting to automatically switch off the device if for a certain time no stimulation occurred. The **Power Off Timeout** slider controls the idle time before the devices shuts itself off.
- **Power Off Button**: If this setting is enabled the device can be switched off by pressing and holding the button for > 5 seconds. Disable this setting if the user should be prevented from switching off the device on its own.

## Expansion Header

This setting controls how the expansion header is used. Only one predefined function of the expansion header is possible. It is not possible to mix functions.

- **Disabled**: The expansion header is not used. This is the default setting.
- **Display**: The OLED display with encoder and buttons is connected to the expansion header. See [Stand Alone Usage](../customize-it/OLED.md) for the build and usage manual.

## Neopixel

Three solder pads on the backside of the PCB can be used to connect a strip of WS2812 addressable RGB LEDs to Noisy Cricket. They display an VU-Meter style animation to show the current stimulation intensity. Connect the Neopixel PCB's solder pads and and Noisy Crickets solder pads with thin wires.

![Neopixel](../media/Neopipxel.jpg)

Insert the number of pixels in the field to scale the animation accordingly. Enter **0** to disable the Neopixel animation.

!!! warning "Current Consumption"

    WS2812 leds are known for their current consumption. Driving a string of 144 leds through the thin wire and from the battery will likely exceed the current supply capabilities of the board.
