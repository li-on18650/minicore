#include <GyverOLED.h>
#include <pins.h>
#include "battery.h" // Підключаємо обробку батареї
#include "buzzer.h"  // Підключаємо обробку батареї

#define OLED_SOFT_BUFFER_64
#define ITEMS 7 // Кількість пунктів головного меню
#define analogPin A0 // Для кнопок

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;


// Порогові значення для кнопок
int thresholds[] = { 4095, 1790, 800, 2260, 2695 }; // Down, Up, Enter, Back, Right
int buttonRange = 100; // Розмір діапазону для порогових значень

// Змінні для роботи меню
int8_t pointer = 0; // Вказівник головного меню
int8_t submenuPointer = 0; // Вказівник субменю
bool inSubmenu = false; // Прапорець для перевірки, чи користувач у субменю
const unsigned long buttonDelay = 100; // Затримка між натисканнями кнопок (мс)


// Оголошення функцій
void checkButtons();
void handleButtonPress(int id);
void executeMenuFunction(int8_t pointer);
void executeBack();
void printPointer(uint8_t pointer);
void displayMenu();
void displayGamesSubmenu();
void displayMusicSubmenu();
void displaySettingsSubmenu();
void executeMusicFunction(int8_t submenuPointer);
void executeGamesFunction(int8_t submenuPointer);
void executeSettingsFunction(int8_t submenuPointer);
void launchSnake();
void launchTetris();
void launchPong();
void adjustBrightness();
void adjustContrast();
void resetSettings();

// Функція виводу головного меню на дисплей
void displayMenu() {
  oled.clear();
  oled.home();
  oled.println(F("Menu:")); // Назва меню
  oled.println(F(" minitext"));
  oled.println(F(" music"));
  oled.println(F(" time"));
  oled.println(F(" games"));
  oled.println(F(" file system"));
  oled.println(F(" gyroscope"));
  oled.println(F(" settings"));
  oled.println(F(" exit"));
  printPointer(pointer + 1); // Зсув курсора на 1 рядок через заголовок

  displayBatteryStatus(oled); // Вивід заряду батареї

  oled.update();
}

// Функція виводу субменю "Games"
void displayGamesSubmenu() {
  const char *submenuItems[] = {" Snake", " Tetris", " Pong"};
  oled.clear();
  oled.home();
  oled.println(F(" Games:"));
  for (uint8_t i = 0; i < 3; i++) {
    oled.println(submenuItems[i]);
  }
  printPointer(submenuPointer + 1);
  oled.update();
}
void displayMusicSubmenu(){
    const char *submenuItems[] = {" Doom", " Never_gonna", " Nokia", " Was wollen wir trinken?"}; // 4 пункти
  oled.clear();
  oled.home();
  oled.println(F(" Music"));
  for (uint8_t i = 0; i < 4; i++) {  // Змінили кількість на 4
    oled.println(submenuItems[i]);
  }
  printPointer(submenuPointer + 1);
  oled.update();
}

// Функція виводу субменю "Settings"
void displaySettingsSubmenu() {
  const char *submenuItems[] = {" Brightness", " Contrast", " Reset"};
  oled.clear();
  oled.home();
  oled.println(F(" Settings:"));
  for (uint8_t i = 0; i < 3; i++) {
    oled.println(submenuItems[i]);
  }
  printPointer(submenuPointer + 1);
  oled.update();
}

// Функція обробки натискання кнопок
void handleButtonPress(int id) {
  if (inSubmenu) {
    // Якщо користувач у субменю
    if (id == 0) { // Down
      submenuPointer = constrain(submenuPointer + 1, 0, 3); // Змінив межу на 3
    } else if (id == 1) { // Up
      submenuPointer = constrain(submenuPointer - 1, 0, 3); // Змінив межу на 3
    } else if (id == 2) { // Enter
      if (pointer == 1) executeMusicFunction(submenuPointer);
      if (pointer == 3) executeGamesFunction(submenuPointer);
      if (pointer == 6) executeSettingsFunction(submenuPointer);
    } else if (id == 3) { // Back
      inSubmenu = false; // Повернення в головне меню
      displayMenu();
    }
  } else {
    // Якщо користувач у головному меню
    if (id == 0) { // Down
      pointer = constrain(pointer + 1, 0, ITEMS - 1);
    } else if (id == 1) { // Up
      pointer = constrain(pointer - 1, 0, ITEMS - 1);
    } else if (id == 2) { // Enter
      executeMenuFunction(pointer);
    } else if (id == 3) { // Back
      executeBack();
    }
  }
  if (inSubmenu) {
    if (pointer == 1) displayMusicSubmenu();
    if (pointer == 3) displayGamesSubmenu();
    if (pointer == 6) displaySettingsSubmenu();
  } else {
    displayMenu();
  }
}

