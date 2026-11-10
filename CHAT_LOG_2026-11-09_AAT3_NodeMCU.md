# Chat log / notatki sesji — elrs_AAT3 (NodeMCU V3 ESP8266, AAT)

**Data:** 2026-11-08/09 (noc)  
**Repo lokalne:** `D:\github\elrs_AAT3`  
**GitHub:** https://github.com/MilewskiPawel/elrs_AAT3 (konto `MilewskiPawel`)  
**Płytka docelowa (finalnie):** ESP NodeMCU V3 (ESP8266, ESP-12E), port **COM9** (CH340)  
**Gałąź robocza (aktualna):** `AAT3plusAAT2_bez_debug`

> Uwaga: w trakcie sesji była też faza pod Tianwen ESP32-C3 Pro / COM6 (gałąź `master` ma historię początkową), ale finalna konfiguracja to NodeMCU V3 na COM9, LEDy na D4.

---

## 1. Wymagania sprzętowe (wdrożone)
- **WS2812:** 4 diody, sygnalizacja statusu. Na NodeMCU V3: **D4 = GPIO2** (`PIN_WS2812=2`, `WS2812_COUNT=4`).
- **OLED SSD1306 128×64, I2C, addr 0x3C.** NodeMCU V3 (bezpieczne piny): **SDA=D2 (GPIO4)**, **SCL=D1 (GPIO5)**. *(Remap z ESP32-C3: GPIO6 to flash na 8266, niedostępny).*
  - **Fix lustra L/P (WYMAGANY)** — po `display.begin(SSD1306_SWITCHCAPVCC, 0x3C)` wysłać komendy:
    - montaż normalny: **`0xA0`, `0xC8`** (SEGREMAP normal, COM scan normal)
    - montaż 180°: **`0xA1`, `0xC0`** (SEGREMAP remap, COM scan remapped)
  - Sterowane flagą `OLED_MOUNT_ROTATED_180` (0/1).
- **Serwa (sygnał, 5 V, wspólna masa):** AZ = **D5 (GPIO14)**, EL = **D6 (GPIO12)**. *(Remap: GPIO3 to UART0 RX na 8266, konflikt z Serial).*
- **Build/flash/monitor:** PlatformIO, `python -m platformio ...`, COM9, monitor 115200 (zależnie od trybu; patrz gałęzie).

## 2. Gałęzie / co zawierają
| Gałąź | Opis |
|---|---|
| `master` | Minimalny firmware (ESP8266): OLED fix, WS2812 D4, serwa, **WiFi AP config** `ELRS_AAT_<ID>` (open) → http://192.168.4.1/, EEPROM (orientacja OLED, kalibracja serw). **Brak** stosu ELRS/Backpack bind. Commity m.in. `77ab119`, `3379807`. |
| `AAT3plusAAT2` | Pełny **ELRS Backpack AAT** (baza z `elrs_AAT2`) + target NodeMCU V3. **Debug:** `VRX_UART_BAUD=115200`, `DEBUG_LOG`, `DEBUG_ELRS_WIFI` (wymusza AP przy starcie). Sieć: **`ExpressLRS AAT Backpack`**, hasło `expresslrs`, http://10.0.0.1/ (mDNS `elrs_aat.local`). OLED fix w `src/module_aat.cpp`. Commity `d942484`, `397cd49`, `627cb2f`, `955a5c9`. |
| `AAT3plusAAT2_bez_debug` (**aktualna**) | Normalny **AAT/Backpack (ESP-NOW)**, bez wymuszonego AP, bez `DEBUG_LOG` (Serial współdzielony z AAT/CRSF). Do bindowania wg binding phrase/UID z TX/Backpackiem. Commity `6cbc6ee`, `b223110`, `f061493` (+ lokalny stan użytkownika może mieć tymczasowe flagi). |

## 3. Pełny Backpack AAT — target i piny (gałęzie `AAT3plusAAT2*`)
**Env:** `AAT_ESP_Backpack_via_UART` (`targets/aat.ini`), `nodemcuv2`, `eagle.flash.4m2m.ld`, espressif8266@4.2.1, COM9.
- `TARGET_VRX_BACKPACK`, `AAT_BACKPACK`, `SSD1306_NO_SPLASH`
- `PIN_BUTTON=0`, `PIN_WS2812=2`, `WS2812_COUNT=4`
- `PIN_SERVO_AZIM=14`, `PIN_SERVO_ELEV=12`
- `PIN_OLED_SDA=4`, `PIN_OLED_SCL=5`, `OLED_MOUNT_ROTATED_180=0`

**Kod fixu OLED (Backpack):** `src/module_aat.cpp` → `displayInit()` po `_display.begin(...)` wysyła `0xA0/0xC8` lub `0xA1/0xC0`.
**Wejście:** `src/Vrx_main.cpp` (AAT: `AatModule vrxModule(Serial)`).

