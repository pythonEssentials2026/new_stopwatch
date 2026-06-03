# ESP32 Wi‑Fi Stopwatch with LED Matrix Display

A **self‑contained, networked stopwatch system** built on the ESP32, designed for real‑world timing scenarios such as swimming training and competitions. The device combines **real‑time hardware display**, **local web control**, and **persistent data logging** into a single embedded system.

This project demonstrates end‑to‑end embedded development: firmware architecture, peripheral control, embedded networking, filesystem usage, and browser‑based device UI.

---

## Project Overview

The ESP32 operates as a **standalone Wi‑Fi access point**, hosting a web interface that allows users to:

* Start, stop, and reset a stopwatch remotely
* Record finished times with metadata (athlete name, pool, stroke, distance)
* View results live in the browser
* Export recorded results as a CSV file

At the same time, elapsed time is rendered in real‑time on a **MAX7219‑driven LED matrix display**, with an audible buzzer signal on start.

No internet connection, cloud services, or external infrastructure are required.

---

## Key Features

* **ESP32 SoftAP mode** – device-hosted Wi‑Fi network
* **Embedded HTTP server** for control and data access
* **Real‑time stopwatch** with millisecond precision
* **MAX7219 LED matrix display** (M:SS:HS format)
* **Audible start signal** via GPIO‑driven buzzer
* **SPIFFS persistent storage** for results logging
* **CSV export** for offline analysis
* **JSON API endpoint** for programmatic access
* **Browser-based UI** (HTML / CSS / JavaScript stored in flash)

---

## System Architecture

**Firmware Components**

* `WiFi.softAP()` for isolated device networking
* `WebServer` for REST-style endpoints
* `SPIFFS` for persistent CSV storage
* `Max72xxPanel` for LED matrix rendering
* Non-blocking timing using `millis()`

**Data Flow**

1. User connects to ESP32 Wi‑Fi AP
2. Browser UI sends HTTP requests (`/start`, `/stop`, `/finish`)
3. ESP32 updates hardware display in real time
4. Finished results are stored to SPIFFS as CSV
5. Data is accessible via download or JSON endpoint

---

## Hardware

* ESP32
* MAX7219 LED matrix modules (4× cascaded)
* Active buzzer
* Power supply (USB or regulated external source)

**Pin Usage**

| Function   | GPIO |
| ---------- | ---- |
| MAX7219 CS | 5    |
| Buzzer     | 27   |

---

## Web API Endpoints

| Endpoint        | Method | Description           |
| --------------- | ------ | --------------------- |
| `/`             | GET    | Web UI                |
| `/start`        | GET    | Start stopwatch       |
| `/stop`         | GET    | Stop stopwatch        |
| `/reset`        | GET    | Reset stopwatch       |
| `/time`         | GET    | Current time string   |
| `/finish`       | GET    | Record result + reset |
| `/download`     | GET    | Download CSV          |
| `/clear`        | POST   | Clear stored results  |
| `/results.json` | GET    | Results as JSON       |

---

## Why This Project Matters

This project was created to solve a **real, practical problem**: giving young swimmers immediate, visible feedback on their performance to encourage improvement. From an engineering perspective, it highlights the ability to:

* Build **robust standalone embedded systems**
* Combine **hardware timing constraints** with **network services**
* Design firmware that bridges **physical devices and web interfaces**
* Implement **persistent data storage** on constrained hardware

The same architectural patterns apply directly to **industrial IoT**, **test instrumentation**, **device dashboards**, and **embedded control panels**.

---

## Technologies Used

* **MCU:** ESP32
* **Language:** C++ (Arduino framework)
* **Networking:** Wi‑Fi SoftAP, HTTP
* **Storage:** SPIFFS
* **Display:** MAX7219 LED matrix
* **Frontend:** HTML / CSS / JavaScript

---




*This project is intentionally designed to be simple, reliable, and easily extensible—mirroring real embedded products rather than proof‑of‑concept code.*
