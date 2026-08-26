/*
 * H10 — USB HID Emulator
 * ESP32-S2/S3 USB keyboard/mouse emulation
 *
 * Features:
 *   - USB keyboard keystroke injection (individual keys, combos, strings)
 *   - USB mouse movement and jiggler
 *   - Predefined macros (Copy/Paste, Ctrl+Alt+Del, etc.)
 *   - Serial command interface
 *
 * Hardware: ESP32-S2/S3 DevKit with native USB
 * Library:  Built-in USB HID (ESP32 Arduino core 3.x)
 *
 * NOTE: USB HID requires ESP32-S2 or ESP32-S3 board.
 *       Compile with: arduino-cli compile --fqbn esp32:esp32:esp32s2
 *       For standard ESP32, only serial commands are available.
 */

#include <Arduino.h>

// ── Board Detection ──────────────────────────────────────────────
#if defined(SOC_USB_OTG_SUPPORTED) && CONFIG_TINYUSB_HID_ENABLED
  #define HAS_USB_HID 1
  #include "USB.h"
  #include "USBHIDKeyboard.h"
  #include "USBHIDMouse.h"
  USBHIDKeyboard Keyboard;
  USBHIDMouse Mouse;
#else
  #define HAS_USB_HID 0
  // Stub classes for compilation on standard ESP32
  class StubKeyboard {
  public:
    void begin() {}
    void end() {}
    size_t write(uint8_t) { return 1; }
    size_t press(uint8_t) { return 1; }
    size_t release(uint8_t) { return 1; }
    void releaseAll() {}
  };
  class StubMouse {
  public:
    void begin() {}
    void end() {}
    void move(int8_t, int8_t) {}
    void move(int8_t, int8_t, int8_t) {}
    void click(uint8_t = 1) {}
    void press(uint8_t = 1) {}
    void release(uint8_t = 1) {}
  };
  StubKeyboard Keyboard;
  StubMouse Mouse;
#endif

// ── Key Definitions ──────────────────────────────────────────────
// Standard USB HID keycodes
#define KEY_A 0x04
#define KEY_B 0x05
#define KEY_C 0x06
#define KEY_D 0x07
#define KEY_E 0x08
#define KEY_F 0x09
#define KEY_G 0x0A
#define KEY_H 0x0B
#define KEY_I 0x0C
#define KEY_J 0x0D
#define KEY_K 0x0E
#define KEY_L 0x0F
#define KEY_M 0x10
#define KEY_N 0x11
#define KEY_O 0x12
#define KEY_P 0x13
#define KEY_Q 0x14
#define KEY_R 0x15
#define KEY_S 0x16
#define KEY_T 0x17
#define KEY_U 0x18
#define KEY_V 0x19
#define KEY_W 0x1A
#define KEY_X 0x1B
#define KEY_Y 0x1C
#define KEY_Z 0x1D
#define KEY_1 0x1E
#define KEY_2 0x1F
#define KEY_3 0x20
#define KEY_4 0x21
#define KEY_5 0x22
#define KEY_6 0x23
#define KEY_7 0x24
#define KEY_8 0x25
#define KEY_9 0x26
#define KEY_0 0x27
#define KEY_ENTER 0x28
#define KEY_RETURN KEY_ENTER
#define KEY_ESC 0x29
#define KEY_BACKSPACE 0x2A
#define KEY_TAB 0x2B
#define KEY_SPACE 0x2C
#define KEY_MINUS 0x2D
#define KEY_EQUAL 0x2E
#define KEY_LEFT_BRACE 0x2F
#define KEY_RIGHT_BRACE 0x30
#define KEY_BACKSLASH 0x31
#define KEY_SEMICOLON 0x33
#define KEY_APOSTROPHE 0x34
#define KEY_GRAVE 0x35
#define KEY_COMMA 0x36
#define KEY_PERIOD 0x37
#define KEY_SLASH 0x38
#define KEY_CAPS_LOCK 0xC1
#define KEY_F1 0xC2
#define KEY_F2 0xC3
#define KEY_F3 0xC4
#define KEY_F4 0xC5
#define KEY_F5 0xC6
#define KEY_F6 0xC7
#define KEY_F7 0xC8
#define KEY_F8 0xC9
#define KEY_F9 0xCA
#define KEY_F10 0xCB
#define KEY_F11 0xCC
#define KEY_F12 0xCD
#define KEY_UP 0xDA
#define KEY_DOWN 0xD9
#define KEY_LEFT 0xD8
#define KEY_RIGHT 0xD7
#define KEY_INSERT 0xD1
#define KEY_DELETE 0xD4
#define KEY_HOME 0xD2
#define KEY_END 0xD5
#define KEY_PAGE_UP 0xD3
#define KEY_PAGE_DOWN 0xD6
#define KEY_PRINT_SCREEN 0xCE
#define KEY_SCROLL_LOCK 0xCF
#define KEY_PAUSE 0xD0

