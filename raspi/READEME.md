# Raspi config

## Wifi
### Eduroam

Für den Betrieb im Eduroam wird das aktuelle Root-Zertifikat benötigt. Dieses findet man unter https://rz.htw-berlin.de/anleitungen/wlan-wi-fi/
Des Weiteren wird eine passende Konfiguration für WPA Supplicant erstellt werden.

Um das interne WLAN Modul für Eduroam zu starten, den folgenden Befehl ausführen:

<code>
sudo wpa_supplicant -D wext -i wlan0 -c /etc/wpa_supplicant/eduroam.conf
</code>

### HtwMobileLab

Für das Sensornetzwerk im Mobile Lab wird ein weiteres WLAN eingerichtet. Dafür wird ein externer WLAN Stick Raspberry Pi eingerichtet.

ASUSTek Computer, Inc. WL-167G v3 802.11n Adapter [Realtek RTL8188SU]

#### Installation

https://www.elektronik-kompendium.de/sites/raspberry-pi/2002171.htm

Das Netzwerk wird für Interface <code>wlan1</code> eingerichtet:

DHCP Server und Wireless Access Point:
<code>sudo apt-get install hostapd</code>


<code>sudo apt-get install dnsmasq</code>


<code>sudo systemctl stop hostapd</code>


<code>sudo systemctl stop dnsmasq</code>

Beides konfigurieren, wie in den Dateien

<code>sudo systemctl restart dnsmasq</code>

<code>sudo systemctl enable dnsmasq</code>


<code></code>

<code></code>

<code></code>

<code></code>

<code></code>



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

