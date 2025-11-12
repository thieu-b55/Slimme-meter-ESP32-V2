# Soldeerdiagram: Relais schakel bij Injectie

## Overzicht
Dit systeem bestaat uit 2 ESP32 modules die via ESP-NOW communiceren:
1. **Interface Module** - Leest de P1 poort van de slimme meter
2. **Relais Module** - Schakelt een relais aan wanneer er injectie is

---

## Module 1: Interface Module (Slimme meter uitlezer)

### Benodigdheden:
- 1x ESP32 WROOM DevKit
- 1x SD kaart module of houder
- 1x BC547 transistor
- 1x RJ12 kabel met connector
- 3x LED (optioneel: Power, SD Error, Blinkie)
- 1x 1kΩ weerstand
- 1x 2.2kΩ weerstand
- 1x 10kΩ weerstand
- 3x 20kΩ weerstand
- 3x 4700µF/16V condensator (indien gevoed via P1 poort)
- Externe 5V voeding (aanbevolen) OF voeding via P1 poort

### Aansluitingen Interface Module:

```
┌────────────────────────────────────────────────────────────────┐
│  RJ12 P1 Poort Connector (kijkend naar de pinnen)             │
│  ┌──────┐                                                      │
│  │ 1  6 │  Pin 1: +5V (rood)                                  │
│  │ 2  5 │  Pin 2: Data Request (geel)                          │
│  │ 3  4 │  Pin 3: Data GND (blauw)                            │
│  └──────┘  Pin 4: Niet aangesloten                             │
│            Pin 5: Data (groen)                                  │
│            Pin 6: Power GND (zwart)                            │
└────────────────────────────────────────────────────────────────┘

P1 Poort  ──────┐
Pin 1 (+5V) ────┼──── 4700µF ─┬─── 4700µF ─┬─── 4700µF ────┬──→ ESP32 5V (VIN)
                │              │            │               │
Pin 2 (Data Req)├──── 10kΩ ───┼────────────┼───────────────┤
                │              │            │               │
Pin 3 (Data GND)├──────────────┴────────────┴───────────────┼──→ ESP32 GND
Pin 6 (Power GND)                                            │
                │                                            │
Pin 5 (Data) ───┴──── 2.2kΩ ──┬── BC547 (B)                │
                                │                            │
                                                             │
                    10kΩ ───────┼──→ ESP32 3.3V             │
                                │                            │
               BC547 (C) ───────┼──→ ESP32 GPIO16           │
                                │                            │
               BC547 (E) ───────┴──→ GND                    │
                                                             │
SD Kaart Module:                                             │
  3.3V    ←───────────────────────────────────────────────── ESP32 3.3V
  GND     ←──────────────────────────────────────────────────→ ESP32 GND
  CS      ←──────────────────────────────────────────────────→ ESP32 GPIO5
  SCK     ←──────────────────────────────────────────────────→ ESP32 GPIO18
  MOSI    ←──────────────────────────────────────────────────→ ESP32 GPIO23
  MISO    ←──────────────────────────────────────────────────→ ESP32 GPIO19

Optionele LEDs:
  LED BLINKIE (groen) ──── 20kΩ ──→ ESP32 GPIO4  ──→ GND
  LED SD ERROR (rood) ──── 20kΩ ──→ ESP32 GPIO2  ──→ GND
  LED POWER (blauw) ────── 20kΩ ──→ ESP32 3.3V   ──→ GND
```

