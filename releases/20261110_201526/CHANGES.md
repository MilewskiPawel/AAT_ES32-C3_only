# Firmware release 20261110_183840

Status: SUKCES (potwierdzone przez użytkownika)

## Zmiany w tej wersji
- Diagnostyka odbioru na OLED (lewy dół): `R E G L`
  - `R` — surowe wywołania odbioru ESP-NOW (`OnDataRecv`)
  - `E` — złożone pakiety MSP (ESP-NOW)
  - `G` — odebrane ramki CRSF GPS (`0x02`)
  - `L` — odebrane ramki CRSF Link Statistics (`0x14`)
- LQ% w prawym dolnym rogu (z CRSF Link Statistics, gdy docierają).
- Obsługa Link Statistics także ze ścieżki ESP-NOW/MSP (`MSP_ELRS_BACKPACK_CRSF_TLM` → `SendLinkTelemetry`) oraz liczenie GPS z `SendGpsTelemetry`.
- Ekran stanu: wyświetlanie końcówki UID oraz pełnego MAC/UID (`9C4FABDD2A58`) dla weryfikacji.
- UID na sztywno: `156,79,171,221,42,88`; domyślna sieć WiFi: `Fasolka` / `<moje-haslo_wifi>`.
- Usunięte domyślne definicje WS2812 z `include/pins.h` (brak aktywnego WS2812 dla env ESP32-C3 bez jawnej definicji).
- Uwagi: przy `R=0` brak pakietów ESP-NOW docierających do AAT (sprawdzić UID/tryb/forwarding telemetrii po stronie GX12/TX Backpack; dla CRSF zwykle Link mode = Normal).
