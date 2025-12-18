# Update 18/12/2025
Data wordt nu opgeslagen in csv formaat.

Wordt op het einde van de dag ook uitgeprint via de seriële poort

Opbouw csv file zie vanaf pagina 35      Slimme meter ESP32 csv.pdf

Data van de vorige versie kan niet meer gelezen worden met deze versie

csv data op SD kaart kan gelezen worden door LibreOffice Calc en waarschijnlijk ook met Windows Excel maar dat kan ik niet proberen wegens geen Windows

# Slimme-meter-ESP32-V2
Interface voor slimme elektriciteitsmeter België / Nederland versie 2

Gebruikte Slimme digitale meter van Fluvius >> Sagemcom

Dit is een update van de Slimme digitale meter

https://github.com/thieu-b55/Slimme-Digitale-Energie-Meter

Voor Arduino core for ESP32 versie 3.x

gebruik de volgende libraries

https://github.com/me-no-dev/AsyncTCP

https://github.com/me-no-dev/ESPAsyncWebServer

Het project bestaat uit 4 modules

Interface module

Display module

Digitale output

PWM module

Modules communiceren draadloos via esp-now

Nu ook met maandpiek 

Eigen WiFi netwerk WiFi.mode(WIFI_AP_STA)

Netwerk : ESP32Energie_V2 

Paswoord : ESP32pswd 

IP address : 192.168.4.1

.bin files van elk programma aanwezig in bin folder kunnen ge-upload worden naar ESP32

Om problemen met niet compatibele updates van core of libaries te vermijden

# Interface module

# weergave via webpagina van :

totaal elektriciteitsverbruik

totaal injektie

totaal gasverbruik

huidig elektriciteitsverbruik

huidig injektie

maandpiek

status digitale uitgangen

status PWM signalen

# instellen van de verschillende parameters

# opslag en weergave van :

elektriciteits verbruik

injektie

gasverbruik

op

uurbasis

dagbasis

maandbasis ook maandpiek

jaarbasis

laatste 25 jaar

# Displaymodule

weergave van

totaal verbruik elektriciteit

totaal injektie

totaal gasverbruik

verbruik (rood) / injektie (groen)

status digitale uitgangen

status PWM signalen

# PWM signalen

2 PWM outputs

PWM 1

kan gebruikt worden voor het aansturen van een elektrische boiler

boiler wordt alleen aangestuurd met het eigen opgewekt vermogen

PWM 2

Deze begint te regelen vanaf het moment dat PWM 1 100% is uitgestuurd

Het eventuele nog resterende zelf opgewekt vermogen kan gebruikt worden om bijvoorbeeld

een elektriche bijverwarming te sturen.

Terugleveren op het net is momenteel niet rendabel

# Digitale uitgangen

kunnen gestuurd worden op

tijdsbasis

bij een bepaalde hoeveelheid opgewekt vermogen

of een combinatie van deze 2