### Pin Samenvatting Interface ESP32:
| ESP32 Pin | Functie          | Verbinding                    |
|-----------|------------------|-------------------------------|
| GPIO16    | P1 Data Input    | BC547 Collector               |
| GPIO4     | LED Blinkie      | LED (via 20kΩ)               |
| GPIO2     | LED SD Error     | LED (via 20kΩ)               |
| GPIO5     | SD CS            | SD Card CS                    |
| GPIO18    | SD SCK           | SD Card SCK                   |
| GPIO23    | SD MOSI          | SD Card MOSI                  |
| GPIO19    | SD MISO          | SD Card MISO                  |
| 5V (VIN)  | Voeding          | P1 Pin 1 (via 3x 4700µF) OF externe 5V |
| 3.3V      | Voeding SD       | SD Card 3.3V + pull-up weerstanden |
| GND       | Massa            | P1 Pin 3, Pin 6, SD GND       |

---

## Module 2: Relais Module (Schakelaar bij injectie)

### Benodigdheden:
- 1x ESP32 WROOM DevKit
- 1x 5V Relais module (hoog vermogen, 30A aanbevolen)
  - Link: https://nl.aliexpress.com/item/4000185959463.html
  - **Kies de 5V uitvoering!**
- 1x Externe 5V voeding (minimaal 1A)
- Huishoudtoestel dat geschakeld moet worden

### Aansluitingen Relais Module:

```
                    ┌─────────────────────┐
Externe 5V Voeding  │                     │
     │              │   ESP32 DevKit      │
     ├──→ 5V (VIN) ─┤                     │
     │              │                     │
     └──→ GND ──────┤ GND                 │
                    │                     │
                    │ GPIO16 ─────────────┼──→ Relais Module (IN)
                    │                     │
                    └─────────────────────┘
                            │
                            │
               ┌────────────┴───────────────┐
               │                            │
        ┌──────┴──────┐              5V Relais Module
        │             │              ┌───────────────┐
     5V ├─────────────┼──────────────┤ DC+           │
        │             │              │               │
    GND ├─────────────┼──────────────┤ DC-           │
        │             │              │               │
  GPIO16├─────────────┼──────────────┤ IN            │
        │             │              │               │
        └─────────────┘              │ COM  ◄────────┼─── 220V L (Fase)
         ESP32                       │               │
                                     │ N.C. (leeg)   │
                                     │               │
                                     │ N.O. ◄────────┼───→ Huishoudtoestel L
                                     └───────────────┘
                                             │
                             220V N (Nul) ───┴───→ Huishoudtoestel N

BELANGRIJK:
- COM = Common (gemeenschappelijk)
- N.O. = Normally Open (normaal geopend - sluit bij activatie)
- N.C. = Normally Closed (normaal gesloten - opent bij activatie)
```

### Pin Samenvatting Relais ESP32:
| ESP32 Pin | Functie      | Verbinding                    |
|-----------|--------------|-------------------------------|
| GPIO16    | Relais Sturing| Relais Module IN             |
| 5V (VIN)  | Voeding      | Externe 5V voeding            |
| GND       | Massa        | Externe 5V GND + Relais DC-   |

### Relais Module Aansluitingen:
| Relais Pin | Functie               | Verbinding                |
|------------|-----------------------|---------------------------|
| DC+        | Voeding positief      | Externe 5V                |
| DC-        | Voeding negatief/GND  | Externe 5V GND + ESP32 GND|
| IN         | Stuursignaal input    | ESP32 GPIO16              |
| COM        | Schakel gemeenschappelijk | 220V Fase (L)         |
| N.O.       | Normaal Open contact  | Huishoudtoestel Fase      |
| N.C.       | Normaal Closed        | Niet aangesloten          |

---

## Software Configuratie

### 1. Interface Module Software:
- Upload: `Slimme_meter_esp32_V2.ino`
- Noteer het MAC adres van deze ESP32 (zie Serial Monitor)
- Configureer relais schakelwaarden via webinterface (192.168.4.1):
  - **Netwerk**: ESP32Energie_V2
  - **Wachtwoord**: ESP32pswd
  - **IP**: 192.168.4.1

### 2. Relais Module Software:
- Upload: `Slimme_meter_esp32_relais_V2.ino`
- Noteer het MAC adres van deze ESP32 (zie Serial Monitor)
- Voer dit MAC adres in bij de Interface Module

