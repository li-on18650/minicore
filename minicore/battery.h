#ifndef BATTERY_H
#define BATTERY_H

#include <GyverOLED.h>

// Константи для Li-Po батареї
const float batteryMaxVoltage = 4.4;  // Максимальна напруга батареї
const float batteryMinVoltage = 3.0; // Мінімальна напруга батареї

// Фільтр середнього значення
const int filterSize = 10; // Розмір буферу для ковзного середнього
float voltageBuffer[filterSize];
int bufferIndex = 0;
bool bufferFull = false;

// Ініціалізація буферу батареї
void initBattery() {
  for (int i = 0; i < filterSize; i++) {
    voltageBuffer[i] = 0;
  }
}

// Функція додавання напруги до буферу
void addVoltageToBuffer(float voltage) {
  voltageBuffer[bufferIndex] = voltage;
  bufferIndex = (bufferIndex + 1) % filterSize;
  if (bufferIndex == 0) {
    bufferFull = true;
  }
}

// Функція обчислення середньої напруги
float getFilteredVoltage() {
  float sum = 0;
  int count = bufferFull ? filterSize : bufferIndex; // Якщо буфер заповнений, беремо всі значення

  for (int i = 0; i < count; i++) {
    sum += voltageBuffer[i];
  }

  return sum / count;
}

// Функція відображення статусу батареї
void displayBatteryStatus(GyverOLED<SSD1306_128x64, OLED_BUFFER>& oled) {
  int analogValue = analogRead(A1); // Читання напруги батареї

  // Розрахунок напруги з урахуванням подільника (R1 = R2 = 100 кОм)
  float voltage = analogValue * 3.3 / 4095.0 * 2;

  // Додавання значення до буферу та отримання згладженого значення
  addVoltageToBuffer(voltage);
  float filteredVoltage = getFilteredVoltage();

  // Розрахунок рівня заряду у відсотках
  float batteryLevel = (filteredVoltage - batteryMinVoltage) / (batteryMaxVoltage - batteryMinVoltage) * 100;

  // Обмеження рівня заряду в межах 0–100%
  batteryLevel = constrain(batteryLevel, 0, 100);

  // Вибір відповідного бітмапу
  const uint8_t batteryIcons[][7] = {
    {0x1c, 0x3e, 0x22, 0x22, 0x22, 0x22, 0x3e}, // 0% заряд
    {0x1c, 0x3e, 0x22, 0x22, 0x22, 0x3e, 0x3e}, // 25% заряд
    {0x1c, 0x3e, 0x22, 0x22, 0x3e, 0x3e, 0x3e}, // 50% заряд
    {0x1c, 0x3e, 0x22, 0x3e, 0x3e, 0x3e, 0x3e}, // 75% заряд
    {0x1c, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e}  // 100% заряд
  };

  uint8_t iconIndex;
  if (batteryLevel > 75) {
    iconIndex = 4; // 100%
  } else if (batteryLevel > 50) {
    iconIndex = 3; // 75%
  } else if (batteryLevel > 25) {
    iconIndex = 2; // 50%
  } else if (batteryLevel > 0) {
    iconIndex = 1; // 25%
  } else {
    iconIndex = 0; // 0%
  }

  // Виведення іконки батареї у верхньому правому куті
  oled.drawBitmap(112, 0, batteryIcons[iconIndex], 7, 7, 0);

  // Виведення рівня заряду у відсотках
  oled.setCursor(90, 0);
  oled.print((int)batteryLevel); // Вивід рівня заряду як ціле число
  oled.print(F("%"));
}

#endif // BATTERY_H