**Build/flash:**
```bash
cd D:\github\elrs_AAT3
python -m platformio run -e AAT_ESP_Backpack_via_UART -t upload
python -m platformio device monitor -p COM9 -b 115200   # zależnie od baud/trybu
```

## 4. LEDy (zależnie od firmware)
- **`master` (AP config):** BOOT bursztyn mruga; **CONFIG (AP aktywny) cyjan mruga**; OK zieleń stała; NO_GPS niebieski mruga; ERROR czerwony mruga.
- **`AAT3plusAAT2*` (Backpack):** sterowane przez `devLED`/Backpack zgodnie ze stanem urządzenia (`PIN_WS2812` skonfigurowany w targecie).

## 5. Bindowanie ELRS/Backpack (ważne)
- Minimalny firmware `master` **nie** binduje się z ELRS (brak radia/ESP-NOW Backpack stack) — ma tylko lokalny AP.
- Pełny Backpack AAT używa **ESP-NOW**; UID/adres musi być zgodny (binding phrase → UID). **VRX web UI w tej wersji nie ma pola Binding Phrase.**
- **UID** = MD5 z `-DMY_BINDING_PHRASE="<phrase>"`, pierwsze 6 bajtów (hash jak ELRS).
- Dodano w `src/options.cpp`: wyliczanie UID z `MY_BINDING_PHRASE` gdy UID nieustawiony/**lub zapisany jako same zera** (żeby nadpisać błędny/zerowy stan). **Nie commitować prawdziwej frazy do publicznego repo.**

**Sprawdzenie UID lokalnie (PowerShell, bez ujawniania frazy):**
```powershell
python -c "import hashlib,sys; p=sys.argv[1]; s='-DMY_BINDING_PHRASE='+chr(34)+p+chr(34); print(','.join(f'{b:02X}' for b in hashlib.md5(s.encode()).digest()[:6]))" "WKLEJ_FRAZE"
```
Wynik porównać z `EEPROM MAC = ...` w logu.

## 6. Stan na koniec sesji (diagnostyka)
- Z logu (tryb debug do odczytu): `bootCounter = 1`, `bindingMode = 0`, `EEPROM MAC = 13,C0,36,C5,DF,48`, `Setup completed`, `RequestVTXPacket...`
- **Problem wykryty:** UID z komendy dla „właściwej” frazy użytkownika wyszedł `AE,BD,61,AD,43,D2`, a AAT ma `13,C0,36,C5,DF,48` → **UID/phrase niezgodne** z TX. Trzeba przebudować AAT z właściwą frazą (tymczasowo `-D MY_BINDING_PHRASE=\"...\"` lokalnie, bez commitu), wgrać, potwierdzić `EEPROM MAC = AE,BD,61,AD,43,D2`. Jeśli nadal stary MAC — może być zapisany niezerowy UID w config/SPIFFS; wtedy reset konfiguracji lub `-t erase` + upload.
- `RequestVTXPacket...` bez `Failed MAC add check`/pakietów = brak poprawnie sparowanego ruchu z TX/Backpackiem (często zły UID po którejś stronie lub brak nadajnika).

**Tymczasowy debug do weryfikacji połączenia (bez `DEBUG_ELRS_WIFI`):** dodać lokalnie `-D VRX_UART_BAUD=115200 -D DEBUG_LOG` (ew. `DEBUG_LOG_VERBOSE`), monitor 115200. Szukać: zgodny `EEPROM MAC`, brak `Failed MAC add check and not in bindingMode.` przy przychodzących pakietach; `Processing MSP_...` / `MSP_ELRS_BIND`. Uwaga: AAT używa Serial do CRSF — log może być zaszumiony.

## 7. Lokalny stan git (uwaga na jutro)
Przed sesyjnymi zmianami widziano lokalne modyfikacje m.in. `targets/aat.ini`, `lib/LED/devLED.cpp` (mogą zawierać tymczasowe flagi/frazę użytkownika). **Nie pushować frazy.** Przed pracą: `git status`, `git diff`.

## 8. Następne kroki (jutro)
1. Ustawić w AAT **właściwą** frazę (tę dającą `AE,BD,61,AD,43,D2`) lokalnie, rebuild+upload (`AAT3plusAAT2_bez_debug`).
2. Zweryfikować `EEPROM MAC` w logu; jeśli nie zmienia się — wyczyścić zapisany UID/config (`erase` lub reset).
3. Potwierdzić tę samą frazę na TX/TX-Backpack (Configurator).
4. Sprawdzić odbiór pakietów ESP-NOW (brak `Failed MAC add check`, obecne `Processing MSP...`/ruch AAT).
5. Usunąć frazę/tymczasowe debug flagi z plików śledzonych, przywrócić czysty build.

---
*Zapisano automatycznie na prośbę użytkownika (koniec sesji).*