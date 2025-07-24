# Basic Usage

![Main Screen](../media/MainScreen.png)

This is the main dashboard of Noisy Cricket. On the left is the side menu. On small screens this disappears and a :material-menu:-icon appears in the top status bar. Clicking on the :material-lightning-bolt: icon or "Noisy Cricket" / given alias always leads back to the main dashboard.

The top status bar shows the WiFi signal status, the battery indicator and the off button. If a new firmware release is available a icon will appear to indicate an over-the-air update.

## Safe State

On first start the device is in safe state. In safe state all outputs are disabled and the device is locked. It won't output any stimulation pulses while in safe state. It will accept commands, schedules and actions may trigger, but not result in an output. In order to start the output the safe state must be cleared by pressing the green icon.

![Clear Safe State](../media/safestate.png)

The safe state is an essential safety feature and allows to stop all output immediately. It will be triggered automatically by the safety functions like the heartbeat watchdog, plugging in charger or manual activation through the GUI or API. To manually engage the safe state press the red icon on the sessions tab of the main page.

![Safe State](../media/EngageSafeState.png)

!!! danger "No Output While Charging"

    For safety reasons, the output is disabled while charging. This is to avoid electric shocks due to faulty chargers or cables. Noisy Cricket offers no galvanic isolation between the charging side and the output. If you are unable to clear the safe state check if Noisy Cricket is still plugged in.

## Stimulation

The stimulation panel is the main control center for the device and allows the control of the stimulation. It controls infinitely running stimulation pattern. These are cyclic in nature and repeat over time.

![Stimulation Settings](../media/StimulationSettings.png)

|                      Parameter                      | Description                                                                                                                                                                                                                                                                                  |
| :-------------------------------------------------: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|                     **Voltage**                     | Sets the output voltage for the stimulation. It ranges from 10 V to 160 V and is the main intensity adjustment. :snowflake: / :hot_pepper: toggles the range of the slider between 60 V and 160 V. To limit the output voltage use the [voltage limit setting](./settings.md#voltage-limit). |
|                    **Sensation**                    | Sensation changes the feeling of the pattern. It ranges from 0 to 100 and the exact behavior depends on the pattern chosen. For most pattern it controls the speed.                                                                                                                          |
|                      **Speed**                      | Speed controls how a pattern changes. It ranges from 0 to 100.                                                                                                                                                                                                                               |
|                  **Voltage Ramp**                   | This increases the voltage over time. It is given in Volts/Minute and can compensate for habituation to the stimulation. Or to create a challenging ramp. The voltage is only increased while output is generated. In idle the voltage increase is paused.                                   |
|                     **Pattern**                     | Selects the [Pattern](../use-it/pattern.md#continuous-pattern) which creates the stimulation.                                                                                                                                                                                                |
| :fontawesome-solid-play: / :fontawesome-solid-stop: | Starts or stops the stimulation. This is the regular way to start and stop the simulation.                                                                                                                                                                                                   |

!!! info

    The output stage of Noisy Cricket has a very low internal impedance. Especially low impedance electrodes like large metal surfaces or insertables only require low voltages to create strong sensations.

!!! info "Large insertable electrodes may become current limited by the output stage"

    If you're using larger metal insertable electrodes and the perceived intensity does not increase with increasing voltage you might have hit the current limit of the output stage. Please do not further increase the voltage, as this just drains the battery faster and makes the PCB run hot.

!!! warning

    Some pattern may feel stronger than others at the same voltage.

!!! warning

    Noisy Cricket is a very powerful electro stimulation unit. Please be careful when adjusting the voltage. Consider using the [voltage limit](../use-it/settings.md#voltage-limit) to avoid unpleasant surprises.

## Bursts

Burst are short one-shot pattern which can be used to send signals, or as an encouragement. They have the highest priority and preempt any other stimulation. Regular stimulations are paused for the duration of the burst and continue afterwards. If several burst pattern arrive at the same time they are queued up and executed in the order of their arrival.

![Burst Settings](../media/BurstSettings.png)

|           Parameter            | Description                                                                                                                                            |
| :----------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
|         **Intensity**          | Sets the output voltage relative to the voltage used for the regular [stimulation](#stimulation).                                                      |
|      **Minimum Voltage**       | Set a minimum voltage. If the intensity of the base voltage is lower as this minimum voltage, the minimum voltage is used instead.                     |
|          **Duration**          | The duration of the burst pattern. Can be anything from 1 Second to 10 Seconds. A burst cannot stopped except by engaging a [safe state](#safe-state). |
| **Change Voltage after Burst** | Changes the set voltage after each burst. This can be used to increase the base voltage with each burst.                                               |
|          **Pattern**           | Selects the [Pattern](../use-it/pattern.md#one-shot-burst-pattern) which creates the stimulation.                                                      |
|    :fontawesome-solid-fire:    | Adds the burst command with the selected settings to the burst queue.                                                                                  |

!!! warning

    Bursts are queued up. Pressing the :fontawesome-solid-fire:-button 3x will play the burst pattern 3 times in direct order.
