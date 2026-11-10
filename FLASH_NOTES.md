# ELRS Backpack AAT — Flashing & Pinout Reference Guide

## 1. Active Flash Summary (ESP8266 NodeMCU V3 — COM12)
- **Repository:** `D:\github\elrs_AAT3` (branch `AAT3plusAAT2_bez_debug`)
- **Environment:** `AAT_ESP_Backpack_via_UART`
- **Board:** `nodemcuv2` (ESP8266EX 4MB)
- **Port:** `COM12` (CH340)
- **Monitor speed:** `115200` baud
- **Status:** Built and flashed successfully. Verified boot log:
  ```text
  bootCounter = 2
  bindingMode = 0
  EEPROM MAC = 13,C0,36,C5,DF,48,
  Setup completed
  RequestVTXPacket...
  ```

### Active Pinout (NodeMCU V3 ESP8266)
| Function | NodeMCU Pin | GPIO | Notes |
|---|---|---|---|
| **OLED SDA** | D2 | GPIO4 | I2C Data (0x3C) |
| **OLED SCL** | D1 | GPIO5 | I2C Clock |
| **Servo Azimuth** | D5 | GPIO14 | Signal (5V power, common GND) |
| **Servo Elevation** | D6 | GPIO12 | Signal (5V power, common GND) |
| **Status WS2812** | D4 | GPIO2 | 4 LEDs (GRB) |
| **Button** | D3 / FLASH | GPIO0 | Standard Backpack button |

### OLED Mirror / Orientation Fix
```cpp
// Sent immediately after _display.begin(SSD1306_SWITCHCAPVCC, 0x3C):
_display.ssd1306_command(0xA0); // SEGREMAP normal (fixes L/R mirror)
_display.ssd1306_command(0xC8); // COM output scan direction: normal
```
*(If rotated 180°, set `-D OLED_MOUNT_ROTATED_180=1` which sends `0xA1` + `0xC0`).*

---

## 2. ESP32-C3 Target Configuration (Tianwen ESP32-C3 Pro / LuatOS OLED)
When switching back to the ESP32-C3 board with on-board 0.96" SSD1306 OLED:

### Target Pinout (ESP32-C3)
| Function | ESP32-C3 GPIO | Notes |
|---|---|---|
| **OLED SDA** | GPIO5 | Integrated on-board 0.96" OLED |
| **OLED SCL** | GPIO6 | Integrated on-board 0.96" OLED |
| **Servo Azimuth** | GPIO3 | Signal pin (5V power, common GND) |
| **Servo Elevation** | GPIO4 | Signal pin (5V power, common GND) |
| **Button** | GPIO9 | On-board BOOT button |

### Reflashing Commands

#### To reflash the NodeMCU V3 (ESP8266) on COM12:
```bash
cd D:\github\elrs_AAT3
python -m platformio run -e AAT_ESP_Backpack_via_UART -t upload
python -m platformio device monitor -p COM12 -b 115200
```

#### To flash the Tianwen ESP32-C3 Pro:
```bash
cd D:\github\elrs_AAT3
python -m platformio run -e AAT_ESP32C3_Backpack_via_UART -t upload --upload-port <COM_PORT>
python -m platformio device monitor -p <COM_PORT> -b 115200
```
