# Complete Pinout Diagram - Alle Verbindingen

## 🎯 Overzicht
Dit document toont ALLE pins van beide ESP32 modules met exacte verbindingen.
Ongebruikte pins zijn duidelijk gemarkeerd.

---

# 📟 MODULE 1: INTERFACE ESP32 (P1 Meter Uitlezer)

## Complete ESP32 DevKit Pinout

```
                        ┌─────────────────────────────────────┐
                        │                                     │
                        │        ESP32 DevKit (30 pins)       │
                        │         Interface Module            │
                        │                                     │
┌───────────────────────┴─────────────────────────────────────┴───────────────────────┐
│                                                                                       │
│   LINKER KANT (USB poort onder)                  RECHTER KANT                        │
│                                                                                       │
│   3.3V  ●─────────────[10kΩ]──────┐              ● GND ───────┐                     │
│                                    │                           │                     │
│   EN    ○ (niet gebruiken)         │              ● GPIO23 ────┼───→ SD MOSI        │
│                                    │                           │                     │
│   VP    ○ (niet gebruiken)         │              ● GPIO22 ○   │ (niet gebruiken)   │
│                                    │                           │                     │
│   VN    ○ (niet gebruiken)         │              ● GPIO1  ○   │ (TX - niet gebruik)│
│                                    │                           │                     │
│   GPIO34 ○ (niet gebruiken)        │              ● GPIO3  ○   │ (RX - niet gebruik)│
│                                    │                           │                     │
│   GPIO35 ○ (niet gebruiken)        │              ● GPIO21 ○   │ (niet gebruiken)   │
│                                    │                           │                     │
│   GPIO32 ○ (niet gebruiken)        │              ● GND ───────┤                     │
│                                    │                           │                     │
│   GPIO33 ○ (niet gebruiken)        │              ● GPIO19 ────┼───→ SD MISO        │
│                                    │                           │                     │
│   GPIO25 ○ (niet gebruiken)        │              ● GPIO18 ────┼───→ SD SCK         │
│                                    │                           │                     │
│   GPIO26 ○ (niet gebruiken)        │              ● GPIO5  ────┼───→ SD CS          │
│                                    │                           │                     │
│   GPIO27 ○ (niet gebruiken)        │              ● GPIO17 ○   │ (niet gebruiken)   │
│                                    │                           │                     │
│   GPIO14 ○ (niet gebruiken)        │              ● GPIO16 ────┼───→ P1 DATA IN ●   │
│                                    │                           │      (BC547 C)      │
│   GPIO12 ○ (niet gebruiken)        │              ● GPIO4  ────┼───→ LED Blinkie ●  │
│                                    │                           │      (optioneel)    │
│   GPIO13 ○ (niet gebruiken)        │              ● GPIO0  ○   │ (BOOT - niet gebr.)│
│                                    │                           │                     │
│   GND   ●──────────────────────────┼───────────────● GPIO2 ────┼───→ LED SD Error ● │
│         │                          │                           │      (optioneel)    │
│         │                          │              ● GPIO15 ○   │ (niet gebruiken)   │
│         │                          │                           │                     │
│   VIN   ●──────────────────────────┤              ● GND ───────┤                     │
│   (5V)  │                                                       │                     │
│         │                                                       │                     │
└─────────┼───────────────────────────────────────────────────────┼─────────────────────┘
          │                                                       │
          │                                    ┌──────────────────┴─────────────────┐
          │                                    │                                    │
          │                            Alle GND's samen verbonden                   │
          │                                    │                                    │
          └────────────────────────────────────┴────────────────────────────────────┘
```

### Legende:
- `●` = Gebruikte pin (VERBINDEN)
- `○` = Ongebruikte pin (NIET VERBINDEN)

---

