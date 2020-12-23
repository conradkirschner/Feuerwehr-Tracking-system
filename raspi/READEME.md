# Raspi config

## Wifi
### Eduroam

Für den Betrieb im Eduroam wird das aktuelle Root-Zertifikat benötigt. Dieses findet man unter https://rz.htw-berlin.de/anleitungen/wlan-wi-fi/
Des Weiteren wird eine passende Konfiguration für WPA Supplicant erstellt werden.

Um das interne WLAN Modul für Eduroam zu starten, den folgenden Befehl ausführen:

<code>
sudo wpa_supplicant -D wext -i wlan0 -c /etc/wpa_supplicant/eduroam.conf
</code>

## DynDNS

Als DynDNS Dienst wird https://dynv6.com eingesetzt. Der Raspi aktualisiert seine DynDNS-Adresse automatisch mit Hilfe von https://ddclient.net/ , wenn sich auf der Schnittstelle <code>wlan0</code> die IP-Adresse ändert.

Installation <code>ddclient</code>:

<code>sudo apt install ddclient</code>

<code></code>

## Grafana

### Installation

https://grafana.com/tutorials/install-grafana-on-raspberry-pi/

<code>wget -q -O - https://packages.grafana.com/gpg.key | sudo apt-key add -</code>

<code>echo "deb https://packages.grafana.com/oss/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list</code>

<code>sudo apt-get update</code>

<code>sudo apt-get install -y grafana</code>