### 3. Instellingen voor Injectie Detectie:
Via de webinterface op 192.168.4.1:

**Voor automatische schakeling bij injectie:**
- **KW**: `0.5` (relais schakelt aan bij 0.5kW of meer injectie)
- **Tijd**: `00:01` (vanaf 00:01 uur is schakeling mogelijk)
- **A/M**: `A` (Automatisch - regeling actief)

**Voor alleen tijdgestuurd:**
- **KW**: `0`
- **Tijd**: `12:00` (schakelt om 12:00)
- **A/M**: `A`

**Voor handmatig aan:**
- **A/M**: `1`

**Voor handmatig uit:**
- **A/M**: `0`

---

## Werking

1. **Injectie Detectie**:
   - Interface module leest P1 poort elke seconde
   - Berekent: `injectie_nu - verbruik_nu`
   - Als resultaat positief is → er is injectie op het net

2. **Relais Schakeling**:
   - Wanneer injectie ≥ ingestelde KW waarde
   - EN tijd is bereikt (indien ingesteld)
   - → Relais wordt HIGH (aan)
   - Relais blijft aan tot 00:00 uur (automatische reset)

3. **Communicatie**:
   - Interface en Relais module communiceren via ESP-NOW (draadloos)
   - Bereik: ongeveer 50-100 meter afhankelijk van obstakels

---

## Veiligheidsvoorschriften

⚠️ **WAARSCHUWING: 220V WISSELSTROOM IS LEVENSGEVAARLIJK!**

- Schakel alle stroom uit voordat u aan 220V bedrading werkt
- Laat 220V installatie uitvoeren door een erkend elektricien
- Gebruik alleen geschikte relais voor 220V belasting
- Test eerst met lage spanning (12V) voordat u 220V aansluit
- Monteer in een IP-beschermde behuizing
- Voorzie zekeringen op alle bedrading

---

## Test Procedure

### Stap 1: Test Interface Module
1. Sluit alleen Interface module aan op P1 poort
2. Reset ESP32 (knop op DevKit)
3. Wacht 30 seconden
4. BLINKIE LED moet knipperen met 0.5Hz
5. Verbind met WiFi netwerk: ESP32Energie_V2
6. Ga naar 192.168.4.1
7. Controleer of injectie waarde wordt getoond

### Stap 2: Test Relais Module
1. Sluit Relais module aan op 5V voeding
2. Upload software
3. Noteer MAC adres
4. Voer MAC adres in bij Interface module
5. Test handmatige bediening (A/M = 1)
6. Controleer of relais klikt

### Stap 3: Test Automatische Schakeling
1. Stel KW in op lage waarde (bijv. 0.5kW)
2. Wacht op zonnige dag
3. Controleer of relais schakelt bij voldoende injectie
4. Monitor via webinterface (192.168.4.1)

---

## Troubleshooting

| Probleem | Mogelijke Oorzaak | Oplossing |
|----------|-------------------|-----------|
| Interface geen data | P1 poort niet geactiveerd | Activeer via Fluvius website |
| BLINKIE knippert niet | Onvoldoende voeding | Gebruik externe 5V voeding |
| Geen ESP-NOW verbinding | Verkeerd MAC adres | Controleer MAC adressen |
| Relais schakelt niet | GPIO16 verkeerd aangesloten | Controleer bedrading |
| Continue reset | Zelfde MAC 2x ingevuld | Gebruik unieke MAC adressen |

---

## Uitbreidingsmogelijkheden

- Voeg Display module toe voor lokale weergave
- Voeg tweede relais toe voor extra apparaat
- Voeg PWM modules toe voor geleidelijke regeling
- Koppel aan Home Assistant via MQTT

---

**Auteur**: thieu-b55
**Versie**: 2.0
**Datum**: November 2024
**Licentie**: MIT

Live long and prosper! 🖖
