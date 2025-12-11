# 🌱 Smart Automated Irrigation & Fertilizer System

This project is a fully automated irrigation, moisture-balancing, and fertilizer management system built using an ESP32.  
It uses real-time moisture and temperature data to intelligently water plants, apply fertilizer on schedule, and maintain ideal soil conditions.  
The system automatically performs irrigation at **6 AM and 6 PM**, adjusts moisture thresholds based on **temperature**, and activates **heating cables** to dry overly wet soil.

---

## 🌟 What This System Does

### ✔ Monitors the soil & environment  
- Soil moisture percentage (analog moisture sensor)  
- Temperature & Humidity (DHT22)  
- Real-time clock scheduling (DS3231 RTC)  
- LCD display for live status updates  

### ✔ Takes automatic action  
- Waters plants when moisture is low (6 AM & 6 PM checks)  
- Skips watering if moisture is already sufficient  
- Adjusts moisture threshold based on temperature  
- Applies fertilizer automatically every **14 days at 6 AM**  
- Activates heating cable when soil moisture is too high  

### ✔ Runs reliably with safety controls  
- Relay-controlled pump, solenoid, and heating cable  
- Temperature-aware moisture compensation  
- Pump runtime safety timeout  
- Persistent storage for fertilizer scheduling  

---

## 🧠 Why This Project Exists

Irrigation systems often waste water or over-water plants due to fixed thresholds.  
Soil moisture needs change depending on temperature, and fertilizer schedules are easy to forget.

This system solves all of that:

- Waters only when needed  
- Adapts to temperature changes  
- Automatically doses fertilizer  
- Prevents over-watering using a heating cable  
- Provides real-time visual feedback  

It ensures healthier plants, efficient water use, and zero manual scheduling.

---

## 🔌 Hardware Used
- **ESP32**  
- **DS3231 Real Time Clock**  
- **DHT22 Temperature & Humidity Sensor**  
- **Soil Moisture Sensor (Analog)**  
- **16×2 I2C LCD Display**  
- **Relay Modules** for:  
  - Water Pump  
  - Fertilizer Solenoid Valve  
  - Heating Cable  

Wiring details can be added in `hardware_connection.md`.

---

## 🛠 How It Works

### 1. Scheduled Moisture Checking  
At **6 AM and 6 PM**, the system checks soil moisture:  
- If moisture is low → watering starts  
- If moisture is sufficient → watering is skipped  

### 2. Temperature-Adaptive Moisture Threshold  
The system automatically increases or decreases the moisture threshold:  
- High temperature → keeps soil slightly wetter  
- Low temperature → allows soil to stay drier  

### 3. Fertilizer Automation  
Every **14 days**, during the 6 AM irrigation cycle:  
- The solenoid valve opens  
- Fertilizer mixes into the water flow  

### 4. Soil Drying System  
If moisture rises above the adjusted threshold at any time:  
- The heating cable turns ON to dry the soil  

### 5. Real-Time Display  
The LCD continuously shows:  
- Temperature  
- Humidity  
- Soil moisture  
- Pump status  
- Heating cable status  

Just tell me!