// Modifier keys
#define MOD_LEFT_CTRL   0x01
#define MOD_LEFT_SHIFT  0x02
#define MOD_LEFT_ALT    0x04
#define MOD_LEFT_GUI    0x08
#define MOD_RIGHT_CTRL  0x10
#define MOD_RIGHT_SHIFT 0x20
#define MOD_RIGHT_ALT   0x40
#define MOD_RIGHT_GUI   0x80

// Mouse buttons
#define MOUSE_LEFT   1
#define MOUSE_RIGHT  2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL    (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

// ── ASCII to HID Keycode Map ────────────────────────────────────
struct KeyMapping {
  char ascii;
  uint8_t keycode;
  uint8_t modifier;
};

const KeyMapping KEYMAP[] = {
  {'a', KEY_A, 0}, {'b', KEY_B, 0}, {'c', KEY_C, 0}, {'d', KEY_D, 0},
  {'e', KEY_E, 0}, {'f', KEY_F, 0}, {'g', KEY_G, 0}, {'h', KEY_H, 0},
  {'i', KEY_I, 0}, {'j', KEY_J, 0}, {'k', KEY_K, 0}, {'l', KEY_L, 0},
  {'m', KEY_M, 0}, {'n', KEY_N, 0}, {'o', KEY_O, 0}, {'p', KEY_P, 0},
  {'q', KEY_Q, 0}, {'r', KEY_R, 0}, {'s', KEY_S, 0}, {'t', KEY_T, 0},
  {'u', KEY_U, 0}, {'v', KEY_V, 0}, {'w', KEY_W, 0}, {'x', KEY_X, 0},
  {'y', KEY_Y, 0}, {'z', KEY_Z, 0},
  {'A', KEY_A, MOD_LEFT_SHIFT}, {'B', KEY_B, MOD_LEFT_SHIFT},
  {'C', KEY_C, MOD_LEFT_SHIFT}, {'D', KEY_D, MOD_LEFT_SHIFT},
  {'E', KEY_E, MOD_LEFT_SHIFT}, {'F', KEY_F, MOD_LEFT_SHIFT},
  {'G', KEY_G, MOD_LEFT_SHIFT}, {'H', KEY_H, MOD_LEFT_SHIFT},
  {'I', KEY_I, MOD_LEFT_SHIFT}, {'J', KEY_J, MOD_LEFT_SHIFT},
  {'K', KEY_K, MOD_LEFT_SHIFT}, {'L', KEY_L, MOD_LEFT_SHIFT},
  {'M', KEY_M, MOD_LEFT_SHIFT}, {'N', KEY_N, MOD_LEFT_SHIFT},
  {'O', KEY_O, MOD_LEFT_SHIFT}, {'P', KEY_P, MOD_LEFT_SHIFT},
  {'Q', KEY_Q, MOD_LEFT_SHIFT}, {'R', KEY_R, MOD_LEFT_SHIFT},
  {'S', KEY_S, MOD_LEFT_SHIFT}, {'T', KEY_T, MOD_LEFT_SHIFT},
  {'U', KEY_U, MOD_LEFT_SHIFT}, {'V', KEY_V, MOD_LEFT_SHIFT},
  {'W', KEY_W, MOD_LEFT_SHIFT}, {'X', KEY_X, MOD_LEFT_SHIFT},
  {'Y', KEY_Y, MOD_LEFT_SHIFT}, {'Z', KEY_Z, MOD_LEFT_SHIFT},
  {'1', KEY_1, 0}, {'2', KEY_2, 0}, {'3', KEY_3, 0}, {'4', KEY_4, 0},
  {'5', KEY_5, 0}, {'6', KEY_6, 0}, {'7', KEY_7, 0}, {'8', KEY_8, 0},
  {'9', KEY_9, 0}, {'0', KEY_0, 0},
  {' ', KEY_SPACE, 0}, {'-', KEY_MINUS, 0}, {'=', KEY_EQUAL, 0},
  {'[', KEY_LEFT_BRACE, 0}, {']', KEY_RIGHT_BRACE, 0},
  {'\\', KEY_BACKSLASH, 0}, {';', KEY_SEMICOLON, 0},
  {'\'', KEY_APOSTROPHE, 0}, {'`', KEY_GRAVE, 0},
  {',', KEY_COMMA, 0}, {'.', KEY_PERIOD, 0}, {'/', KEY_SLASH, 0},
  {'!', MOD_LEFT_SHIFT | KEY_1, 0}, {'@', MOD_LEFT_SHIFT | KEY_2, 0},
  {'#', MOD_LEFT_SHIFT | KEY_3, 0}, {'$', MOD_LEFT_SHIFT | KEY_4, 0},
  {'%', MOD_LEFT_SHIFT | KEY_5, 0}, {'^', MOD_LEFT_SHIFT | KEY_6, 0},
  {'&', MOD_LEFT_SHIFT | KEY_7, 0}, {'*', MOD_LEFT_SHIFT | KEY_8, 0},
  {'(', MOD_LEFT_SHIFT | KEY_9, 0}, {')', MOD_LEFT_SHIFT | KEY_0, 0},
  {'_', MOD_LEFT_SHIFT | KEY_MINUS, 0}, {'+', MOD_LEFT_SHIFT | KEY_EQUAL, 0},
  {'{', MOD_LEFT_SHIFT | KEY_LEFT_BRACE, 0},
  {'}', MOD_LEFT_SHIFT | KEY_RIGHT_BRACE, 0},
  {'|', MOD_LEFT_SHIFT | KEY_BACKSLASH, 0},
  {':', MOD_LEFT_SHIFT | KEY_SEMICOLON, 0},
  {'"', MOD_LEFT_SHIFT | KEY_APOSTROPHE, 0},
  {'~', MOD_LEFT_SHIFT | KEY_GRAVE, 0},
  {'<', MOD_LEFT_SHIFT | KEY_COMMA, 0},
  {'>', MOD_LEFT_SHIFT | KEY_PERIOD, 0},
  {'?', MOD_LEFT_SHIFT | KEY_SLASH, 0},
  {'\t', KEY_TAB, 0}, {'\n', KEY_ENTER, 0},
};

