import PyInstaller.__main__
import customtkinter
import os
import platform

# Pfad zu CustomTkinter finden (wichtig fuer Themes/Bilder)
ctk_path = os.path.dirname(customtkinter.__file__)

# Separator fuer --add-data ist abhaengig vom OS (; fuer Windows, : fuer Linux)
separator = ';' if platform.system() == "Windows" else ':'

print(f"Building EXE on {platform.system()}...")
print(f"CustomTkinter path: {ctk_path}")

PyInstaller.__main__.run([
    'main.py',                       # Dein Haupt-Skript
    '--name=ButtonboxTool',          # Name der EXE
    '--onefile',                     # Alles in eine Datei packen
    '--windowed',                    # Kein Konsolenfenster anzeigen
    f'--add-data={ctk_path}{separator}customtkinter', # CTK Daten einbinden
    f'--add-data=firmware{separator}firmware',       # Firmware Dateien einbinden
    '--clean',                       # Cache vorher leeren
    '--noconfirm',                   # Bestehende Ordner ueberschreiben
])

print("\nBuild abgeschlossen! Die Datei liegt im Ordner 'dist'.")
