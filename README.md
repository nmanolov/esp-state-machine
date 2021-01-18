# State Machine and Tickables

Arduino framework is great. At some point, however, it becomes increasingly difficult to maintain a single loop program that runs in ease of development and expressive power. Going to RTOS would require too much effort for who doesn't have deeper understanding of embedded systems.

## Components
This project aims in the middle. It introduces two useful abstractions that allow expressive power:
- `Tickable` - abstraction for everything that should "do something" in the main loop.
- `Satate Machine` - has some state, receives sygnals, changes the state and this changes trigger side effects

## Examples
- The hello world of automation [blynky](./lib/StateMachine/examples/blinky.ino) with a State machine.
- Toggle light with a button [toggly](./lib/StateMachine/examples/toggle_light.ino) with a state machine.
- Play a melody on a speaker [here](./lib/devices/examples/1_melody.cpp).
- Control repeat and playback of the melody over mqtt from home assistant [here](./examples/1_melody_with_integrations.cpp).

- The current [main.cpp](./src/main.cpp) is what I have deployed 3 places at home: MQTT enabled "HVAC", that controls a heating element via relay and aims for a configurable over mqtt target temperature.

Having these two, and corresponding descendants, I feel confident I can express most interesting home automation needs.
For example a garage could be represented as:
- States: CLOSED, OPENING, OPEN, CLOSING, STOPED_IN_THE_MIDDLE
- Sygnals: Radio_button_press_open, Radio_button_press_close, motion_sensor_detect_while_closing, ....

The project got a little sidetracked with all the things I wanted to achieve for the current need I have:
- Home Assistant integration via MQTT: autodiscovery, reading temperature sensors, commanding a heating hvac.
- OTA, as the heating elements aren't accessible: roof, and pit
- remote logging
- NTP time


## TODO
- [ ] Web based configuration instead of the precompiled configuration for: network, device name, mqtt server and client info.
  I am thinking of something similar to [WifiManager](https://github.com/tzapu/WiFiManager)

# Inspirations
- [WLED](https://github.com/Aircoookie/WLED) - a lighting automation project
- [QuantumLeaps StateMachine](https://www.state-machine.com/) - the big brother of all tools that are related to state machines for embedded systems.
