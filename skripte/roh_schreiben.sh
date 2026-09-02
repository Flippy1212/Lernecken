#!/bin/sh

# Wird von der Home-Assistant-Automation "ESP-Daten empfangen"
# aufgerufen, sobald ein ESP per Webhook Daten sendet

#  WICHTIG: sh, nicht bash. Der Home-Assistant-Container ist
#  Alpine-basiert; bash ist dort nicht garantiert vorhanden

# TOKEN:
# Muss mit dem Wert im ESP uebereinstimmen
TOKEN_ERWARTET="TOKEN WAEHLEN"

if [ "$1" != "$TOKEN_ERWARTET" ]; then
# Falsches Token: nichts schreiben, Vorgang beenden
    echo "roh_schreiben: falsches Token, verworfen" >&2
    exit 1
fi

case "$2" in
  *[!A-Za-z0-9_-]*|"")
    echo "ungueltige ID: $2" >&2
    exit 1 ;;
esac

# ID pruefen
# ID ordnet ESP zur Lernecke zu
if [ -z "$2" ]; then
    echo "roh_schreiben: keine ID, verworfen" >&2
    exit 1
fi

# Zielordner liegt unter www/, damit die Rohdateien ggf. zu Fehlersuche 
# auch direkt im Browser einsehbar sind
mkdir -p /config/www/roh

# Datei schreiben
#Zeit auf UTC Zone festlegen
# das Alter aus und erkennt ausgefallene Lernecken
cat > /config/www/roh/$2.json << EOF
{"id":"$2","plaetze":[$3,$4,$5,$6],"zeit":"$(date -u -Iseconds)"}
EOF
