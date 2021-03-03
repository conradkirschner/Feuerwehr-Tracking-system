#!/bin/bash

# interfaces
# wlan0: Uplink -> oeffentlich erreichbar
# wlan1: intern -> Routing nach außen ermoeglichen, sonst blocken und NAT

# alles loeschen
iptables -F
iptables -X

# alles blocken
iptables -P INPUT DROP
iptables -P OUTPUT DROP
iptables -P FORWARD DROP

# local ein- u ausgehend erlauben
iptables -A INPUT -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

## wlan0
# bestehend und ausgehend erlauben
iptables -A INPUT -i wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
iptables -A OUTPUT -o wlan0 -m state --state NEW,RELATED,ESTABLISHED -j ACCEPT

# Ping erlauben
#sudo iptables -A INPUT -i wlan0 -p icmp -j ACCEPT
iptables -A INPUT -i wlan0 -p icmp -m limit --limit 1/sec --limit-burst 1 -j ACCEPT
iptables -A OUTPUT -o wlan0 -p icmp -j ACCEPT

# SSH erlauben
#sudo iptables -A INPUT -i wlan0 -p tcp --dport 22 -j ACCEPT
iptables -A INPUT -i wlan0 -p tcp --dport 22 -m conntrack --ctstate NEW,ESTABLISHED -j ACCEPT
iptables -A OUTPUT -o wlan0 -p tcp --sport 22 -m conntrack --ctstate ESTABLISHED -j ACCEPT

# Grafana von aussen zulassen
iptables -A INPUT -i wlan0 -p tcp --dport 3000 -m conntrack --ctstate NEW,ESTABLISHED -j ACCEPT
iptables -A OUTPUT -o wlan0 -p tcp --dport 3000 -m conntrack --ctstate ESTABLISHED -j ACCEPT

# alles ausgehende erlauben 
iptables -A OUTPUT -o wlan0 -m state --state NEW -j ACCEPT

# lan to wan
iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE 
iptables -A FORWARD -i wlan1 -o wlan0 -j ACCEPT
iptables -A FORWARD -i wlan0 -o wlan1 -m state --state RELATED,ESTABLISHED -j ACCEPT
iptables -A FORWARD -i wlan1 -o wlan0 -j ACCEPT

# alles kaputte loeschen
iptables -A INPUT -m conntrack --ctstate INVALID -j DROP

## wlan1 alles erlauben
iptables -A INPUT -i wlan1 -p all -j ACCEPT
iptables -A OUTPUT -o wlan1 -p all -j ACCEPT

## nur aus HTW Netzwerk?
#iptables -A INPUT -i eth0 -p tcp -s 192.168.100.0/24 --dport 22 -m state --state NEW,ESTABLISHED -j ACCEPT
#iptables -A OUTPUT -o eth0 -p tcp --sport 22 -m state --state ESTABLISHED -j ACCEPT


# save & load
#sudo iptables-save > /etc/iptables/iptables.rules
#sudo iptables-restore < /etc/iptables/iptables.rules

#apt-get install iptables-persistent
#/etc/iptables/rules.v4
#iptables-save > /etc/iptables/rules.v4

#https://crm.vpscheap.net/index.php?rp=/knowledgebase/29/25-Most-Frequently-Used-Linux-IPTables-Rules-Examples.html
#https://opensource.com/article/18/10/iptables-tips-and-tricks
