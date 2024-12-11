# Smart-home-with-esp32
This project uses an ESP32 to create a web-based interface to control a single LED and an RGB LED.

Features:
Single LED Control: Turn on/off a single LED via web buttons.
RGB LED Control: Adjust RGB values using sliders.

Hardware:
ESP32: Connects to Wi-Fi and hosts the web server.

Connections:
Single LED: GPIO 21.
RGB LED: Red (GPIO 18), Green (GPIO 5), Blue (GPIO 4).

How It Works:
ESP32 connects to Wi-Fi.
Serves a web page with buttons and sliders.
Handles HTTP GET requests to:
Toggle the single LED (/A, /B).
Set RGB LED values (/rgb?red=<value>&green=<value>&blue=<value>).

Usage:
Upload code to ESP32 via Arduino IDE.
Connect to the ESP32's IP in a browser to access the interface.
