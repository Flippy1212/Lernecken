# Nacharbeit Lerneckenhandbuch — Zusammenfassung der Änderungen

Antwort auf den Prüfbericht, was alle gemacht wurde ist hier beschrieben.

## A · Widersprüche Handbuch ↔ Code

- **A1** — Kein inhaltlicher Fehler: es war eine veraltete `automations.yaml` beigelegt. In Home Assistant läuft der korrekte Wert. Anmerkung: „war noch alte datei hochgeladen (in HA richtig)". Die aktuelle Datei wurde nachgereicht.
- **A2** — Analog A1: veralteter Sketch beigelegt. Anmerkung: „Alter code hochgeladen". Aktueller Code (`AVG_FRAMES 5`) nachgereicht.
- **A3** — Kein Widerspruch: der Direktsprung EMPTY→HUMAN ist im Handbuch beschrieben. Anmerkung: „Steht auch so im Handbuch, im nächsten Satz (3.2)".
- **A3b** — Betraf den veralteten Sketch. Anmerkung: „Alter Code". Im aktuellen Code wird `humanHold[z]` auch beim Direktsprung gesetzt, der Nachlauf entfällt nicht mehr.
- **A4** — Korrigiert: „rohschreiben.sh" → `roh_schreiben.sh`.
- **A5** — Korrigiert: „Auflösung des Sensors" (sollte eigentlich klar sein).
- **A6** — Quellenangabe ergänzt; das Adafruit-Datenblatt wird nun zitiert (Literaturverzeichnis).

## B · Inhaltliche Lücken

- **B1** — Hardware-Kapitel ergänzt (konkretes Board, Sensor).
- **B2** — Skizze eingefügt, die Sensorausrichtung, Zonennummern und Sitzplätze zusammenbringt. Einschränkung wie im Handbuch vermerkt: „nicht trivial da abhängig von Platzierung des Sensors, muss gegebenenfalls angepasst werden."
- **B3** — Zeitbasis der Erkennung ergänzt (Zyklusdauer, `HUMAN_HOLD` genauer beschrieben); der falsche Code-Kommentar „30 s in ms" bei `SEND_INTERVAL` wurde auf 15 s korrigiert.
- **B4** — Aufruf der Webseite und die Adressen (inkl. Etagen-Anker) in der Einleitung ergänzt.
- **B5** — Klarstellung statt Einrichtungskapitel. Anmerkung: „Das sollte eher individuell mit Prof Will besprochen werden, Arduino Einrichtung selber erarbeiten".
- **B6** — Pfad-Falle dokumentiert.
- **B7** — Fehlersuche-Kapitel ergänzt; Code aktualisiert (Zeitstempel auf UTC normalisiert, WLAN-Reconnect im Betrieb).
- **B8** — Aktueller Stand kurz beschrieben (nur `1B131` real installiert, übrige Einträge Platzhalter).
- **B9** — Klarstellung zum Geltungsbereich von Kapitel 5. Anmerkung: „Gemeint ist nur was an den Skripten angepasst werden muss, natürlich muss man Hardware kaufen etc."

## C · Sicherheit

- **C1** — Zugangsdaten (WLAN-Passwort, Token) durch Platzhalter ersetzt.
- **C2** — Webhook-Adresse durch einen zufälligen Wert ersetzt.
- **C3** — Bemerkung der Öffentlichkeit hinzugefügt.
- **C4** — ID-Filter in `roh_schreiben.sh` ergänzt (nur `[A-Za-z0-9_-]`), Pfad wird in Anführungszeichen geschrieben (in roh_schreiben.sh).
- **C5** — Bewusst nicht ins Handbuch aufgenommen. Anmerkung: „Für Handbuch nicht so wichtig".

## D · Fehlende Artefakte & Aufräumen

- **D1** — Eigentlich nicht vorgesehen? Ist ja ein fertiges PDF, welches abgegeben wird.       Andere Gruppen sollten ihr eigenes Dokument erstellen/erweitern.
- **D2** — Quelldatei des Zustandsdiagramms (Abb. 1) beigelegt.
- **D3** — Klarstellung: „Handbuch ist README".
- **D4** — Noch offen, Zuständigkeit Mauro. Siehe Abschnitt „Noch offen".
- **D5** — Stückliste ergänzt.
- **D6** — Anmerkung: „Ist ja auch kein GIT Projekt" — daher keine Repository-/Versionsangaben.
- **D7** — Drei `.DS_Store`-Dateien entfernt.
- **D8** — Defektes Zeichen in `roh_schreiben.sh` (Zeile 10, „übereinstimmen") auf sauberes UTF-8 korrigiert.
- **D9** — Widersprüchliche Code-Kommentare korrigiert (Sendeintervall 15 s; Token-Hinweis entfernt).

## Noch offen (Zuständigkeit Mauro)




- **D4 · Beispieldateien beilegen.** Dem Projektordner je eine Beispieldatei hinzufügen: eine erzeugte Rohdatei `roh/<ID>.json` und die zusammengefasste `daten.json`, die das Frontend liest. Das Handbuch zeigt bisher nur die ESP-JSON, nicht die beiden erzeugten Formate.