#define KEYMAP_SIZE (sizeof(KEYMAP) / sizeof(KEYMAP[0]))

// ── Functions ────────────────────────────────────────────────────
void typeChar(char c) {
  for (size_t i = 0; i < KEYMAP_SIZE; i++) {
    if (KEYMAP[i].ascii == c) {
      if (KEYMAP[i].modifier) {
        Keyboard.press(KEYMAP[i].modifier);
      }
      Keyboard.press(KEYMAP[i].keycode);
      delay(10);
      Keyboard.releaseAll();
      return;
    }
  }
}

void typeString(const String& str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    typeChar(str[i]);
    delay(5);
  }
}

void keyCombo(uint8_t mod, uint8_t key) {
  Keyboard.press(mod);
  Keyboard.press(key);
  delay(50);
  Keyboard.releaseAll();
}

void mouseJiggle(int amplitude, int intervalMs) {
  Serial.println(F("[*] Mouse jiggling... (Ctrl+C or 'stop' to stop)"));
  while (true) {
    Mouse.move(amplitude, 0);
    delay(intervalMs);
    Mouse.move(-amplitude, 0);
    delay(intervalMs);
    Mouse.move(0, amplitude);
    delay(intervalMs);
    Mouse.move(0, -amplitude);
    delay(intervalMs);

    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();
      if (cmd == "stop") break;
    }
  }
  Serial.println(F("[+] Jiggler stopped."));
}

void mouseCircle(int radius, int steps, int delayMs) {
  Serial.println(F("[*] Drawing circle..."));
  for (int i = 0; i < steps; i++) {
    float angle = (2.0 * PI * i) / steps;
    int x = (int)(radius * cos(angle));
    int y = (int)(radius * sin(angle));
    Mouse.move(x, y);
    delay(delayMs);
  }
  Serial.println(F("[+] Circle complete."));
}

