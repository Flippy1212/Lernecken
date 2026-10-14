#!/usr/bin/env python3

import json
import os
import sys
from datetime import datetime, timezone

ROH_ORDNER  = "/config/www/roh"
ZIEL        = "/config/www/daten.json"
AUSFALL_SEK = 300

def alter_in_sekunden(iso_zeit):
    try:
        gemessen = datetime.fromisoformat(iso_zeit)
        if gemessen.tzinfo is None:              #falls mal keine Zone
            gemessen = gemessen.replace(tzinfo=timezone.utc)
        return (datetime.now(timezone.utc) - gemessen).total_seconds()
    except Exception as e:
        print(f"sammeln: Zeitstempel '{iso_zeit}' nicht lesbar ({e!r})", file=sys.stderr)
        return 999999

lernecken = {}

# Sicherstellen, dass der Ordner existiert
if os.path.exists(ROH_ORDNER):
    for dateiname in os.listdir(ROH_ORDNER):
        if not dateiname.endswith(".json"):
            continue

        pfad = os.path.join(ROH_ORDNER, dateiname)
        try:
            with open(pfad, "r", encoding="utf-8") as f:
                roh = json.load(f)
        except Exception:
            continue

        ecken_id = roh.get("id")
        if not ecken_id:
            continue

        veraltet = alter_in_sekunden(roh.get("zeit", "")) > AUSFALL_SEK

        lernecken[ecken_id] = {
            "plaetze": None if veraltet else roh.get("plaetze"),
            "kontakt": roh.get("zeit"),
        }

ergebnis = {
    "aktualisiert": datetime.now(timezone.utc).isoformat(),
    "lernecken": lernecken,
}

# Atomar schreiben
tmp = ZIEL + ".tmp"
with open(tmp, "w", encoding="utf-8") as f:
    json.dump(ergebnis, f, indent=2, ensure_ascii=False)

os.replace(tmp, ZIEL)