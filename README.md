# Buttonbox Firmware & Tools

Dieses Repository enthält die Firmware und Konfigurationstools für das Grizzly Buttonbox Projekt. Es besteht aus zwei Hauptkomponenten:

1.  **Firmware (`minikeyboardv3-software-...`)**: Arduino/PlatformIO Firmware für den Raspberry Pi Pico W.
2.  **ConfigTool (`ConfigTool/`)**: Eine Python/CustomTkinter GUI zur Verwaltung des Geräts und der Konfigurationen.

## Ordnerstruktur

- **`minikeyboardv3-software-grizzly_btn_box_0_0_4/`**: Quellcode für die Firmware.
- **`ConfigTool/`**: Python-basiertes GUI-Tool zur Konfiguration der Buttonbox.

## Firmware

Die Firmware wird mit PlatformIO erstellt und unterstützt verschiedene Board-Konfigurationen (MiniKeyboard, ButtonBox mit 1 oder 2 Funky-Switches).

### Schnellstart (Firmware)

1.  In das Firmware-Verzeichnis wechseln: `cd minikeyboardv3-software-grizzly_btn_box_0_0_4`
2.  Projekt bauen: `pio run`
3.  Firmware hochladen: `pio run -t upload`

## ConfigTool

Das ConfigTool ermöglicht die einfache Zuweisung von Hardware-Eingaben zu Aktionen über eine grafische Oberfläche.

### Schnellstart (Tool)

1.  In das Tool-Verzeichnis wechseln: `cd ConfigTool`
2.  Abhängigkeiten installieren: `pip install -r requirements.txt`
3.  Anwendung starten: `python main.py`