## 🔴 Complete Bedradingsschema Interface Module

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                       │
│                         RJ12 P1 CONNECTOR (6 pins)                                   │
│                                                                                       │
│   Pin 1 (Rood, +5V)  ─────┬──────────[10kΩ]───────→ Pin 2 (Geel, Data Request)     │
│                           │                                                          │
│                           │  ┌────[4700µF]────┐ OPTIE A: Voeding via P1            │
│                           │  │                 │ (3x condensatoren parallel)        │
│                           └──┼────[4700µF]────┼──→ ESP32 VIN (5V)                   │
│                              │                 │                                     │
│                              └────[4700µF]────┘                                      │
│                              │                                                       │
│                           OF │                                                       │
│                              │                                                       │
│   EXTERNE 5V VOEDING ────────┴──────────────────→ ESP32 VIN (5V)  ⭐ AANBEVOLEN    │
│                                                                                       │
│                                                                                       │
│   Pin 2 (Geel, Data Request) ──[10kΩ]──→ Pin 1 (+5V)  [ALTIJD NODIG]               │
│                                                                                       │
│   Pin 3 (Blauw, Data GND)  ────┬──→ ESP32 GND                                       │
│   Pin 6 (Zwart, Power GND) ────┤                                                     │
│                                 │                                                     │
│   Pin 4 (niet verbinden)        │                                                     │
│                                 │                                                     │
│   Pin 5 (Groen, Data) ──────────┼───→ [2.2kΩ] ──→ BC547 Base (B)                   │
│                                 │                                                     │
└─────────────────────────────────┼─────────────────────────────────────────────────────┘
                                  │
                                  │
         ┌────────────────────────┼──────────────────────────────────────┐
         │                        │                                      │
         │                  BC547 TRANSISTOR                             │
         │                  (Vlakke kant naar je toe)                    │
         │                                                                │
         │                        ┌────┐                                 │
         │                        │ BC │ Vlakke kant                     │
         │                        │547 │                                 │
         │                        └──┬─┘                                 │
         │                      ┌────┼────┐                              │
         │                      │    │    │                              │
         │                     (C)  (B)  (E)                             │
         │                      │    │    │                              │
         │    ESP32 3.3V ───[10kΩ]─→│    │                              │
         │                      │    │    │                              │
         │    ESP32 GPIO16 ←────┤    │    │                              │
         │                      │    │    │                              │
         │    P1 Pin 5 ──[2.2kΩ]────┤    │                              │
         │                      │    │    │                              │
         │    ESP32 GND ←───────┼────┼────┘                              │
         │                      │    │                                   │
         └──────────────────────┘    └───────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                       │