void mouseBox(int width, int height, int delayMs) {
  Serial.println(F("[*] Drawing rectangle..."));
  Mouse.move(width, 0);
  delay(delayMs);
  Mouse.move(0, height);
  delay(delayMs);
  Mouse.move(-width, 0);
  delay(delayMs);
  Mouse.move(0, -height);
  delay(delayMs);
  Serial.println(F("[+] Rectangle complete."));
}

// ── Command Handlers ─────────────────────────────────────────────
void cmdType(const String& text) {
  Serial.print(F("[*] Typing: "));
  Serial.println(text);
  typeString(text);
  Serial.println(F("[+] Done."));
}

void cmdKey(const String& keyName) {
  String k = keyName;
  k.toLowerCase();

  if (k == "enter" || k == "return") { Keyboard.press(KEY_ENTER); delay(50); Keyboard.releaseAll(); }
  else if (k == "tab")  { Keyboard.press(KEY_TAB); delay(50); Keyboard.releaseAll(); }
  else if (k == "esc")  { Keyboard.press(KEY_ESC); delay(50); Keyboard.releaseAll(); }
  else if (k == "backspace") { Keyboard.press(KEY_BACKSPACE); delay(50); Keyboard.releaseAll(); }
  else if (k == "space") { Keyboard.press(KEY_SPACE); delay(50); Keyboard.releaseAll(); }
  else if (k == "up")   { Keyboard.press(KEY_UP); delay(50); Keyboard.releaseAll(); }
  else if (k == "down") { Keyboard.press(KEY_DOWN); delay(50); Keyboard.releaseAll(); }
  else if (k == "left") { Keyboard.press(KEY_LEFT); delay(50); Keyboard.releaseAll(); }
  else if (k == "right") { Keyboard.press(KEY_RIGHT); delay(50); Keyboard.releaseAll(); }
  else if (k == "delete") { Keyboard.press(KEY_DELETE); delay(50); Keyboard.releaseAll(); }
  else if (k == "home") { Keyboard.press(KEY_HOME); delay(50); Keyboard.releaseAll(); }
  else if (k == "end")  { Keyboard.press(KEY_END); delay(50); Keyboard.releaseAll(); }
  else if (k == "pgup") { Keyboard.press(KEY_PAGE_UP); delay(50); Keyboard.releaseAll(); }
  else if (k == "pgdn") { Keyboard.press(KEY_PAGE_DOWN); delay(50); Keyboard.releaseAll(); }
  else if (k.startsWith("f") && k.length() <= 3) {
    int fnum = k.substring(1).toInt();
    if (fnum >= 1 && fnum <= 12) {
      Keyboard.press(KEY_F1 + fnum - 1);
      delay(50);
      Keyboard.releaseAll();
    }
  } else {
    Serial.print(F("[?] Unknown key: "));
    Serial.println(keyName);
    return;
  }
  Serial.print(F("[+] Pressed: "));
  Serial.println(keyName);
}

void cmdCombo(const String& combo) {
  // Format: modifier+key (e.g., "ctrl+c", "alt+f4")
  String modStr = "";
  String keyStr = combo;
  int plusIdx = combo.indexOf('+');

  if (plusIdx > 0) {
    modStr = combo.substring(0, plusIdx);
    keyStr = combo.substring(plusIdx + 1);
  }

  uint8_t mod = 0;
  modStr.toLowerCase();
  if (modStr == "ctrl" || modStr == "control") mod = MOD_LEFT_CTRL;
  else if (modStr == "shift")   mod = MOD_LEFT_SHIFT;
  else if (modStr == "alt")     mod = MOD_LEFT_ALT;
  else if (modStr == "gui" || modStr == "meta" || modStr == "win") mod = MOD_LEFT_GUI;

  keyStr.toLowerCase();
  uint8_t key = 0;
  if (keyStr.length() == 1) {
    char c = keyStr[0];
    if (c >= 'a' && c <= 'z') key = KEY_A + (c - 'a');
    else if (c >= '1' && c <= '9') key = KEY_1 + (c - '1');
    else if (c == '0') key = KEY_0;
  } else if (keyStr == "enter") key = KEY_ENTER;
  else if (keyStr == "tab") key = KEY_TAB;
  else if (keyStr == "esc") key = KEY_ESC;
  else if (keyStr == "backspace") key = KEY_BACKSPACE;
  else if (keyStr == "delete") key = KEY_DELETE;
  else if (keyStr == "space") key = KEY_SPACE;
  else if (keyStr == "f4") key = KEY_F4;
  else if (keyStr == "f5") key = KEY_F5;

  if (mod && key) {
    keyCombo(mod, key);
    Serial.print(F("[+] Combo: "));
    Serial.println(combo);
  } else {
    Serial.print(F("[?] Invalid combo: "));
    Serial.println(combo);
  }
}

