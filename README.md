# Slimme-meter-ESP32-V2
Interface voor slimme elektriciteitsmeter België / Nederland versie 2
Slimme-meter-ESP32-V2

Interface voor slimme elektriciteitsmeter België / Nederland versie 2

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

Deze modules communiceren draadloos met elkaar via esp-now

Eigen WiFi netwerk (WiFi.mode(WIFI_AP_STA)

Netwerk : ESP32Energie_V2 Paswoord : ESP32pswd IP address : 192.168.4.1
Wat kunnen we ermee

weergave van via webpagina

totaal elektriciteitsverbruik

totaal injektie

totaal gasverbruik

huidig elektriciteitsverbruik

huidig injektie

status digitale uitgangen

status PWM signalen

# Wat kunnen we ermee 

opslag en weergave van 

elektriciteist verbruik

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