│                     OPTIONEEL: SD KAART MODULE                                       │
│                                                                                       │
│   SD Pin         Kabel          ESP32 Pin                                            │
│   ─────────────────────────────────────────                                          │
│   VCC (3.3V) ───────────────→ 3.3V                                                   │
│   GND        ───────────────→ GND                                                    │
│   CS         ───────────────→ GPIO5                                                  │
│   MOSI       ───────────────→ GPIO23                                                 │
│   MISO       ───────────────→ GPIO19                                                 │
│   SCK        ───────────────→ GPIO18                                                 │
│                                                                                       │
└───────────────────────────────────────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                       │
│                     OPTIONEEL: STATUS LED's                                          │
│                                                                                       │
│   LED Functie    Kleur     Weerstand    ESP32 Pin      GND                          │
│   ─────────────────────────────────────────────────────────                          │
│                                                                                       │
│   Blinkie        Groen  ──[20kΩ]──→ GPIO4  ──┐                                      │
│   (activiteit)                    Lange poot │                                       │
│                                               ↓                                       │
│                                            LED │                                      │
│                                               ↓                                       │
│                                      Korte poot│                                      │
│                                               ↓                                       │
│                                             GND ←────────────────────────────────┐   │
│                                                                                   │   │
│   SD Error       Rood   ──[20kΩ]──→ GPIO2  ──→ LED ──→ GND                       │   │
│   (SD fout)                                                                       │   │
│                                                                                   │   │
│   Power          Blauw  ──[20kΩ]──→ 3.3V   ──→ LED ──→ GND                       │   │
│   (voeding aan)                                                                   │   │
│                                                                                   │   │
└───────────────────────────────────────────────────────────────────────────────────┴───┘
```

---

## 📋 Verbindingslijst Module 1 (Interface)

### VERPLICHTE Verbindingen:

| Nr | Van                | Via/Waarde | Naar           | Functie              |
|----|--------------------|-----------:|----------------|----------------------|
| 1  | P1 Pin 1 (+5V)     | 10kΩ       | P1 Pin 2       | Data Request pull-up |
| 2  | P1 Pin 1 (+5V)     | Direct OF 3x4700µF | ESP32 VIN | Voeding         |
| 3  | Ext. 5V (optie B)  | Direct     | ESP32 VIN      | Voeding (beter!)     |
| 4  | P1 Pin 3 (GND)     | Direct     | ESP32 GND      | Massa                |
| 5  | P1 Pin 6 (GND)     | Direct     | ESP32 GND      | Massa                |
| 6  | P1 Pin 5 (Data)    | 2.2kΩ      | BC547 Base     | Data signaal         |
| 7  | BC547 Collector    | Direct     | ESP32 GPIO16   | Data naar ESP32      |
| 8  | BC547 Emitter      | Direct     | ESP32 GND      | Transistor massa     |
| 9  | ESP32 3.3V         | 10kΩ       | BC547 Collector| Pull-up weerstand    |
| 10 | Ext. 5V GND (opt)  | Direct     | ESP32 GND      | Massa voeding        |

### OPTIONELE Verbindingen (SD Kaart):

| Nr | Van           | Via    | Naar         | Functie    |
|----|---------------|--------|--------------|------------|
| 11 | SD VCC        | Direct | ESP32 3.3V   | Voeding SD |
| 12 | SD GND        | Direct | ESP32 GND    | Massa SD   |
| 13 | SD CS         | Direct | ESP32 GPIO5  | Chip Select|
| 14 | SD MOSI       | Direct | ESP32 GPIO23 | Data Out   |
| 15 | SD MISO       | Direct | ESP32 GPIO19 | Data In    |
| 16 | SD SCK        | Direct | ESP32 GPIO18 | Clock      |

### OPTIONELE Verbindingen (LEDs):

| Nr | Van              | Via   | Naar         | Naar      |
|----|------------------|-------|--------------|-----------|
| 17 | LED Groen +      | 20kΩ  | ESP32 GPIO4  | LED - naar GND |
| 18 | LED Rood +       | 20kΩ  | ESP32 GPIO2  | LED - naar GND |
| 19 | LED Blauw +      | 20kΩ  | ESP32 3.3V   | LED - naar GND |

---

# 📟 MODULE 2: RELAIS ESP32 (Schakelaar)

## Complete ESP32 DevKit Pinout

```
                        ┌─────────────────────────────────────┐
                        │                                     │
                        │        ESP32 DevKit (30 pins)       │
                        │           Relais Module             │
                        │                                     │
