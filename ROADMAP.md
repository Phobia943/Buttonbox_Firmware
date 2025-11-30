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

### 1. Firmware - Dynamische Hardware-Konfiguration (Jumper-Erkennung)
*   **Ziel:** Die Firmware soll zur Laufzeit erkennen können, ob ein Jumper gesteckt ist, um zwischen den "1 Funky + Encoder" und "2 Funky" Hardware-Setups umzuschalten.
*   **Problem:** Der genaue Pin für diesen Jumper auf dem PCB ist mir noch nicht bekannt. Die derzeitige Firmware-Struktur nutzt `#define` zur Kompilierzeit, was geändert werden muss, um eine dynamische Erkennung zu ermöglichen.
*   **Nächster Schritt (Feedback benötigt):** **Ich benötige den genauen Pin, der für den Jumper zum Umschalten zwischen den beiden Hardware-Setups verwendet wird.** Basierend auf dieser Information kann ich die Firmware anpassen, um diese dynamische Initialisierung zu implementieren (z.B. durch Umstellung auf `std::vector` für Input-Geräte und Lesen des Pins in `setup()`).

### 2. Konfigurations-Tool - Erweiterter Konfigurationseditor
*   **Ziel:** Statt nur das rohe JSON anzuzeigen, soll eine benutzerfreundliche Oberfläche zum Bearbeiten der Tastenbelegungen und visuellen Anpassungen mit Bildern erstellt werden.
*   **Nächster Schritt:**
    *   Entwicklung eines JSON-Parsers innerhalb der Python-App, um die Konfiguration in editierbare Datenfelder umzuwandeln.
    *   Implementierung einer visuellen Darstellung der Buttonbox (z.B. mit Bildern) im "Configuration"-Tab, auf der Tasten ausgewählt und ihre Aktionen zugewiesen werden können.
    *   Implementierung der "Save Config to Device"-Funktion, die die bearbeitete Konfiguration zurück an die Buttonbox sendet.

### 3. Firmware - SimHub Output-Kontrolle
*   **Ziel:** Die Buttonbox soll auf SimHub-Ausgangsdaten reagieren und externe Hardware (LEDs, Displays, etc.) steuern können.
*   **Nächster Schritt (Feedback benötigt):** Ich benötige Informationen über die **genaue Hardware (Typ, Anzahl, angeschlossene Pins)**, die von SimHub gesteuert werden soll (z.B. NeoPixel-LEDs für RPM, 7-Segment-Anzeigen für Gänge).

### 4. Weitere Firmware-Optimierungen (nach Bedarf)
*   **Ziel:** Jegliche weiteren Leistungs-, Speicher- oder Codequalitäts-Verbesserungen.
*   **Nächster Schritt:** Sobald die oben genannten Kernfunktionalitäten stehen, können wir spezifische Optimierungswünsche identifizieren und umsetzen.
