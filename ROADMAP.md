# Buttonbox Firmware & Config Tool - Aktueller Status und Roadmap

Dies ist eine Zusammenfassung des aktuellen Projektstands und der geplanten nächsten Schritte.

## Aktueller Status

### A. Firmware (im Ordner `minikeyboardv3-software-grizzly_btn_box_0_0_4`)
*   **Codebasis analysiert:** Raspberry Pi Pico W, Arduino Framework, `earlephilhower` Core.
*   **SimHub Kompatibilität (Eingang) implementiert:**
    *   Der serielle Kommunikationscode (`src/InterfaceProvider.cpp`) wurde auf nicht-blockierendes Lesen umgestellt, um die Performance zu verbessern und Konfigurationen sowie SimHub-Befehle (Format `KEY=VALUE`) zu verarbeiten.
    *   Wichtige Debug-Ausgaben (`Serial.print`) in `src/app_config_handler.cpp`, die bei jeder Aktion geloggt wurden, sind nun auskommentiert, um den seriellen Bus für SimHub frei zu halten.
*   **Hardware Konfiguration:** Die Umschaltung zwischen "1 Funky Switch mit zusätzlichen Encodern" und "2 Funky Switches" ist derzeit auf Kompilierungszeit (`#define`) festgelegt. Eine dynamische Erkennung mittels Jumper ist geplant.

### B. Konfigurations- und Update-Tool (im Ordner `ConfigTool`)
*   **Basis-GUI:** Eine Python-Desktop-Anwendung (`customtkinter`) wurde erstellt, die serielle Ports erkennt und eine Verbindung herstellen kann.
*   **Firmware Update Funktionalität:**
    *   Ein Tab "Firmware Update" ermöglicht das Auswählen einer `.uf2` Firmware-Datei.
    *   Ein Button sendet den Befehl `__BOOTLOADER__` an die Buttonbox, um sie in den Bootloader-Modus zu versetzen.
    *   Das Tool versucht, das "RPI-RP2" Laufwerk zu finden und kopiert die `.uf2` Datei dorthin.
*   **Konfigurations-Anzeige:** Ein Tab "Configuration" kann die rohe JSON-Konfiguration (mittels `__PRINT_CONFIG__` Befehl) vom Gerät laden und anzeigen.
*   **Setup:** Ein Python Virtual Environment (`venv`) wurde eingerichtet und die notwendigen Bibliotheken (`customtkinter`, `pyserial`) installiert.
*   **Dokumentation:** Eine `README.md` für das Tool wurde erstellt.
*   **Git:** Alle bisherigen Änderungen wurden in dein GitHub-Repository gepusht.

---

## Roadmap (Nächste Schritte)

### 1. Firmware - Hardware-Konfiguration über Build-Umgebungen
*   **Status:** **IMPLEMENTIERT**.
*   **Beschreibung:** Da der Jumper zur Laufzeiterkennung von Hardware-Setups in Version 1.1 entfallen ist, wurde die Strategie geändert. Es wurden separate Build-Umgebungen in `platformio.ini` erstellt (z.B. `env:minikeyboard`, `env:buttonbox_1funky`, `env:buttonbox_2funky`). Jede Umgebung definiert den benötigten `BOARD_TYPE_...` während des Kompilierens.
*   **Vorteil:** Dies ermöglicht optimierte Firmware-Builds für jede Hardware-Variante und vermeidet unnötigen Code oder dynamische Erkennungslogik zur Laufzeit.
*   **Nächster Schritt:** Im Konfigurations-Tool sollte die Möglichkeit geschaffen werden, die korrekte Firmware für die entsprechende Hardware-Variante auszuwählen und zu flashen.

### 2. Konfigurations-Tool - Erweiterter Konfigurationseditor
*   **Status:** **IMPLEMENTIERT (Basis-Funktionalität inkl. Config-Generator)**
*   **Beschreibung:**
    *   Der serielle Kommunikationscode in `ConfigTool/main.py` wurde überarbeitet, um die gesamte JSON-Konfiguration vom Gerät zu laden.
    *   Eine interne JSON-Parsing-Logik wurde hinzugefügt, um die empfangene Konfiguration in Python-Datenstrukturen zu konvertieren.
    *   Eine rudimentäre visuelle Darstellung der geladenen Konfiguration (`self.config_data`) wird nun im "Configuration"-Tab angezeigt, indem die Top-Level-Sektionen (`pages`, `templates`, `entities`) und ihre Inhalte zusammengefasst werden.
    *   Die Funktion zum Speichern der Konfiguration auf dem Gerät über die serielle Schnittstelle wurde implementiert.
    *   **NEU: Config-Generator:** Eine Benutzeroberfläche mit Dropdown-Menüs wurde hinzugefügt, die es dem Benutzer ermöglicht, eine leere JSON-Konfiguration basierend auf der Anzahl der gewünschten Funky Switches, Encoder und Buttons zu generieren. Dies erleichtert die initiale Einrichtung eines Boards erheblich.
*   **Nächster Schritt:** Weiterentwicklung des visuellen Editors, um das Bearbeiten von Tastenbelegungen und Aktionen direkt über die GUI zu ermöglichen. (Dies wäre der nächste Schritt, wenn der Punkt weiterentwickelt wird.)

### 3. Firmware - SimHub Output-Kontrolle
*   **Ziel:** Die Buttonbox soll auf SimHub-Ausgangsdaten reagieren und externe Hardware (LEDs, Displays, etc.) steuern können.
*   **Nächster Schritt (Feedback benötigt):** Ich benötige Informationen über die **genaue Hardware (Typ, Anzahl, angeschlossene Pins)**, die von SimHub gesteuert werden soll (z.B. NeoPixel-LEDs für RPM, 7-Segment-Anzeigen für Gänge).

### 4. Weitere Firmware-Optimierungen (nach Bedarf)
*   **Ziel:** Jegliche weiteren Leistungs-, Speicher- oder Codequalitäts-Verbesserungen.
*   **Nächster Schritt:** Sobald die oben genannten Kernfunktionalitäten stehen, können wir spezifische Optimierungswünsche identifizieren und umsetzen.
