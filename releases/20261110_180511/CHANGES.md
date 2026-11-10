# Firmware release 20261110_180511

## Zmiany
- UID/binding address ustawiony na sztywno: ******** (domyślnie, po załadowaniu konfiguracji i w zapisie z konfiguratora).
- Domyślna sieć WiFi STA: `Fasolka` / `<moje-haslo_wifi>` (wersja konfiguracji VRX podbita, reset do domyślnych).
- Ekran OLED: zamiast VERSION wyświetlane są ostatnie 4 znaki ustawionego UID/binding phrase (np. "2,88" ***
***).
- Konfigurator (Antenna tracking): pole UID odczytywane z config.GetGroupAddress() i widoczne/edytowalne w UI (zapisywane przy Save na dole strony).
