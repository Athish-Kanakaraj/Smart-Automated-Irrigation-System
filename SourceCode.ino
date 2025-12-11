#include <Wire.h>
#include "RTClib.h"
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>

#define DHTPIN 4
#define DHTTYPE DHT22

const int pinWaterRelay = 16;
const int pinFertilizerRelay = 17;
const int pinHeatingRelay = 18;
const int pinSoilAnalog = 36;
const int pinStatusLED = 2;

const unsigned long fertilizerIntervalDays = 14UL;
const unsigned long fertilizerDoseDuration = 10UL;
const unsigned long pumpSafetyMaxSeconds = 300UL;
const unsigned long irrigationCheckInterval = 1000UL;

const int soilDryValue = 3400;
const int soilWetValue = 1200;
const float baseMoistureThresholdPct = 40.0;
const float hysteresisPct = 5.0;

const float tempRefC = 25.0;
const float tempCompFactor = 0.6;

RTC_DS3231 rtc;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences prefs;

unsigned long lastIrrigationCheckEpoch = 0;
int lastCheckHour = -1;
unsigned long lastPumpStartMillis = 0;
bool pumpRunning = false;
unsigned long pumpRunStartEpoch = 0;
unsigned long lastFertilizeEpoch = 0;
bool fertilizerPlannedThisMorning = false;
unsigned long lastLoopMillis = 0;

float readSoilMoisturePct() {
  int raw = analogRead(pinSoilAnalog);
  if (raw < 0) raw = 0;
  if (raw > 4095) raw = 4095;
  float pct;
  if (soilDryValue == soilWetValue) pct = 0.0;
  else pct = (float)(soilDryValue - raw) / (float)(soilDryValue - soilWetValue) * 100.0;
  if (pct < 0.0) pct = 0.0;
  if (pct > 100.0) pct = 100.0;
  return pct;
}

float getTempC() {
  float t = dht.readTemperature();
  if (isnan(t)) return NAN;
  return t;
}

float getHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) return NAN;
  return h;
}

unsigned long nowEpoch() {
  DateTime dt = rtc.now();
  return (unsigned long)dt.unixtime();
}

void startPump() {
  if (!pumpRunning) {
    digitalWrite(pinWaterRelay, HIGH);
    pumpRunning = true;
    pumpRunStartEpoch = nowEpoch();
    lastPumpStartMillis = millis();
  }
}

void stopPump() {
  if (pumpRunning) {
    digitalWrite(pinWaterRelay, LOW);
    pumpRunning = false;
  }
}

void openFertilizerSolenoid(unsigned long seconds) {
  digitalWrite(pinFertilizerRelay, HIGH);
  delay(seconds * 1000UL);
  digitalWrite(pinFertilizerRelay, LOW);
}

bool isFertilizerDue(unsigned long currentEpoch) {
  if (lastFertilizeEpoch == 0) return true;
  unsigned long diff = (currentEpoch - lastFertilizeEpoch);
  unsigned long days = diff / 86400UL;
  return (days >= fertilizerIntervalDays);
}

void saveLastFertilizeEpoch(unsigned long epoch) {
  lastFertilizeEpoch = epoch;
  prefs.putULong("lastFert", epoch);
}

float computeAdjustedMoistureThreshold(float tempC) {
  float base = baseMoistureThresholdPct;
  if (isnan(tempC)) return base;
  float delta = (tempC - tempRefC) * tempCompFactor;
  float adjusted = base + delta;
  if (adjusted < 10.0) adjusted = 10.0;
  if (adjusted > 90.0) adjusted = 90.0;
  return adjusted;
}

void scheduledIrrigation(unsigned long currentEpoch, bool isMorning) {
  float moisturePct = readSoilMoisturePct();
  float tempC = getTempC();
  float adjustedThreshold = computeAdjustedMoistureThreshold(tempC);
  if (moisturePct < adjustedThreshold) {
    bool doFertilizer = false;
    if (isMorning && isFertilizerDue(currentEpoch)) {
      doFertilizer = true;
    }
    startPump();
    if (doFertilizer) {
      digitalWrite(pinFertilizerRelay, HIGH);
      delay(fertilizerDoseDuration * 1000UL);
      digitalWrite(pinFertilizerRelay, LOW);
      saveLastFertilizeEpoch(currentEpoch);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinWaterRelay, OUTPUT);
  pinMode(pinFertilizerRelay, OUTPUT);
  pinMode(pinHeatingRelay, OUTPUT);
  pinMode(pinStatusLED, OUTPUT);
  digitalWrite(pinWaterRelay, LOW);
  digitalWrite(pinFertilizerRelay, LOW);
  digitalWrite(pinHeatingRelay, LOW);
  digitalWrite(pinStatusLED, LOW);

  analogReadResolution(12);

  Wire.begin();
  rtc.begin();
  dht.begin();

  lcd.init();
  lcd.backlight();

  prefs.begin("irrigate", false);
  lastFertilizeEpoch = prefs.getULong("lastFert", 0);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Irrigation Ready");
  delay(800);
}

void loop() {
  unsigned long loopMillis = millis();
  if (loopMillis - lastLoopMillis < irrigationCheckInterval) return;
  lastLoopMillis = loopMillis;

  float moisturePct = readSoilMoisturePct();
  float tempC = getTempC();
  float hum = getHumidity();

  float adjustedThreshold = computeAdjustedMoistureThreshold(tempC);
  float offThreshold = adjustedThreshold + (hysteresisPct / 2.0);

  if (moisturePct > adjustedThreshold) digitalWrite(pinHeatingRelay, HIGH);
  else digitalWrite(pinHeatingRelay, LOW);

  if (pumpRunning) {
    unsigned long pumpRunSec = nowEpoch() - pumpRunStartEpoch;
    if (pumpRunSec > pumpSafetyMaxSeconds) stopPump();
  }

  if (pumpRunning) {
    if (moisturePct >= offThreshold) stopPump();
  }

  if (rtc.begin()) {
    DateTime now = rtc.now();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();
    if ((hour == 6 || hour == 18) && minute == 0 && lastCheckHour != hour) {
      bool isMorning = (hour == 6);
      scheduledIrrigation(now.unixtime(), isMorning);
      lastCheckHour = hour;
    }
    if (minute > 1 && lastCheckHour == hour) lastCheckHour = -1;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  if (!isnan(tempC)) lcd.printf("T%.1fC H%.0f%% S%.0f%%", tempC, hum, moisturePct);
  else lcd.printf("T--C H--%% S%.0f%%", moisturePct);
  lcd.setCursor(0,1);
  if (pumpRunning) lcd.print("Pump:ON ");
  else lcd.print("Pump:OFF ");
  if (digitalRead(pinHeatingRelay)) lcd.print("Heat:ON");
  else lcd.print("Heat:OFF");

  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 500) {
    digitalWrite(pinStatusLED, !digitalRead(pinStatusLED));
    lastBlink = millis();
  }
}