┌───────────────────────┴─────────────────────────────────────┴───────────────────────┐
│                                                                                       │
│   LINKER KANT (USB poort onder)                  RECHTER KANT                        │
│                                                                                       │
│   3.3V  ○ (niet gebruiken)                       ● GND ───────┐                     │
│                                                                │                     │
│   EN    ○ (niet gebruiken)                       ● GPIO23 ○   │ (niet gebruiken)   │
│                                                                │                     │
│   VP    ○ (niet gebruiken)                       ● GPIO22 ○   │ (niet gebruiken)   │
│                                                                │                     │
│   VN    ○ (niet gebruiken)                       ● GPIO1  ○   │ (TX - niet gebruik)│
│                                                                │                     │
│   GPIO34 ○ (niet gebruiken)                      ● GPIO3  ○   │ (RX - niet gebruik)│
│                                                                │                     │
│   GPIO35 ○ (niet gebruiken)                      ● GPIO21 ○   │ (niet gebruiken)   │
│                                                                │                     │
│   GPIO32 ○ (niet gebruiken)                      ● GND ───────┤                     │
│                                                                │                     │
│   GPIO33 ○ (niet gebruiken)                      ● GPIO19 ○   │ (niet gebruiken)   │
│                                                                │                     │
│   GPIO25 ○ (niet gebruiken)                      ● GPIO18 ○   │ (niet gebruiken)   │
│                                                                │                     │
│   GPIO26 ○ (niet gebruiken)                      ● GPIO5  ○   │ (niet gebruiken)   │
│                                                                │                     │
│   GPIO27 ○ (niet gebruiken)                      ● GPIO17 ○   │ (niet gebruiken)   │
│                                                                │                     │
│   GPIO14 ○ (niet gebruiken)                      ● GPIO16 ────┼───→ RELAIS IN ●    │
│                                                                │                     │
│   GPIO12 ○ (niet gebruiken)                      ● GPIO4  ○   │ (niet gebruiken)   │
│                                                                │                     │
│   GPIO13 ○ (niet gebruiken)                      ● GPIO0  ○   │ (BOOT - niet gebr.)│
│                                                                │                     │
│   GND   ●──────────────────────────────────────────● GPIO2 ○  │ (niet gebruiken)   │
│         │                                                      │                     │
│         │                                          ● GPIO15 ○  │ (niet gebruiken)   │
│         │                                                      │                     │
│   VIN   ●──────────────────────────              ● GND ───────┤                     │
│   (5V)  │                                                      │                     │
│         │                                                      │                     │
└─────────┼──────────────────────────────────────────────────────┼─────────────────────┘
          │                                                      │
          │                               ┌──────────────────────┴──────┐
          │                               │                             │
          │                       Alle GND's samen verbonden            │
          │                               │                             │
          └───────────────────────────────┴─────────────────────────────┘
