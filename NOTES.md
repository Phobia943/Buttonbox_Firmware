# Projekt-Notizen: Buttonbox Firmware Optimierung & SimHub

## Ziel
Optimierung der Firmware für ein Custom PCB mit Raspberry Pi Pico und Integration von SimHub-Kompatibilität.

## Quelle
*   **Repository:** `https://git.schackmann.me/simon/minikeyboardv3-software`
*   **Branch:** `buttonbox`
*   **Status:** Repository aktuell offline.

## Geplante SimHub-Integration

### 1. Kommunikation (PC -> Buttonbox)
SimHub sendet Telemetriedaten (für LEDs, Displays, Rumble-Motoren) an den Pico.
*   **Schnittstelle:** USB Serial (CDC).
*   **SimHub Feature:** "Custom Serial Profile".
*   **Protokoll-Idee:** Textbasiertes Format für einfache Fehlersuche.
    *   Beispiel: `RPM=5000;GEAR=3;FLAG=BLUE\n`
    *   Parser auf dem Pico liest bis `\n` und wertet die Befehle aus.

### 2. Eingabe (Buttonbox -> PC)
Die Tasten der Box sollen Aktionen am PC auslösen.
*   **Methode:** Beibehaltung (oder Implementierung) von USB HID (Joystick oder Keyboard).
*   **Vorteil:** Wird von Windows/SimHub nativ als Controller erkannt. Keine spezielle Serial-to-Key Software nötig.

## Optimierungs-Ideen (Vorbereitung)
*   **Debouncing:** Überprüfung der Entprell-Logik der Tasten für schnellere Reaktionszeiten ohne Geister-Eingaben.
*   **Loop-Effizienz:** Sicherstellen, dass der Serial-Read nicht blockiert und die Tastenabfrage verzögert.
*   **Struktur:** Trennung von Matrix-Scan-Logik und Serial-Kommunikation.

## Nächste Schritte
1.  Repository klonen, sobald verfügbar.
2.  Code-Analyse (Sprache: C++ SDK vs. MicroPython/CircuitPython?).
3.  Implementierung des Serial-Parsers.