void cmdMacro(const String& name) {
  String m = name;
  m.toLowerCase();

  if (m == "copy") {
    Serial.println(F("[*] Macro: Copy (Ctrl+C)"));
    keyCombo(MOD_LEFT_CTRL, KEY_C);
  } else if (m == "paste") {
    Serial.println(F("[*] Macro: Paste (Ctrl+V)"));
    keyCombo(MOD_LEFT_CTRL, KEY_V);
  } else if (m == "undo") {
    Serial.println(F("[*] Macro: Undo (Ctrl+Z)"));
    keyCombo(MOD_LEFT_CTRL, KEY_Z);
  } else if (m == "redo") {
    Serial.println(F("[*] Macro: Redo (Ctrl+Y)"));
    keyCombo(MOD_LEFT_CTRL, KEY_Y);
  } else if (m == "selectall" || m == "select-all") {
    Serial.println(F("[*] Macro: Select All (Ctrl+A)"));
    keyCombo(MOD_LEFT_CTRL, KEY_A);
  } else if (m == "save") {
    Serial.println(F("[*] Macro: Save (Ctrl+S)"));
    keyCombo(MOD_LEFT_CTRL, KEY_S);
  } else if (m == "find") {
    Serial.println(F("[*] Macro: Find (Ctrl+F)"));
    keyCombo(MOD_LEFT_CTRL, KEY_F);
  } else if (m == "newtab") {
    Serial.println(F("[*] Macro: New Tab (Ctrl+T)"));
    keyCombo(MOD_LEFT_CTRL, KEY_T);
  } else if (m == "close") {
    Serial.println(F("[*] Macro: Close Tab (Ctrl+W)"));
    keyCombo(MOD_LEFT_CTRL, KEY_W);
  } else if (m == "refresh") {
    Serial.println(F("[*] Macro: Refresh (Ctrl+R)"));
    keyCombo(MOD_LEFT_CTRL, KEY_R);
  } else if (m == "lock") {
    Serial.println(F("[*] Macro: Lock Screen (Win+L)"));
    keyCombo(MOD_LEFT_GUI, KEY_L);
  } else if (m == "run") {
    Serial.println(F("[*] Macro: Run Dialog (Win+R)"));
    keyCombo(MOD_LEFT_GUI, KEY_R);
  } else if (m == "alttab") {
    Serial.println(F("[*] Macro: Alt+Tab"));
    keyCombo(MOD_LEFT_ALT, KEY_TAB);
  } else if (m == "screenshot") {
    Serial.println(F("[*] Macro: Screenshot (Win+Shift+S)"));
    Keyboard.press(MOD_LEFT_GUI);
    Keyboard.press(MOD_LEFT_SHIFT);
    Keyboard.press(KEY_S);
    delay(50);
    Keyboard.releaseAll();
  } else {
    Serial.print(F("[?] Unknown macro: "));
    Serial.println(name);
  }
}

void cmdMouse(const String& args) {
  int space1 = args.indexOf(' ');
  if (space1 < 0) {
    Serial.println(F("[!] Usage: mouse <dx> <dy> [speed]"));
    return;
  }
  int space2 = args.indexOf(' ', space1 + 1);

  String dxStr = args.substring(0, space1);
  String dyStr = (space2 > 0) ? args.substring(space1 + 1, space2) : args.substring(space1 + 1);
  String spdStr = (space2 > 0) ? args.substring(space2 + 1) : "10";

  int dx = dxStr.toInt();
  int dy = dyStr.toInt();
  int spd = spdStr.toInt();
  if (spd < 1) spd = 10;

  Serial.print(F("[*] Mouse move: "));
  Serial.print(dx);
  Serial.print(F(", "));
  Serial.println(dy);

  int steps = max(abs(dx), abs(dy));
  float stepX = (float)dx / steps;
  float stepY = (float)dy / steps;

  for (int i = 0; i < steps; i++) {
    Mouse.move((int)stepX, (int)stepY);
    delay(spd);
  }
  Serial.println(F("[+] Done."));
}