```

### Legende:
- `●` = Gebruikte pin (VERBINDEN)
- `○` = Ongebruikte pin (NIET VERBINDEN)

---

## 🔵 Complete Bedradingsschema Relais Module

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                       │
│                        EXTERNE 5V VOEDING (1A minimum)                               │
│                                                                                       │
│                                   ┌──────┐                                           │
│                                   │ 5V   │ Voeding Adapter                           │
│                                   │Adapt.│ (bijv. USB lader)                         │
│                                   └───┬──┘                                           │
│                                       │                                               │
│                          ┌────────────┴────────────┐                                 │
│                          │                         │                                 │
│                       5V (+)                     GND (-)                              │
│                          │                         │                                 │
│                          ├─────────────────────────┼──────────────────┐              │
│                          │                         │                  │              │
│                          ↓                         ↓                  │              │
│                    ┌──────────┐              ┌──────────┐            │              │
│                    │          │              │          │            │              │
│                    │  ESP32   │              │  RELAIS  │            │              │
│                    │  DevKit  │              │  MODULE  │            │              │
│                    │          │              │          │            │              │
│                    │  VIN ●←──┤              │  DC+  ●←─┤            │              │
│                    │      (5V)│              │      (5V)│            │              │
│                    │          │              │          │            │              │
│                    │  GND ●───┼──────────────┤  DC-  ●──┼────────────┘              │
│                    │          │              │          │                            │
│                    │GPIO16 ●──┼──────────────→  IN   ●  │                           │
│                    │          │   Stuursignaal          │                            │
│                    └──────────┘              └────┬─────┘                            │
│                                                   │                                  │
│                                           220V CONTACTEN                             │
│                                                   │                                  │
└───────────────────────────────────────────────────┼──────────────────────────────────┘
                                                    │
                                                    │
┌───────────────────────────────────────────────────┼──────────────────────────────────┐
│                                                   │                                  │
│                        220V SCHAKEL CONTACTEN                                        │
│                    ⚠️  LEVENSGEVAARLIJK - ELEKTRICIEN! ⚠️                           │
│                                                   │                                  │
│                                           ┌───────┴────────┐                         │
│                                           │                │                         │
│    METERKAST                              │  RELAIS MODULE │       APPARAAT          │
│    ────────────                           │  ────────────  │       ─────────         │
│                                           │                │                         │
│    Fase ──────────────────────────────────┤  COM      ●   │                         │
│    (Bruin/Zwart)                          │                │                         │
│                                           │  N.O.     ●───┼─────→ Fase (Bruin)      │
│                                           │  (Normaal Open)│                         │
│                                           │                │                         │
│                                           │  N.C.     ○   │  (Niet Verbinden)       │
│                                           │  (Niet gebr.)  │                         │
│                                           │                │                         │
│    Nul ───────────────────────────────────┼────────────────┼─────→ Nul (Blauw)      │
│    (Blauw)                                │                │                         │
│                                           │                │                         │
│    Aarde ─────────────────────────────────┼────────────────┼─────→ Aarde (Geel/Groen)│
│    (Geel/Groen)                           │                │                         │
│                                           │                │                         │
│                                           └────────────────┘                         │
│                                                                                       │
│   WERKING:                                                                           │
│   • Relais UIT (GPIO16 LOW):  N.O. contact OPEN  → Apparaat UIT                     │
│   • Relais AAN (GPIO16 HIGH): N.O. contact SLUIT → Apparaat AAN                     │
│                                                                                       │
└───────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 📋 Verbindingslijst Module 2 (Relais)

### DC Laagspanning Verbindingen (VEILIG):

| Nr | Van                | Via    | Naar              | Functie              |
|----|--------------------|--------|-------------------|----------------------|
| 1  | Ext. 5V Voeding +  | Direct | ESP32 VIN (5V)    | Voeding ESP32        |
| 2  | Ext. 5V Voeding +  | Direct | Relais DC+        | Voeding Relais       |
| 3  | Ext. 5V Voeding -  | Direct | ESP32 GND         | Massa ESP32          |
| 4  | Ext. 5V Voeding -  | Direct | Relais DC-        | Massa Relais         |
| 5  | ESP32 GPIO16       | Direct | Relais IN         | Stuursignaal         |

**Belangrijk:** Alle GND's moeten samen verbonden zijn (ESP32 GND + Relais DC- + Voeding -)

### 220V Hoogspanning Verbindingen (ELEKTRICIEN! ⚠️):

| Nr | Van Meterkast    | Naar Relais | Van Relais | Naar Apparaat |
|----|------------------|-------------|------------|---------------|
| 6  | Fase (bruin)     | COM         | -          | -             |
| 7  | -                | -           | N.O.       | Fase (bruin)  |
| 8  | Nul (blauw)      | -           | -          | Nul (blauw)   |
| 9  | Aarde (geel/groen)| -          | -          | Aarde (geel/groen) |

**Let op:**
- N.C. contact wordt NIET verbonden
- Nul en Aarde gaan DIRECT naar apparaat (niet via relais)
- Alleen FASE gaat via relais COM → N.O. contact

---

## 🔧 Foto Referentie ESP32 Pin Nummering

### ESP32 DevKit Layout (bovenaanzicht, USB naar beneden):

```
           ┌─────────────────┐
           │                 │
           │   ESP32 CHIP    │
           │                 │
           └─────────────────┘
                    │
         ┌──────────┴──────────┐
         │                     │
LINKS    │                     │    RECHTS
         │                     │
