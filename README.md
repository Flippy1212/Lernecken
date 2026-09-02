# Lerneckenprojekt ? Belegungserkennung

Belegungserkennung fr die Lernecken am Standort Fischerhof der Hochschule Hannover. Ein ESP-32 misst mit einem Infrarot-Sensor die Belegung der vier Sitzpl„tze einer Lernecke und schickt sie an Home Assistant. Eine ”ffentlich zug„ngliche Webseite zeigt an, welche Pl„tze frei (grn) oder besetzt (rot) sind.

**Live-Webseite:** https://hidd.christianwill.de/local/index.html
Einzelne Etage direkt: https://hidd.christianwill.de/local/index.html#/etage/1B.1

> Aktuell ist nur die Lernecke **1B.1.31** angebunden. Alle anderen Lernecken auf der Website sind Platzhalter.

## Funktionsprinzip (Kurzberblick)

Ein ESP-32 mit AMG8833-Infrarot-Sensor teilt die 64 Pixel in vier Zonen (= vier Sitzpl„tze) auf und bestimmt pro Platz, ob er frei oder besetzt ist. Die Belegung wird per HTTPS-POST an einen Home-Assistant-Webhook geschickt, dort in einer `daten.json` gesammelt und von der Webseite dargestellt.

Zwischen ESP und Webseite steht dabei immer eine Datei (`daten.json`), sodass sich jeder Teil einzeln „ndern und testen l„sst.

## Dokumentation

**Alle wichtigen Informationen stehen im Handbuch** ([`lernecken-handbuch.pdf`](lernecken-handbuch.pdf)). Bitte vor nderungen oder Inbetriebnahme dort nachlesen. Es beschreibt im Detail:

- **Hardware** ? verwendete Bauteile und Montage
- **Datenweg** ? wie die Daten vom ESP bis zur Webseite laufen
- **ESP-32 Funktionsweise** ? IR-Sensor AMG8833 und die Belegungserkennung (Zust„nde EMPTY / CHANGING / HUMAN)
- **Home Assistant** ? Skripte, Dateien, Automationen und `configuration.yaml`
- **Neue Lernecke hinzufgen** ? Schritt fr Schritt
- **Wichtige Werte zum Anpassen** ? alle Parameter am ESP und in Home Assistant
- **Fehlersuche** ? h„ufige Symptome, Ursachen und was zu prfen ist

## Autoren

Mauro Holzki & Philippe Simons, Hochschule Hannover Fakult„t 1