void cmdJiggle(const String& args) {
  int amp = 5;
  int interval = 2000;

  int sp = args.indexOf(' ');
  if (sp > 0) {
    amp = args.substring(0, sp).toInt();
    interval = args.substring(sp + 1).toInt();
  } else if (args.length() > 0) {
    amp = args.toInt();
  }
  if (amp < 1) amp = 5;
  if (interval < 100) interval = 2000;

  mouseJiggle(amp, interval);
}

void cmdClick(const String& args) {
  String btn = args;
  btn.toLowerCase();
  if (btn == "right") {
    Mouse.click(MOUSE_RIGHT);
    Serial.println(F("[+] Right click"));
  } else if (btn == "middle") {
    Mouse.click(MOUSE_MIDDLE);
    Serial.println(F("[+] Middle click"));
  } else {
    Mouse.click(MOUSE_LEFT);
    Serial.println(F("[+] Left click"));
  }
}

void cmdScroll(const String& args) {
  int amount = args.toInt();
  if (amount == 0) amount = 3;
  Mouse.move(0, 0, amount);
  Serial.print(F("[+] Scroll: "));
  Serial.println(amount);
}

void cmdDuckyScript(const String& line) {
  String cmd = line;
  cmd.trim();
  int sp = cmd.indexOf(' ');
  String action = (sp > 0) ? cmd.substring(0, sp) : cmd;
  String arg = (sp > 0) ? cmd.substring(sp + 1) : "";
  action.toLowerCase();

  if (action == "string" || action == "type") {
    typeString(arg);
  } else if (action == "delay") {
    delay(arg.toInt());
  } else if (action == "enter" || action == "return") {
    Keyboard.press(KEY_ENTER);
    delay(50);
    Keyboard.releaseAll();
  } else if (action == "gui" || action == "meta") {
    String key = arg;
    key.toLowerCase();
    uint8_t k = 0;
    if (key.length() == 1 && key[0] >= 'a' && key[0] <= 'z') k = KEY_A + (key[0] - 'a');
    if (k) keyCombo(MOD_LEFT_GUI, k);
  } else if (action == "ctrl" || action == "control") {
    String key = arg;
    key.toLowerCase();
    uint8_t k = 0;
    if (key.length() == 1) {
      if (key[0] >= 'a' && key[0] <= 'z') k = KEY_A + (key[0] - 'a');
      else if (key[0] >= '1' && key[0] <= '9') k = KEY_1 + (key[0] - '1');
      else if (key[0] == '0') k = KEY_0;
    }
    if (k) keyCombo(MOD_LEFT_CTRL, k);
  } else if (action == "alt") {
    String key = arg;
    key.toLowerCase();
    uint8_t k = 0;
    if (key == "f4") k = KEY_F4;
    else if (key == "tab") k = KEY_TAB;
    else if (key.length() == 1 && key[0] >= 'a' && key[0] <= 'z') k = KEY_A + (key[0] - 'a');
    if (k) keyCombo(MOD_LEFT_ALT, k);
  } else if (action == "shift") {
    String key = arg;
    key.toLowerCase();
    uint8_t k = 0;
    if (key.length() == 1 && key[0] >= 'a' && key[0] <= 'z') k = KEY_A + (key[0] - 'a');
    if (k) keyCombo(MOD_LEFT_SHIFT, k);
  } else if (action == "click") {
    cmdClick(arg);
  } else if (action == "moverel") {
    int sp2 = arg.indexOf(' ');
    if (sp2 > 0) {
      int dx = arg.substring(0, sp2).toInt();
      int dy = arg.substring(sp2 + 1).toInt();
      Mouse.move(dx, dy);
    }
  }
}

