#include <LiquidCrystal.h>
#include <math.h>

// LCD pins: RS, Enable, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int SENSOR_MIDPOINT_PIN = A0;
const int REFERENCE_MIDPOINT_PIN = A1;

float R_FIXED_OHMS = 111.0;

const float ADC_REFERENCE = 5.0;
const float PT100_R0 = 100.0;

// PT100 coefficients 
const float PT100_A = 3.9083e-3;
const float PT100_B = -5.775e-7;

float resistanceToTemperature(float resistance) {
  float c = 1.0 - resistance / PT100_R0;
  float discriminant =
      PT100_A * PT100_A - 4.0 * PT100_B * c;

  if (discriminant < 0.0) {
    return -9999.0;
  }

  return (2.0 * c) / (-PT100_A - sqrt(discriminant));
}

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  int sensorCount = analogRead(SENSOR_MIDPOINT_PIN);
  int referenceCount = analogRead(REFERENCE_MIDPOINT_PIN);

  if (sensorCount <= 0 || sensorCount >= 1023) {
    delay(500);
    return;
  }

  float referenceVoltage =
      referenceCount * ADC_REFERENCE / 1023.0;

  float voltageDifference =
      (sensorCount - referenceCount) * ADC_REFERENCE / 1023.0;

  float sensorVoltage = referenceVoltage + voltageDifference;
  float sensorFraction = sensorVoltage / ADC_REFERENCE;

  if (sensorFraction <= 0.0 || sensorFraction >= 1.0) {
    delay(500);
    return;
  }

  float pt100Resistance =
      R_FIXED_OHMS * (1.0 - sensorFraction) / sensorFraction;

  float temperature = resistanceToTemperature(pt100Resistance);

  if (temperature == -9999.0) {
    delay(500);
    return;
  }

  // Display
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.write((byte)223);
  lcd.print("C");

  delay(500);
}