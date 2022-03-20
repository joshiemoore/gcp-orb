# gcp-orb
Make GCP Dot mood lighting with an ESP-32 and some LED strips. This ESP-32 sketch queries the GCP index at regular intervals and sets a connected LED strip
to the [current color of the GCP Dot](http://www.gcpdot.com/gcpdot/).

This program is Free Software, licensed under the terms of [the GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.txt).

## What is the GCP Dot?
The Global Consciousness Project collects random numbers from around the world. Sometimes the numbers collected aren't as random as they should be.
Research suggests that human consciousness at large scale can cause subtle structure to appear in "random" data. When the GCP Dot is redder, this
suggests a significantly larger variance than expected. When the GCP Dot is bluer, this suggests a significantly smaller variance than expected.

You can find more information about the Global Consciousness Project [here](https://global-mind.org/).

## Make your own GCP Orb
1. Acquire an ESP-32 dev board and an LED strip compatible with the FastLED library.
2. Connect the 5V and ground pins of the LED strip to 5V and ground on your ESP-32.
3. Connect the LED strip's data pin to a GPIO pin on the ESP-32, and remember which pin you chose.
4. Plug your ESP-32 dev board into your computer's USB port.
5. Clone this repository, and open `gcporb.ino` in Arduino IDE.
6. Install libraries `HttpClient` and `TinyXML` under Tools > Manage Libraries.
7. Edit the User Configuration section to match your setup:
* `WIFI_SSID` should be set to the SSID of your wifi network.
* `WIFI_PASS` should be set to your wifi password.
* `LED_PIN` should be set to the GPIO pin you plugged your LED strip data pin into earlier.
* `NUM_LEDS` is the number of LEDs on your LED strip (needed by FastLED).
8. Upload the sketch to your ESP-32.

If everything worked, then your new GCP Orb will automatically query the GCP index at regular intervals, and you
will have mood lighting that matches the color of the GCP Dot on the GCP website. You do not need to keep the
GCP Orb plugged into the computer at this point, it's fully self-contained once it's set up. You can power it
with any 5V power supply that works on an ESP-32.

![pondering my orb](pondering_orb.jpg)
