# Raspi config

## Wifi
### Eduroam

Um das interne WLAN Modul für Eduroam zu starten, den folgenden Befehl ausführen:

<code>
sudo wpa_supplicant -D wext -i wlan0 -c /etc/wpa_supplicant/eduroam.conf
</code>

