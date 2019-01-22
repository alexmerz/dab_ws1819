# Dokumentation Firmware

Die Roboter akzeptieren über die serielle Schnittstelle/Bluetooth eine Reihe von Textkommandos bestehend aus einem Kommandowort und bis zu drei Parametern. Das Kommandowort wird immer groß geschrieben, die einzelnen Elemente werden mit einem einzelnen Leerzeichen voneinander getrennt. Ein Kommando muss immer mit einem Zeilenumbruch "\n" abgeschlossen werden. Zum Beispiel:
"FORWARD 2000 100\n"

Wurde der Befehl korrekt erkannt, liefert die Schnittstelle ein OK + Zeilenumbruch zurück. Konnte der Befehl nicht erkannt werden, liefert die Schnittstelle ein NOK + Zeilenumbruch zurück.

Die Geschwindigkeit der seriellen Schnittstelle (nicht Bluetooth!) beträgt 9600 Baud.

Zeitangaben müssen in Millisekunden erfolgen (1s = 1000ms). Die Speed-Angabe hat einen zulässigen Wertebereich von 0 - 255, Werte ca. unter 50 dürften keine sinnvolle physische Aktion zur Ursache haben.

## Kommandos

**FORWARD \<Zeit\> \<Speed\>**

Nach von bewegen

**BACK \<Zeit\> \<Speed\>**

Nach hinten bewegen

**LCIRCLE \<Zeit\> \<Speed\>**

Nach links drehen

**RCIRCLE \<Zeit\> \<Speed\>**

Nach rechts drehen

**LEFT \<Zeit\> \<Speed\>**

Nach links schieben

**RIGHT \<Zeit\> \<Speed\>**

Nach rechts schieben

**ID**

Liefert den Namen des Roboters
Derzeit auf *Hallo* hardcodiert

**CLEAR**

Tut nichts, liefert nur OK zurück. Kann neben ID als Test der Verbindung genutzt werden.