// ── Menu ─────────────────────────────────────────────────────────
void printMenu() {
  Serial.println();
  Serial.println(F("╔══════════════════════════════════════════╗"));
  Serial.println(F("║      H10 — USB HID Emulator             ║"));
  Serial.println(F("╠══════════════════════════════════════════╣"));
#if HAS_USB_HID
  Serial.println(F("║  [USB HID ACTIVE]                       ║"));
#else
  Serial.println(F("║  [SERIAL ONLY — need ESP32-S2/S3]       ║"));
#endif
  Serial.println(F("║                                         ║"));
  Serial.println(F("║  Keyboard:                              ║"));
  Serial.println(F("║    type <text>    — type a string       ║"));
  Serial.println(F("║    key <name>     — press a key         ║"));
  Serial.println(F("║    combo <mod+key>— key combination     ║"));
  Serial.println(F("║    macro <name>   — predefined macro    ║"));
  Serial.println(F("║                                         ║"));
  Serial.println(F("║  Mouse:                                 ║"));
  Serial.println(F("║    mouse <dx> <dy>— move mouse          ║"));
  Serial.println(F("║    click [btn]   — click (left/right)  ║"));
  Serial.println(F("║    scroll <amt>  — scroll wheel        ║"));
  Serial.println(F("║    jiggle [amp]  — anti-sleep jiggle   ║"));
  Serial.println(F("║    circle [r]    — draw circle          ║"));
  Serial.println(F("║                                         ║"));
  Serial.println(F("║  Payloads:                              ║"));
  Serial.println(F("║    payload <line> — DuckyScript line    ║"));
  Serial.println(F("║    codes          — show key codes      ║"));
  Serial.println(F("║    menu           — show this menu      ║"));
  Serial.println(F("╚══════════════════════════════════════════╝"));
  Serial.println();
}

void cmdCodes() {
  Serial.println(F("╔══════════════════════════════════════════╗"));
  Serial.println(F("║       H10 — Key Code Reference          ║"));
  Serial.println(F("╠══════════════════════════════════════════╣"));
  Serial.println(F("║  Keys: enter, tab, esc, backspace,      ║"));
  Serial.println(F("║        space, delete, home, end,        ║"));
  Serial.println(F("║        pgup, pgdn, up, down, left,      ║"));
  Serial.println(F("║        right, f1-f12                     ║"));
  Serial.println(F("║                                         ║"));
  Serial.println(F("║  Modifiers: ctrl, shift, alt, gui       ║"));
  Serial.println(F("║  Combos: ctrl+c, alt+f4, gui+r, etc.    ║"));
  Serial.println(F("║                                         ║"));
  Serial.println(F("║  Macros: copy, paste, undo, redo,       ║"));
  Serial.println(F("║    selectall, save, find, newtab,        ║"));
  Serial.println(F("║    close, refresh, lock, run, alttab,   ║"));
  Serial.println(F("║    screenshot                           ║"));
  Serial.println(F("║                                         ║"));
  Serial.println(F("║  Mouse: left, right, middle             ║"));
  Serial.println(F("╚══════════════════════════════════════════╝"));
}

// ── Setup ────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

#if HAS_USB_HID
  Keyboard.begin();
  Mouse.begin();
  USB.begin();
  Serial.println(F("[+] USB HID initialized"));
#else
  Serial.println(F("[!] Standard ESP32 — USB HID not available"));
  Serial.println(F("    Compile for ESP32-S2/S3 for USB HID"));
  Serial.println(F("    Serial commands available for testing"));
#endif

  Serial.println();
  Serial.println(F("╔══════════════════════════════════════════╗"));
  Serial.println(F("║   H10 — USB HID Emulator v1.0           ║"));
  Serial.println(F("║   Keyboard + Mouse Emulation            ║"));
  Serial.println(F("╚══════════════════════════════════════════╝"));
  printMenu();
}

// ── Loop ─────────────────────────────────────────────────────────
void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  int sp = line.indexOf(' ');
  String cmd = (sp > 0) ? line.substring(0, sp) : line;
  String args = (sp > 0) ? line.substring(sp + 1) : "";
  cmd.toLowerCase();

  if (cmd == "type") {
    cmdType(args);
  } else if (cmd == "key") {
    cmdKey(args);
  } else if (cmd == "combo") {
    cmdCombo(args);
  } else if (cmd == "macro") {
    cmdMacro(args);
  } else if (cmd == "mouse") {
    cmdMouse(args);
  } else if (cmd == "click") {
    cmdClick(args);
  } else if (cmd == "scroll") {
    cmdScroll(args);
  } else if (cmd == "jiggle") {
    cmdJiggle(args);
  } else if (cmd == "circle") {
    int r = args.length() > 0 ? args.toInt() : 20;
    if (r < 1) r = 20;
    mouseCircle(r, 60, 20);
  } else if (cmd == "payload") {
    cmdDuckyScript(args);
  } else if (cmd == "codes") {
    cmdCodes();
  } else if (cmd == "menu") {
    printMenu();
  } else {
    Serial.print(F("[?] Unknown: "));
    Serial.println(line);
  }
}
