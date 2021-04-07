# Feuerwehr-Tracking-system
- [Einleitung](#einleitung)
- [Hauptteil](#hauptteil)
	- [Raspberry Pi einrichten](#raspberry)
	- [ESP32](#ESP32)
	- [Eduroam](#eudoram)	
	- [FAQ](#FAQ)	

- [Fazit](#Fazit)
<a name="einleitung"></a>
## Einleitung

![alt text](https://github.com/conradkirschner/Feuerwehr-Tracking-system/blob/main/docs/feuerwehrauto.png "Logo Title Text 1")

<a name="hauptteil"></a>
## Hauptteil
<a name="raspberry"></a>
### Raspberry Pi einrichten
<a name="ESP32"></a>
#### Wifi
#### Eduroam

Für den Betrieb im Eduroam wird das aktuelle Root-Zertifikat benötigt. Dieses findet man unter https://rz.htw-berlin.de/anleitungen/wlan-wi-fi/
Des Weiteren wird eine passende Konfiguration für WPA Supplicant erstellt werden.

Um das interne WLAN Modul für Eduroam zu starten, den folgenden Befehl ausführen:

<code>
sudo wpa_supplicant -D wext -i wlan0 -c /etc/wpa_supplicant/eduroam.conf
</code>

#### HtwMobileLab

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

<code>sudo chmod 600 /etc/hostapd/hostapd.conf</code>

Testen/Debug:

<code>sudo hostapd -dd /etc/hostapd/hostapd.conf</code>

Als Daemon registrieren:

<code>sudo nano /etc/default/hostapd</code>

<code>sudo systemctl unmask hostapd</code>

<code>sudo systemctl start hostapd</code>

<code>sudo systemctl enable hostapd</code>

Routing und NAT für die Internet-Verbindung konfigurieren:

<code>sudo nano /etc/sysctl.conf</code>

<code>net.ipv4.ip_forward=1</code>

<code>sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE</code>

<code>sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"</code>

KLAPPT NICHT:

<code>sudo nano /etc/rc.local</code>

<code>iptables-restore < /etc/iptables.ipv4.nat</code>

<code>reboot</code>

Log & Debug:

<code>sudo systemctl status hostapd</code>

<code>ps ax | grep hostapd</code>

<code>sudo systemctl status dnsmasq</code>

<code>ps ax | grep dnsmasq</code>

<code></code>



#### DynDNS

Als DynDNS Dienst wird https://dynv6.com eingesetzt. Der Raspi aktualisiert seine DynDNS-Adresse automatisch mit Hilfe von https://ddclient.net/ , wenn sich auf der Schnittstelle <code>wlan0</code> die IP-Adresse ändert.

##### Installation

<code>sudo apt install ddclient</code>

##### Konfiguration

<code>/etc/ddclient.conf</code>

<code>systemctl restart ddclient</code>

##### Log

<code>tail -f /var/log/daemon.log</code>

#### MQTT

##### Installation

https://diyi0t.com/microcontroller-to-raspberry-pi-wifi-mqtt-communication/

<code>sudo apt-get install mosquitto</code>

<code>sudo apt-get install mosquitto-clients -y</code>

##### Konfiguration

<code>sudo nano /etc/mosquitto/mosquitto.conf </code>

User anlegen:

<code>sudo mosquitto_passwd -c /etc/mosquitto/pwfile htw-mobile-lab</code>

User löschen:
<code>sudo mosquitto_passwd -d /etc/mosquitto/pwfile</code>

Mosquitto Dienst neustarten:

<code>sudo systemctl restart mosquitto</code>

Mosquitto Dienst beim Booten starten:

<code>sudo systemctl enable mosquitto</code>

Subscriber installieren:

<code>sudo pip install paho-mqtt</code>

<code>sudo nano /etc/mosquitto/get_MQTT_data.py</code>

##### Log

<code>/var/log/mosquitto/mosquitto.log </code>

#### InfluxDB

##### Installation

<code>sudo apt install influxdb influxdb-client</code>

##### Konfiguration

<code>sudo nano /etc/influxdb/influxdb.conf</code>

##### Datenbank

<code>influx</code>

<code>CREATE DATABASE htw_mobile_lab</code>

<code>CREATE USER htw_mobile_lab WITH PASSWORD 'password' </code>

<code>GRANT ALL ON htw_mobile_lab TO htw_mobile_lab </code>

#### MQTT-Influx

<code>sudo pip3 install paho-mqtt influxdb</code>

Startskript:

<code>sudo nano /etc/mosquitto/launcher.sh </code>

<code>chmod 755 launcher.sh</code>

##### Grafana

##### Installation

https://grafana.com/tutorials/install-grafana-on-raspberry-pi/

<code>wget -q -O - https://packages.grafana.com/gpg.key | sudo apt-key add -</code>

<code>echo "deb https://packages.grafana.com/oss/deb stable main" | sudo tee -a /etc/apt/sources.list.d/grafana.list</code>

<code>sudo apt-get update</code>

<code>sudo apt-get install -y grafana</code>

### Batteriemessung
<object data="https://github.com/conradkirschner/Feuerwehr-Tracking-system/blob/main/docs/Dokumentation%20f%C3%BCr%20Batteriemessung.pdf" type="application/pdf" width="700px" height="700px">
    <embed src="https://github.com/conradkirschner/Feuerwehr-Tracking-system/blob/main/docs/Dokumentation%20f%C3%BCr%20Batteriemessung.pdf">
        <p>Dieser Browser unterstützt keine PDFs. Bitte laden Sie das PDF herunter, um es zu betrachten: <a href="https://github.com/conradkirschner/Feuerwehr-Tracking-system/blob/main/docs/Dokumentation%20f%C3%BCr%20Batteriemessung.pdf">PDF ansehen</a>.</p>
    </embed>
</object>

### ESP32
<a name="eudoram"></a>
### Eduroam
<a name="FAQ"></a>
## FAQ
### 1. WiFi is not working / System Stopped
   - do restart, remove power for 5 sec!
### 2. ESP Debugging Tipp
   - Ein Sensor ist am Pi per USB angeschlossen um den seriellen Output im Fehlerfall mitlesen zu können. Das Skript kann erst ausgeführt werden, wenn der Sensor per USB am Pi (also wenn das Gerät /dev/ttyUSB0) verfügbar ist. *screen* schreibt dann die Ausgabe im Hintergrund in das Log.
<a name="Fazit"></a>  
  ```
    Interface:	/dev/ttyUSB0  
    Skript:	/home/pi/usb_serial_output.sh  
    Log:	tail -f /home/pi/usb_serial_output.log
```
## Fazit
