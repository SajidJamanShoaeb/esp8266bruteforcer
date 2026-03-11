# ESP8266 WiFi Bruteforcer
A web-based WiFi network scanning and monitoring dashboard built with the ESP8266.
This project demonstrates how embedded devices can create a local web interface to scan nearby wireless networks and display information in real time.

The goal of this project is educational — helping students, hobbyists, and IoT developers understand how WiFi scanning, embedded web servers, and device control work using the ESP8266.

---

## Features

* 📡 **WiFi Network Scanner** – Scan and list nearby wireless networks.
* 🌐 **Built-in Web Interface** – Access the control panel from any browser.
* ⚡ **Real-time Status Updates** – View device status and connection info.
* 💡 **LED Status Indicator** – Onboard LED shows device activity.
* 🔌 **Standalone Access Point Mode** – ESP8266 creates its own WiFi network.
* 🧠 **Educational Example** – Learn how ESP8266 web servers work.

---

## Hardware Requirements

* ESP8266 development board (NodeMCU recommended)
* USB cable
* Computer with Arduino IDE
* Optional: external LED or components for experimentation

---

## Software Requirements

Install the following before uploading the code:

* **Arduino IDE**
* **ESP8266 Board Package**

Required libraries:

```
ESP8266WiFi
ESP8266WebServer
```

These libraries are included in the ESP8266 board package.

---

## Installation

1. Install **Arduino IDE**.
2. Add the **ESP8266 board package** to Arduino.
3. Open the project `.ino` file.
4. Select your board:

```
Tools → Board → NodeMCU 1.0 (ESP8266)
```

5. Select the correct **COM port**.
6. Click **Upload**.

After uploading, open the **Serial Monitor (115200 baud)** to view device status.

---

## Usage

1. Power the ESP8266.
2. The device will create its own WiFi access point.
3. Connect to the ESP network using your phone or computer.
4. Open a browser and go to the device IP address.
5. The web dashboard will load where you can scan and view nearby WiFi networks.

---

## Educational Purpose

This project is designed to demonstrate:

* How WiFi scanning works
* Embedded web servers on microcontrollers
* IoT device interfaces
* Network awareness and security basics

---

## Security Notice

This project is intended **only for learning and educational purposes**.
Do not use it to access or interfere with networks you do not own or have permission to test.

---

## Future Improvements

* Signal strength graph
* Network channel analyzer
* Better UI design
* Mobile responsive dashboard
* Logging system

---

## Author

Created as an ESP8266 educational IoT project.

---

## License

This project is released under the **MIT License**.
Feel free to modify and use it for learning and research.