3.3V  ●──┤                     ├──● GND
EN    ○──┤                     ├──● GPIO23
VP    ○──┤                     ├──● GPIO22
VN    ○──┤                     ├──● GPIO1 (TX)
G34   ○──┤                     ├──● GPIO3 (RX)
G35   ○──┤                     ├──● GPIO21
G32   ○──┤                     ├──● GND
G33   ○──┤     ESP32 WROOM     ├──● GPIO19
G25   ○──┤      30-PINS        ├──● GPIO18
G26   ○──┤                     ├──● GPIO5
G27   ○──┤                     ├──● GPIO17
G14   ○──┤                     ├──● GPIO16  ← BELANGRIJK!
G12   ○──┤                     ├──● GPIO4
G13   ○──┤                     ├──● GPIO0
GND   ●──┤                     ├──● GPIO2
VIN   ●──┤                     ├──● GPIO15
         │                     ├──● GND
         └──────────┬──────────┘
                    │
             ┌──────┴──────┐
             │   USB PORT  │
             └─────────────┘
```

---

## 📸 Component Identificatie

### BC547 Transistor (Interface Module):
```
Vlakke kant naar je toe:

     ┌─────┐
     │ BC  │  ← Platte kant
     │ 547 │
     └──┬──┘
   ┌────┼────┐
   │    │    │
 Collector  Base  Emitter
   (C)    (B)    (E)
   │      │      │
   │      │      └─→ Altijd naar GND
   │      └────────→ Via 2.2kΩ naar P1 Data
   └───────────────→ Naar GPIO16 + 10kΩ naar 3.3V
```

### 5V Relais Module:
```
┌────────────────────────────┐
│    5V RELAIS MODULE        │
│         (30A type)         │
├────────────────────────────┤
│  LAAGSPANNING (DC):        │
│   DC+  ← 5V voeding +      │
│   DC-  ← 5V voeding - (GND)│
│   IN   ← ESP32 GPIO16      │
├────────────────────────────┤
│  HOOGSPANNING (AC) ⚠️:     │
│   COM  ← 220V Fase IN      │
│   N.O. → 220V Fase UIT     │
│   N.C. ← NIET VERBINDEN    │
└────────────────────────────┘

Soms staat er ook:
- VCC in plaats van DC+
- Signal / S in plaats van IN
- NO in plaats van N.O.
- NC in plaats van N.C.
```

### RJ12 P1 Connector:
```
Kijkend naar de pinnen (lipje naar beneden):

┌────────┐
│ 1    6 │
│ 2    5 │
│ 3    4 │
└────────┘
  ▼▼▼▼

