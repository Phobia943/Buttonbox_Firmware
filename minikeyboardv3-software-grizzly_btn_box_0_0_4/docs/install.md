# Installation/Update
Das Vorgehen zum Installieren und Updaten ist dasselbe und wird nachfolgend beschrieben.

Wenn das von mir gebaute [Config-Format](config.md) verwendet werden soll dann folgendes Vorgehen:
- Im [git](https://git.schackmann.me/simon/minikeyboardv3-software/-/releases) den aktuellsten (oder gewünschten) Release runterladen
- Jeder Release hat bei `Assets` den Abschnitt `other` mit einem Downloadlink der Firmware
- Auf dem Pi Pico den [Bootsel Knopf](images/bootsel.png) gedrückt halten und dann per USB verbinden
- Der Pi wird als USB Laufwerk gestartet und (in Windows) automatisch im Explorer als Fenster geöffnet
- In dieses Fenster die `firmware.uf2` reinziehen und kurz warten
- Fertig der sollte jetzt alle USB Funktionen starten, das kann im Geräte Manager überprüft werden!
  - Den Gerätemanager öffnen (Windows Suche oder Rechtsklick auf Startlogo und dann Gerätemanager)
  - Unter Ansicht `Geräte nach Container` auswählen (siehe [hier](images/deviceManagerContainer.png))
  - Die Tastatur sollte als `Pico W` auftauchen und nach Aufklappen einige USB Funktionen enthalten (vergleiche [hier](images/deviceManagerDevice.png))

Fertig. Jetzt kann die config bearbeitet werden damit die Tastatur tut was sie tun soll. Hierzu einfach im [Config Kapitel](config.md) weiterlesen.

# Selbst Kompilieren
Soll der Code bearbeitet werden und anschließend auf den Pi geflashed werden geht das wie folgt:
- Der code wurde mit PlatformIO geschrieben. Um möglichst damit arbeiten zu können sollte PlatformIO installiert werden
  - Das geht (sehr einfach) mit `Visual Studio Code` und dem entsprechenden `PlatformIO` PLugin
  - Oder (auch recht einfach) mit `CLion` wie [hier](https://www.jetbrains.com/help/clion/platformio.html) beschrieben
  - Es gibt auch andere Editoren/Umgebungen aber das sind die beiden mit denen ich arbeite und von denen ich weiß dass sie funktionieren. Hier gilt im Zweifel gidf!
- Zu aller erst der Anleitung [hier](https://arduino-pico.readthedocs.io/en/latest/platformio.html) folgen
- Dann den Code aus dem Repo klonen oder im aktuellsten (oder gewünschten) Release den Source Code herunterladen
- Dann `platformio.ini` als Projekt öffnen und in der `platformio.ini` die Zeile `upload_port` anpassen (unter Windows Gerätemanager befragen, bei Mac/Linux: ¯\\_(ツ)_/¯)
- Mit dem entsprechenden Knopf oder mit `pio run -t upload` Kompilieren und Hochladen. Das kann beim ersten Mal ein Weilchen dauern
- Falls das nicht funktioniert muss wahrscheinlich erst der korrekte Treiber für den Pi Bootloader Modus installiert werden. Siehe zum Beispiel [hier](https://forums.raspberrypi.com/viewtopic.php?t=337274). **VORSICHT**: mit Zadiq kann man sich recht schnell Dinge zerschießen!
- Im Zweifel befindet sich nach dem Kompilieren die Firmware auch unter: `.pio/build/rpipicow` als `firmware.uf2`
- Im laufenden Betrieb kann der Serielle Output des Pi mit den Seriellen Konsole von Platformio betrachtet werden (oder mit `pio device monitor -p COMXX`)
- Soll mein Handling der Interaktionen ausgeschaltet werden muss lediglich die Funktion `device_callback` in der `main.cpp` überschrieben werden.
- Vorsicht: diese Funktion muss weiterhin existieren es sei denn mein Handling der Inputs soll ebenfalls entfernt werden. In diesem Fall wäre es aber wahrscheinlich einfacher einfach ein neues Projekt zu erstellen.

Soll geschriebener Code in "meine" Software übernommen werden bitte ein Issue erstellen, daraus einen Merge Request erstellen, alle Änderungen im entsprechenden Branch vornehmen und dann schaue ich mir das an und Merge entsprechend. 