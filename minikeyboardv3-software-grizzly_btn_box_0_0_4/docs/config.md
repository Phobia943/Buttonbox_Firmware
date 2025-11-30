# Was und wie
Die config für Minikeyboard V3 ist eine [`json`](https://www.json.org/json-en.html) Datei, in der alles was für den Betrieb eingestellt werden kann konfigurierbar ist.

Im `Examples` Ordner liegen Beispiele für (aktuell gültige) valide Config Dateien. Unter anderem eine leere Defaultconfig
die als Basis für eigene Configs genutzt werden kann.

**VORSICHT** die Config Formate (Versionsnummern als erster Eintrag in der Datei) sind nicht unbedingt kompatibel mit 
anderen Versionen der Firmware! Weder abwärts noch aufwärts. Aktuell wird der Eintrag nicht geprüft, sondern dient
lediglich als Erinnerung. Wird eine inkompatibel Config Version auf die Tastatur gespielt passiert im besten Fall nichts
im schlimmsten Fall stürzt alles ab! Falls das passiert hilft nurnoch die Firmware neu aufzuspielen und vorher den EEPROM
zu [leeren](https://forum.micropython.org/viewtopic.php?t=10838).

Die Config sehen recht kompliziert aus, aber wenn immer nur abschnittsweise gearbeitet wird recht gut kontrollierbar.
Wichtig ist ein Editor der prüft ob der resultierende JSON syntaktisch korrekt ist! VS Code zeigt zum Beispiel Fehler
in der Datei an.

Ein großer Nachteil am JSON Format ist, das keine Kommentare unterstützt werden!

## Ablauf
Die Config wird bei Programmstart (also beim Einstecken der Tastatur) einmalig gelesen und in interne Objekte umgewandelt,
die aktuell nicht änderbar sind. Die Config kann im laufenden Betrieb mit dem `__RELOAD__` Befehl über die Serielle
Konsole ebenfalls neu geladen werden.

Für alle Knöpfe und Encoder (Sub ID) eines logischen übergelagerten Geräts (Funkyswitch, Keymatrix --> ID) können
Listen aus Aktionen festgelegt werden. Eine Aktion ist ein ausführbares Element für die Tastatur. Eine Aktion kann
potentiell auch selbst wieder eine oder mehrere Aktionen enthalten. Wenn die config korrekt geladen wurde, werden im 
laufenden Betrieb bei einer Eingabe ID und sub ID in der Liste der Aktionen gesucht und falls gefunden ausgeführt.
Falls für eine ID - sub ID Kombination keine Aktionen definiert sind passiert nichts.

## Aufbau
Diese Beschreibung gilt für die aktuelle Version `1.1` potentiell alles hieran könnte sich in zukünftigen Releases ändern...
Im Zweifel immer die aktuellen Beispiel im `examples` Ordner konsultieren! Ich werde versuchen die Doku uptodate zu halten
aber die Hauptpriorität liegt aktuell eher auf der Arbeit an Features und Dokumentation fällt dabei sehr gerne unter
den Tisch.

Eine gültige Config Datei enthält 1 JSON Objekt mit folgendem Inhalt (Key: Value):
- `version`: aktuelle Versionsnummer der config. Wird im Moment nicht ausgewertet steht eher als Information für den User drin.
- `entities`: Enthält Übersetzungen von Namen zu interner ID für Inputgeräte. Bei der Minitastatur gibt es hier nur
`Funkyswitch` (der Encoder) sowie `Keymatrix` die eigentlichen Tasten
- `sub_entities`: Enthält Übersetzungen von Namen zu interner sub ID für Inputgeräte. Bei der Minitastatur vor allem
für den Encoder sinnvoll, damit mit Namen wie `left|right|up|down|center` gearbeitet werden kann statt der entsprechenden
IDs als Zahlen
- `templates`: Enthält wiederverwendbare Abfolgen von Aktionen, die an mehr als einer Stelle eingesetzt werden sollen
ohne Teile der Config mehrfach schreiben zu müssen.
- `pages`: Enthält die eigentliche Konfiguration der Aktionen. Aktuell wird nur eine einzige Page unterstützt, 
das wird sich in Zukunft aber ändern

## Actions
Im Folgenden werden Actions, deren Aufbau und Verwendung beschrieben. Ich versuche diese Liste aktuell zu halten aber
wie oben beschrieben im Zweifel sind die Beispiele aktueller als die Config.

Achtung: momentan sind alle Actions blocking. Es kann also immer nur eine Action aktiv sein. Während dieser Zeit 
werden auch keine weiteren Tastendrücke erkannt! Das wird in der Zukunft geändert ist aktuell aber eine Limitation
der Software (nicht der Hardware wie bei V1).

### Einzelne Tasten 
```json lines
{"push":  "A"}
{"press":  "a"}
{"release":  "a"}
```
Die oben abgebildeten Config Zeilen sind die 3 grundlegend möglichen Einzeltastenaktionen. `Push` drückt eine Taste
und lässt sie sofort wieder los, `Press` hält die Taste gedrückt und Release lässt eine vorher gedrückte Taste los.
Ein `Push` ist also quasi ein `Press` mit sofort folgdendem `Release`. Nach dem Keywort kann ein einzelner Buchstabe
folgen wie in den Beispielen (Groß- und Kleinschreibung werden beachtet!) oder ein Codewort wie `enter|esc` usw...
Eine vollständige Liste aller möglichen Codeworte befindet sich [hier](codewords.md).

Weiterhin können die Tasten auch mit Modifikatoren gedrückt werden wie in folgendem Beispiel:
```json lines
{"push":  {"key":  "s", "modifier":  "ctrl"}}
{"push":  {"key":  "s", "modifier":  "ctrl+shift"}}
```
Wichtig: die Taste die gedrückt werden soll landet zusammen mit dem Modifikator (bzw den Modifikatoren) in einem eigenen
JSON Objekt! Alle möglichen Modifikatoren sind ebenfalls [hier](codewords.md) aufgelistet. Modifikatoren werden mit
einem `+` OHNE Leerzeichen verknüpft, falls mehr als einer benötigt wird.

Wird als Key `G0-G31` angegeben wird die entsprechende Gamepad Taste gedrückt (Game Controller)

### Text
Es kann ein kompletter String geschrieben werden wie folgt. Groß und Kleinschreibung werden soweit möglich beachtet.
````json
{"write":  "hello world"}
````

### Wait
Die `Wait` Aktion verzögert die weitere Ausführung um X Millisekunden.
````json
{"wait":  250}
````

### Maus
Die Mausaction handelt jegliche Form von Maus Output. Hier wird sich in der Zukunft wahrscheinlich noch einiges tun,
da ich mit der momentanen Implementierung nicht zufrieden bin.

````json
{"mouse":  {"x":  <deltaX>, "y":  <deltaY>, "wheel":  <deltaWheel>, "click": "left|right|middle", "release": "left|right|middle"}}
````
Alle Felder sind optional und es müssen nur diejenigen angegeben werden die auch genutzt werden sollen! Alles andere
wird automatisch auf 0/neutral gesetzt.

Vorsicht: `click` hält die Taste gedrückt bis `release` aufgerufen wird!
X und Y Bewegungen werden **NICHT** als eine einzelne Bewegung (ein Sprung) ausgeführt, da Windows das scheinbar nicht
mag und die USB Implementierung auch nicht mehr als 127 bzw -128 Mausinkremente zulässt (das ist nicht viel!).
deltaX und deltaY können trotzdem "beliebig" groß angegeben werden und die Aktion führt die Bewegung in entsprechend
vielen 10er Schritten aus (das ist zwar langsam funktioniert aber gut und wiederholbar im Gegensatz zu größeren
Inkrementen)

Bei einem Rework der Maus wird zusätzlich vermutlich eine Bool Option `"absolute": true|false` hinzugefügt.

### LED
Die Led Aktion verändert die Farbe der beteiligten (Sub)Entity. Auch hier wird es ein Rework geben um die Sache
flexibler zu gestalten!

````json lines
{"led":  "active"}
{"led":  "inactive"}
{"led":  "red|green|yellow|blue|white|off"}
````
Active sowie Inactive Color sind aktuell global hardcoded auf grün bzw weiß. Das wird in der Zukunft änderbar sein!
Auch die Liste der wählbaren Farben ist hardcoded und muss noch änderbar gemacht werden! Wird für eine Subentity keine
einzige LED Action erstellt, werden automatisch eine Active Action beim Drücken und eine Inactive Action beim Loslassen
erstellt.

### Template Action
Eine Template Action ruft eine in der `template` Sektion definierte Abfolge von Actions auf. Wenn das entsprechende
Template nicht definiert ist wird ein Fehler geloggt und es passiert nichts.
````json
{"template":  "templateName"}
````
Achtung: der Value String muss exakt(!) dem Namen des Templates entsprechen!

### Conditions
Um das Ausführverhalten zur Laufzeit ändern zu können sind Conditions enthalten. Hierzu gibt es 2 möglich Actions.
Die `conditional` Action die den Zustand einer Condition verändert, sowie die `ifCondition` die ensprechend dem 
aktuellen Zustand der Condition Actions ausführt bzw nicht ausführt.

Die zugrundeliegenden Variablen -Conditionals- sind global und stehen grundlegend allen Actions zur Verfügung.
Existiert noch kein Conditional mit dem entsprechenden Namen bei einer `ifCondition` wird sie automatisch mit `false`
erstellt und initialisiert. Conditionals werden ausschließlich mit ihrem Namen (string) identifiziert.

Der Wert eines Conditionals kann wie folgt geändert werden.
````json
{"conditional":  {"name":  "CondName", "newState":  "set|unset|toggle", "stateLed":  ledNumber}}
````
Das `stateLed` Feld ist optional und kann verwendet werden um eine LED (für Indices [hier](hardware.md) schauen) 
entsprechend dem Zustand der Conditional Variable zu setzen (default: rot und grün für false und true)

Soll ein Conditional verwendet werden um eine Entscheidung zu treffen gibt es bis jetzt lediglich folgende Möglichkeit:
````json
{
  "ifCondition": {
    "conditional": "condName",
    "true": [],
    "false": []
  }
}
````
In `true` und `false` können anschließend beliebig viele Actions definiert werden die ausgeführt werden sollen. Wird 
nur `true` oder nur `false` benötigt, kann der entsprechend andere Eintrag einfach weggelassen werden. Existiert das
Conditional noch nicht wird es erstellt und `false` wird ausgeführt.

### Repeat Action
Soll eine Action mehrfach ausgeführt werden kann die Repeat Action genutzt werden um eine (einzige) andere Action
mehrfach aufzurufen.
````json
{
  "repeat": {
    "times": X,
    "interval": intInMS,
    "action": {"push":  "a"}
  }
}
````
In Zukunft wird Repeat wohl erweitert werden auch mehrere Actions mehrfach auszuführen. Aktuell könnte hierfür auch
eine Template Action verwendet werden.

Vorsicht: Repeat Actions sind (wie alle anderen Actions auch) im Moment Blocking! Während sie aktiv ist kann nichts
anderes getan werden. Keine Tastendrücke werden erkannt und keine anderen Aktionen ausgeführt!

## Templates
Ein Template ist wie folgt aufgebaut:
````json
{
  "templateName": {
    "actions": [
      {"push":  "a"},
      {"push":  "b"}
    ]
  }
}
````

Dieses Format wird in der Zukunft höchstwahrscheinlich erweitert werden. Die Liste an Actions kann auch andere Templates
enthalten! Aber nur wenn diese vorher definiert wurden! Die Reihenfolge wie Templates in der config stehen spielt 
hierbei eine Rolle.

## Pages
Der Aufbau der Pages ist wie folgt:
````json
{
  "pages": [
    {page0},
    {page1},
    {page2}
  ]
}
````
Wobei eine Page "von innen" wie folgt aussieht:
```json
{
  "entityIDorName": {
    "subEntityIDorName": {
      "press": [actions],
      "release": [actions],
      "increase": [actions],
      "decrease": [actions]
    }
  }
}
```
Hinweis: `increase` und `decrease` existieren nur für Encoder!

## Ergänzungen zu den Beispielen
TODO






