Pin 1: Rood    (+5V)
Pin 2: Geel    (Data Request)
Pin 3: Blauw   (Data GND)
Pin 4: -       (Niet verbinden)
Pin 5: Groen   (Data)
Pin 6: Zwart   (Power GND)
```

---

## ✅ Complete Controle Checklist

### Voor Module 1 (Interface):

#### Transistor Circuit:
- [ ] BC547 vlakke kant correct georiënteerd
- [ ] 2.2kΩ tussen P1 Pin 5 en BC547 Base
- [ ] 10kΩ tussen ESP32 3.3V en BC547 Collector
- [ ] BC547 Collector naar ESP32 GPIO16
- [ ] BC547 Emitter naar ESP32 GND

#### P1 Verbindingen:
- [ ] P1 Pin 1 (+5V) via 10kΩ naar Pin 2
- [ ] P1 Pin 1 naar ESP32 VIN (via condensatoren OF externe voeding)
- [ ] P1 Pin 3 (GND) naar ESP32 GND
- [ ] P1 Pin 6 (GND) naar ESP32 GND
- [ ] Alle GND's samen verbonden

#### Voeding:
- [ ] Externe 5V voeding + naar ESP32 VIN
- [ ] Externe 5V voeding - naar ESP32 GND
- [ ] OF: 3x 4700µF condensatoren parallel P1 Pin 1 naar VIN

#### Optioneel - SD Kaart:
- [ ] SD VCC naar ESP32 3.3V
- [ ] SD GND naar ESP32 GND
- [ ] SD CS naar GPIO5
- [ ] SD MOSI naar GPIO23
- [ ] SD MISO naar GPIO19
- [ ] SD SCK naar GPIO18

#### Optioneel - LEDs:
- [ ] LED Groen via 20kΩ naar GPIO4, - naar GND
- [ ] LED Rood via 20kΩ naar GPIO2, - naar GND
- [ ] LED Blauw via 20kΩ naar 3.3V, - naar GND

### Voor Module 2 (Relais):

#### Voeding:
- [ ] 5V voeding + naar ESP32 VIN
- [ ] 5V voeding + naar Relais DC+
- [ ] 5V voeding - naar ESP32 GND
- [ ] 5V voeding - naar Relais DC-
- [ ] ESP32 GND en Relais DC- samen verbonden

#### Stuursignaal:
- [ ] ESP32 GPIO16 naar Relais IN

#### 220V (door elektricien):
- [ ] Meterkast Fase naar Relais COM
- [ ] Relais N.O. naar Apparaat Fase
- [ ] Meterkast Nul direct naar Apparaat Nul
- [ ] Meterkast Aarde direct naar Apparaat Aarde
- [ ] Relais N.C. NIET verbonden

---

## 🧪 Test Procedure

### Stap 1: Visuele Inspectie
1. Controleer alle soldeerverbindingen
2. Zoek naar kortsluitingen
3. Controleer polariteit van componenten
4. Controleer of alle GND's samen zijn

### Stap 2: Multimeter Tests (zonder voeding!)
1. Meet tussen VIN en GND: moet > 1kΩ zijn
2. Meet tussen 3.3V en GND: moet > 1kΩ zijn
3. Meet weerstanden: 2.2kΩ ±10%, 10kΩ ±10%, 20kΩ ±10%

### Stap 3: Interface Module Test
1. Sluit externe 5V voeding aan (NIET P1 eerst!)
2. Controleer of ESP32 opstart
3. Kijk naar Serial Monitor (115200 baud)
4. Noteer MAC adres
5. Als OK: sluit P1 poort aan

### Stap 4: Relais Module Test
1. Sluit 5V voeding aan
2. Upload software
3. Noteer MAC adres
4. Test handmatig (A/M = 1 via webinterface)
5. Relais moet klikken

### Stap 5: Volledige Systeem Test
1. Beide modules aan
2. MAC adressen uitgewisseld via webinterface
3. Stel lage KW waarde in (bijv. 0.1)
4. Wacht op zonnige middag
5. Controleer of relais schakelt bij injectie

---

## 🆘 Troubleshooting per Pin

### Interface Module:

| Pin | Probleem | Check |
|-----|----------|-------|
| GPIO16 | Geen data | BC547 correct? 10kΩ pull-up aanwezig? |
| VIN | ESP32 reset | Voldoende stroom? Condensatoren? |
| GND | Rare waardes | Alle GND's samen? |
| GPIO5 | SD werkt niet | CS verbinding OK? |
| GPIO4 | LED knippert niet | 20kΩ aanwezig? LED polariteit? |

### Relais Module:

| Pin | Probleem | Check |
|-----|----------|-------|
| GPIO16 | Relais schakelt niet | Verbinding naar IN? Software geüpload? |
| VIN | ESP32 werkt niet | 5V voeding werkend? |
| GND | Instabiel | GND ESP32 en Relais DC- samen? |

---

## 📦 Bestanden in Repository

| Bestand | Geschikt voor |
|---------|---------------|
| `COMPLETE_PINOUT_DIAGRAM.md` | Dit document - Complete pinouts |
| `SOLDEERDIAGRAM_PINVERBINDINGEN.md` | Stap-voor-stap solderen |
| `EENVOUDIG_RELAIS_SCHEMA.md` | Snel overzicht concept |
| `SOLDEERDIAGRAM_RELAIS_INJECTIE.md` | Technische details |

---

**Succes met je project!** ⚡🔌

*Live long and prosper!* 🖖

**thieu-b55 | Versie 2.0 | November 2024**
