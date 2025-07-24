---
hide:
  - navigation
  - toc
---

# Noisy Cricket - A Small, but Powerful Wearable E-Stim Development Board

![Noisy Crickets in the Wild](media/NoisyCricket.jpg)

Noisy Cricket is a small (wearable) e-stim development board which packs quite a punch. It is named after the famous tiny gun from the Men In Black movies. Because it is just that: Tiny, but not to be messed with. It offers a unique and innovate approach to e-stim for erotic and consensual BDSM play. It is based around an ESP32-S3 chip offering endless possibilities and plenty of connection options.

## Features

### :butterfly: One Device - One Electrode

Noisy Cricket is strictly a single channel device. While this seems counterintuitive at first, it offers several unique advantages. Since it is wireless all channels are perfectly isolated from each other. Crosstalk can be ruled out completely. Since it is wearable it can be attached close to the electrode eliminating those cumbersome trip wires entirely. And with the endless wireless connectivity options it is trivial to assemble and orchestrate large multi-channel sensory experiences. Further more it is suited for large deployments with dozens of devices to serve as fun party game.

### :cricket: Small & Wearable - Embed it Into Your Project

Its small PCB of only 35 x 65 mm make it the ideal solution for embedded e-stim projects. Check the repository for a number of 3D printed cases. Wether stand-alone, with an OLED display and encoder, or embedded into a generic chastity belt, Noisy Cricket is designed to be flexible and hackable.

### :medical_symbol: Powerful Output Compliant With IEC 60601-2-10

As the name suggests it packs quite a punch. Because e-stim just kicks a lot harder when everyone knows the device won't run out of power and is impossible to conquer. Yet, the pulse based output stays far away from any harmful levels of electricity. Although not designed as a medical device the output nonetheless fully complies with the medical norm IEC 60601-2-10 _"Medical electrical equipment - Part 2-10: Particular requirements for the basic safety and essential performance of nerve and muscle stimulators"_. Thanks to highly efficient pulse shapes it achieves excellent nerve activation at very low energy levels. A current limited output further enhances the safety and also ensures that shorting the output won't damage the device.

### :fireworks: Creates a Spectacle

The lack of any observable feedback sometimes makes it hard for the observer to get a sense for what is actually going on, or what the subject is feeling. A bright white LED flash illuminates the dungeon while the pulses rush through the subject. A small buzzer creates an audible feedback where you can hear what's going on and get a good impression of the feeling.

And of course there are solder points for Neopixels on the board. Because there can't be enough RGB LEDs on any project. These display a VU-Meter and visually animate the strength of the pulses.

### :game_die: Fun to Use

It goes without saying that it comes with loads of build-in pattern. Of course this includes the most famous pattern of the ET312, and so much more. Besides the famous "Multi-Adjust" which is called "Sensation" at second degree of freedom "Speed" is used to control how fast the pattern are running.

But the pattern go one step further, which is especially interesting for BDSM scenarios. In addition to the cyclically repeating regular pattern so called "Burst" pattern are available. These are one-shot bursts lasting for a few seconds. They can simply emulate the "Fire" button of other devices, and can be use as signals, punishments or just for fun.

Sensor driven actions enable fun predicament sessions. Use the QWIIC expansion connector to connect sensor boards like a load cell or an distance sensor to Noisy Cricket. Let the device take action when the sensor readings are outside the defined window. This allows endless training and predicament scenarios letting your imagination running wild.

Noisy Crickets' concept also allows for large multiplayer party games. Special provision functions make it easy to code party games and associate a device with an player.

### :brain: AI powered motion and position detection

Yes, AI is a must these days and so does Noisy Cricket. Thanks to it's IMU with AI core it can detect positions and motions. Lift position training to new heights: enforce rigorous fitness training, ensure those yoga poses are perfectly executed or make sure your partner stays on all fours. Pre-trained models make it easy to start and have a lot of fun. But the best, it doesn't require a super computer to train those poses, gestures or significant motions. Simply record new data samples through the UI, process the samples in the software of the IMU manufacturer and upload the trained model.

### :telephone: Rich Communication Interfaces

Noisy Cricket is wireless and connects to your local WiFi. Equipped with a RESTful API, MQTT client, and a powerful websocket based communication protocol it can be controlled easily. A build-in webserver serves a configuration UI giving you full control over the device. Additionally, Noisy Cricket has a pulse streaming API which grants direct access to the output stage on a pulse by pulse basis.

### :nut_and_bolt: Accessible DIY Solution with Low Entry Barrier

The PCB was designed to utilize JLCPCB's assembly service. The repository comes with all the required Gerber, BOM and SMD placement files, ready to be uploaded. This lowers the barrier to DIY such a device, as no SMD assembly skills are required. Only very light soldering is needed to connect the battery header or extension boards. For the cases a 3D printer is handy. One with a multi-material unit even better.

5 fully assembled boards from JLCPCB cost less than most medium to high-end boxes.

### :dart: Many Extensions

Noisy Cricket is a development board which has flexibility and extendability in mind. It comes with an expansion header, and exposes a couple of pins from the ESP32. These can be used to extend the devices functionality:

- OLED-Display with encoder and buttons to give it a stand-alone user interface without needing WiFi
- A microphone to react to sound
- 4 touch sensor pins allow any piece of metal to become an input sensor for predicament play
- Use the pins as analog or digital inputs

Additionally, the I2C port is exposed on a QWIIC compatible connector. Plenty of sensors are available as ready made break-out-boards just waiting to be connected. For instance connect a load cell sensor to measure weight or force.

### :wrench: Hackable

It goes without saying that the whole project is open-source and easy to extend. Only limited by your own imagination. The PCB is designed to be as open and extendable as possible, leaving you with a prefect base for your own ideas. Hack it, give it new features and enjoy the results. Of course the community would appreciate any contribution. Please give back by opening a pull request with your modifications. Create new cases, additional sensor inputs or new software functions. Or write and contribute your own pattern. Several helping classes make it easy to write rich pattern.

[Let's build a few!](make-it/PCBA.md)

## License

Copyright &copy; 2025 by <a href="https://github.com/theelims"  target="_blank" rel="noopener">elims</a>

All work - software and hardware - is licensed under the

**:fontawesome-brands-creative-commons: :fontawesome-brands-creative-commons-by: :fontawesome-brands-creative-commons-sa: Creative Commons Attribution-ShareAlike 4.0 International Public License**

For a full text of the license visit [CC-BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

!!! Question "Can I sell this on my web shop?"

    Yes, of course! You may start producing the PCB and sell it on your shop. The license was deliberately chosen to allow a commercial use by whoever is willing to go through the woes of starting their own business. However, the tech-kinky scene is rather limited in size and thus you must give back any improvements such that the kinky maker scene can take advantages of it. Please drop me a note if you sell it, so that I can include it into the sourcing guide.
