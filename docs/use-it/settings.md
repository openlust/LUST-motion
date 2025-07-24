# eStim Settings

## Alias

Noisy Cricket can be given a name. This is useful if you have several devices. The Alias could describe the electrode it is connected to. Or for parties the user / player associated with the device. The alias may be up to 16 characters long.

## Voltage Limit

For safety reasons one may limit the output voltage to levels that meet the individual capabilities. This safety limits can't be overruled and Noisy Cricket will constrain all outputs to these limits. This affects all control methods, including the remote API, Bursts, Actions, Streaming and the Web UI. By setting the voltage ramp parameter one may increase the voltage limit over time. This is useful to compensate for the habituation effect of the skin.

## Soft Start

Ramps up the voltage over the specified time when starting or changing a cyclical stimulation pattern. It does not apply to Bursts or Actions. Useful to avoid sudden and unpleasant shocks. The soft start time is the time it takes to ramp up from 0 to the set voltage. The soft start time is in seconds.

## Audio Feedback

Noisy Cricket can provide audible feedback of the electric pulses. This can add to the intimidation factor of the device. It can also be useful to give spectators an impression about the output. The pitch encodes the intensity of the stimulation. The higher the pitch, the higher the intensity.

| Feedback    | Description                                                                                                                  |
| ----------- | ---------------------------------------------------------------------------------------------------------------------------- |
| `None`      | No audio feedback for the stimulation. Like other devices.                                                                   |
| `Pulsating` | Each pulse gives a short tone. Pitch encodes the intensity. As each pulse is followed by a pause it gives a pulsating sound. |
| `Whistling` | Only the pitch encodes the intensity. Gives a constant whistling noise.                                                      |

## Heartbeat Mode

Heartbeat mode is a safety feature that will stop Noisy Cricket if the heartbeat signal is lost. This is useful if it is controlled via a remote API and the connection is lost. The heartbeat signal is simply sending the Safe State control message at least once every second. If the heartbeat signal is lost Noisy Cricket will enter the [safe state](../use-it/usage.md#safe-state). The machine can be restarted by deliberately clearing the safe state. If multiple remotes are connected to the machine, `Any` will stop the machine if any one remote is lost. `Last` will stop the machine once the last remote sending control messages is lost.

| Heartbeat Mode | Description                                 |
| :------------: | ------------------------------------------- |
|   `Disabled`   | Heartbeat disabled                          |
|     `Any`      | Enter safe state if one connections drops   |
|     `Last`     | Enter safe state when last connection drops |
