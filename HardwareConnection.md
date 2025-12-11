# 🔌 Hardware Connections – Smart Irrigation & Fertilizer System

Below are the wiring details for all sensors, modules, relays, and peripherals used in the system.

---

## 🧠 ESP32 Pin Mapping

### 🌡 DHT22 (Temperature & Humidity)
| DHT22 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| DATA      | GPIO 4    |
| GND       | GND       |

---

## 🌱 Soil Moisture Sensor (Analog)
| Sensor Pin | ESP32 Pin |
|------------|-----------|
| AO (Analog Out) | GPIO 36 (VP) |
| VCC              | 3.3V or 5V (match your module) |
| GND              | GND |

---

## ⏱ DS3231 RTC (I2C)
| RTC Pin | ESP32 Pin |
|---------|-----------|
| SDA     | GPIO 21   |
| SCL     | GPIO 22   |
| VCC     | 3.3V or 5V |
| GND     | GND       |

---

## 🖥 16×2 LCD Display (I2C)
| LCD Pin | ESP32 Pin |
|---------|-----------|
| SDA     | GPIO 21   |
| SCL     | GPIO 22   |
| VCC     | 5V        |
| GND     | GND       |

(I2C Address: **0x27**)

---

## 💧 Relays (Active-Low Logic)

### 🚰 Water Pump Relay
| Relay Pin | ESP32 Pin |
|-----------|-----------|
| IN        | GPIO 16   |

### 🧪 Fertilizer Solenoid Relay
| Relay Pin | ESP32 Pin |
|-----------|-----------|
| IN        | GPIO 17   |

### 🔥 Heating Cable Relay
| Relay Pin | ESP32 Pin |
|-----------|-----------|
| IN        | GPIO 18   |

### Relay Power Pins
- VCC → 5V  
- GND → GND  
- **Active-low operation:**  
  - `digitalWrite(pin, LOW)` → **Relay ON**  
  - `digitalWrite(pin, HIGH)` → **Relay OFF**

---

## 🔆 Status LED (Optional)
| Device | ESP32 Pin |
|--------|-----------|
| LED    | GPIO 2    |

---

## 🧷 Power Guidelines
- ESP32 powered via USB or 5V input  
- Soil moisture sensor may require 3.3V or 5V (depends on module version)  
- All relays must be powered from **stable 5V**  
- **All modules must share a common ground** with ESP32  
