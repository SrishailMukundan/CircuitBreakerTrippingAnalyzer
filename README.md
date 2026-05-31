# Smart Circuit Breaker Tripping Analyzer (IoT Current Monitor)

An IoT-enabled, non-invasive electrical current monitoring system designed to track real-time power consumption, calculate circuit thresholds, and provide early-warning alerts before a household circuit breaker trips. 

This project bridges **Ohm's Law** and **electrical engineering principles** with **embedded computing** and **data science** to solve a common domestic infrastructure challenge: unexpected circuit overloads.

---
 
## 💡 The Physics & Engineering Principle

Residential circuit breakers in the US typically trip at a continuous current threshold of **15A** or **20A** at a nominal voltage of **120V AC**. When multiple high-wattage appliances (e.g., microwaves, space heaters) run on the same parallel branch, the total current draw exceeds the breaker's rated capacity, causing a thermal or magnetic trip.

The system uses a non-invasive current transformer (CT) to measure alternating current via electromagnetic induction. The relationship between electrical power ($P$), voltage ($V$), and current ($I$) is continuously computed using:

$$P = V \times I$$

By sampling the root-mean-square current ($I_{\text{RMS}}$), the system accurately predicts total real-time wattage and tracks how close the branch circuit is to its maximum operating limit.

---

## 🛠️ System Architecture & Features

### 1. Hardware & Fabrication
* **Non-Invasive Sensing:** Utilizes an **SCT-013-000 Current Transformer sensor** clamped safely around the circuit's hot wire, eliminating the need to cut live AC lines.
* **Microcontroller Node:** An **ESP32 development board** samples the analog voltage output from the CT sensor burden resistor circuit.
* **Enclosure Design:** Custom 3D-printed, wall-mountable enclosure designed in **Fusion 360** featuring integrated cable management, modular mounting tabs, and ventilation paths for the electronics.

### 2. Embedded Systems & Calibration
* **High-Speed Sampling:** The ESP32 utilizes its internal ADC to continuously sample the AC sine wave at a high frequency to calculate true $I_{\text{RMS}}$.
* **Data Smoothing:** Implements a digital **moving average filter** in C++/Arduino to smooth out high-frequency noise and transient spikes from appliance startup surges.
* **Edge Alerts:** Features a physical piezoceramic buzzer inside the enclosure that sounds an audible warning if current consumption crosses **85%** of the rated breaker capacity.

### 3. Software Dashboard (IoT & Python)
* **Wireless Data Streaming:** The ESP32 acts as a Wi-Fi client, streaming real-time current data via **WebSockets** or **MQTT** over a local network.
* **Desktop GUI:** A custom desktop application written in **Python** using `PyQt6`, `NumPy`, and `Matplotlib` that displays:
  * A live, rolling line graph of total current (Amps) and load (Watts).
  * A digital gauge showing percentage threshold remaining before a theoretical trip.
  * Local logging capabilities to save power consumption patterns into a `.csv` file for long-term load analysis.

---

## 📂 Repository Structure

```text
├── hardware/
│   ├── cad/               # Fusion 360 .STEP and .STL files for enclosure
│   └── schematics/        # Circuit diagrams (burden resistor, sensor connection)
├── firmware/
│   └── src/               # ESP32 C++/Arduino code (sampling & Wi-Fi transmission)
├── software/
│   ├── dashboard.py       # Python PyQt6 GUI application
│   └── requirements.txt   # Python dependencies
└── README.md