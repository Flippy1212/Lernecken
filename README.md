# Lerneckenprojekt - Belegungserkennung

Belegungserkennung für die Lernecken am Standort Fischerhof der Hochschule Hannover. Ein ESP-32 misst mit einem Infrarot-Sensor die Belegung der vier Sitzplätze einer Lernecke und schickt sie an Home Assistant. Eine öffentlich zugängliche Webseite zeigt an, welche Plätze frei (grün) oder besetzt (rot) sind.

**Live-Webseite:** https://hidd.christianwill.de/local/index.html
Einzelne Etage direkt: https://hidd.christianwill.de/local/index.html#/etage/1B.1

> Aktuell ist nur die Lernecke **1B.1.31** angebunden. Alle anderen Lernecken auf der Website sind Platzhalter.

## Funktionsprinzip (Kurzüberblick)

Ein ESP-32 mit AMG8833-Infrarot-Sensor teilt die 64 Pixel in vier Zonen (= vier Sitzplätze) auf und bestimmt pro Platz, ob er frei oder besetzt ist. Die Belegung wird per HTTPS-POST an einen Home-Assistant-Webhook geschickt, dort in einer `daten.json` gesammelt und von der Webseite dargestellt.

Zwischen ESP und Webseite steht dabei immer eine Datei (`daten.json`), sodass sich jeder Teil einzeln ändern und testen lässt.

## Dokumentation

**Alle wichtigen Informationen stehen im Handbuch** ([`lernecken-handbuch.pdf`](lernecken-handbuch.pdf)). Bitte vor Änderungen oder Inbetriebnahme dort nachlesen. Es beschreibt im Detail:

- **Hardware** – verwendete Bauteile und Montage
- **Datenweg** – wie die Daten vom ESP bis zur Webseite laufen
- **ESP-32 Funktionsweise** – IR-Sensor AMG8833 und die Belegungserkennung (Zustände EMPTY / CHANGING / HUMAN)
- **Home Assistant** – Skripte, Dateien, Automationen und `configuration.yaml`
- **Neue Lernecke hinzufügen** – Schritt für Schritt
- **Wichtige Werte zum Anpassen** – alle Parameter am ESP und in Home Assistant
- **Fehlersuche** – häufige Symptome, Ursachen und was zu prüfen ist

## Autoren

Mauro Holzki & Philippe Simons, Hochschule Hannover Fakultät 1