// Функція виконання вибраного пункту меню
void executeMenuFunction(int8_t pointer) {
  oled.clear();
  oled.home();

  switch (pointer) {
    case 0: oled.println(F("Minitext selected")); break;
    case 1: {
      submenuPointer = 0;
      inSubmenu = true;
      displayMusicSubmenu();
      return;
    }
    case 2: oled.println(F("Time selected")); break;
    case 3: {
      submenuPointer = 0;
      inSubmenu = true;
      displayGamesSubmenu();
      return;
    }
    case 4: oled.println(F("File system selected")); break;
    case 5: oled.println(F("Gyroscope selected")); break;
    case 6: {
      submenuPointer = 0;
      inSubmenu = true;
      displaySettingsSubmenu();
      return;
    }
    case 7: oled.println(F("Exiting menu...")); break;
  }

  oled.update();
  delay(1000);
  displayMenu();
}

// Функція виконання пункту субменю "Games"
void executeGamesFunction(int8_t submenuPointer) {
  switch (submenuPointer) {
    case 0: launchSnake(); break;
    case 1: launchTetris(); break;
    case 2: launchPong(); break;
    default: oled.println(F("Invalid Option")); break;
  }
}

void executeMusicFunction(int8_t submenuPointer) {
  switch (submenuPointer) {
    case 0: doom(); break;
    case 1: never_gonna_give_you_up(); break;
    case 2: nokia(); break;
    case 3: was_wollen_wir_trinken(); break;
    default: oled.println(F("Invalid Option")); break;
  }
}


// Функція виконання пункту субменю "Settings"
void executeSettingsFunction(int8_t submenuPointer) {
  switch (submenuPointer) {
    case 0: adjustBrightness(); break;
    case 1: adjustContrast(); break;
    case 2: resetSettings(); break;
    default: oled.println(F("Invalid Option")); break;
  }
}

// Функції для запуску ігор
void launchSnake() {
  oled.clear();
  oled.home();
  oled.println(F("Launching Snake..."));
  oled.update();
  delay(1000);
}

void launchTetris() {
  oled.clear();
  oled.home();
  oled.println(F("Launching Tetris..."));
  oled.update();
  delay(1000);
}

void launchPong() {
  oled.clear();
  oled.home();
  oled.println(F("Launching Pong..."));
  oled.update();
  delay(1000);
}

// Функції для налаштувань
void adjustBrightness() {
  oled.clear();
  oled.home();
  oled.println(F("Adjusting Brightness..."));
  oled.update();
  delay(1000);
}

void adjustContrast() {
  oled.clear();
  oled.home();
  oled.println(F("Adjusting Contrast..."));
  oled.update();
  delay(1000);
}

void resetSettings() {
  oled.clear();
  oled.home();
  oled.println(F("Resetting Settings..."));
  oled.update();
  delay(1000);
}

// Функція обробки кнопки "Back"
void executeBack() {
  oled.clear();
  oled.home();
  oled.println(F("Back pressed"));
  oled.update();
  delay(500);
  displayMenu(); // Повернення до меню
}

// Функція перевірки кнопок
void checkButtons() {
  int sensorValue = analogRead(analogPin); // Читання значення з аналогового піну

  for (int i = 0; i < 5; i++) {
    if (sensorValue > (thresholds[i] - buttonRange) && sensorValue < (thresholds[i] + buttonRange)) {
      handleButtonPress(i);
      delay(buttonDelay);
      break;
    }
  }
}

// Функція виводу вказівника
void printPointer(uint8_t pointer) {
  oled.setCursor(0, pointer); // Встановлення курсора у відповідний рядок
  oled.print(F(">"));
}

// Налаштування
void setup() {
  oled.init();
  oled.setContrast(255);
  initBattery(); // Ініціалізуємо буфер батареї
  displayMenu();
}

// Основний цикл
void loop() {
  checkButtons(); // Перевірка кнопок
}
