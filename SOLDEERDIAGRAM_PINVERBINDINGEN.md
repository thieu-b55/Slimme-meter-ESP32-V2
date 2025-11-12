# Soldeerdiagram: Exacte Pin Verbindingen

## 🎯 Overzicht
Dit document toont EXACT welke pin naar welke pin moet worden verbonden voor beide modules.

---

## 📦 MODULE 1: INTERFACE ESP32 (bij de slimme meter)

### Benodigde Componenten:
```
✓ 1x ESP32 DevKit (30 pins model)
✓ 1x BC547 NPN Transistor
✓ 1x RJ12 connector met 6 pinnen
✓ 3x Weerstand: 2.2kΩ, 10kΩ, 10kΩ
✓ 1x Externe 5V voeding (AANBEVOLEN)
  OF
✓ 3x Condensator 4700µF/16V (voor voeding via P1)
```

---

## 🔴 STAP 1: RJ12 P1 Connector Voorbereiden

### RJ12 Pin Layout (kijkend naar de pinnen):
```
   ┌────────┐
   │ 1    6 │
   │ 2    5 │
   │ 3    4 │
   └────────┘
```

### RJ12 Pin Functies:
| Pin | Kleur  | Functie      | Gebruik           |
|-----|--------|--------------|-------------------|
| 1   | Rood   | +5V          | Voeding positief  |
| 2   | Geel   | Data Request | Trek naar +5V     |
| 3   | Blauw  | Data GND     | Massa             |
| 4   | Groen  | -            | Niet gebruiken    |
| 5   | Groen  | Data         | P1 datasignaal    |
| 6   | Zwart  | Power GND    | Massa             |

**TIP:** Knip pin 4 af, deze wordt niet gebruikt.

---

## 🔴 STAP 2: BC547 Transistor Voorbereiden

### BC547 Pin Layout (vlakke kant naar je toe):
```
     ┌─────┐
     │ BC  │  Vlakke kant
     │ 547 │
     └──┬──┘
   ┌────┼────┐
   │    │    │
  (C)  (B)  (E)
   │    │    │
```

### BC547 Pin Functies:
| Pin | Naam      | Functie              |
|-----|-----------|----------------------|
| C   | Collector | Naar ESP32 GPIO16    |
| B   | Base      | Naar P1 Data (Pin 5) |
| E   | Emitter   | Naar GND             |

---

## 🔴 STAP 3: Soldeer Weerstanden aan BC547

### Weerstand 1: 2.2kΩ (Base weerstand)
```
P1 Pin 5 ────[2.2kΩ]──── BC547 Base (B)
(Data)
```

**Soldeer:**
1. Soldeer 2.2kΩ weerstand aan BC547 pin B (middelste pin)
2. Andere kant blijft los (gaat later naar P1 Pin 5)

### Weerstand 2: 10kΩ (Pull-up weerstand)
```
ESP32 3.3V ────[10kΩ]──── BC547 Collector (C)
```

**Soldeer:**
1. Soldeer 10kΩ weerstand aan BC547 pin C (linker pin, vlakke kant naar je toe)
2. Andere kant blijft los (gaat later naar ESP32 3.3V)

### Weerstand 3: 10kΩ (Data Request pull-up)
```
P1 Pin 1 ────[10kΩ]──── P1 Pin 2
(+5V)                   (Data Request)
```

**Soldeer:**
1. Deze weerstand verbindt P1 Pin 1 met P1 Pin 2
2. Soldeer aan beide RJ12 draden

---

## 🔴 STAP 4: Verbind BC547 met ESP32

### Exacte Verbindingen:

| Van Component              | Naar ESP32 Pin | Opmerking                    |
|----------------------------|----------------|------------------------------|
| BC547 Collector (C)        | **GPIO16**     | Direct verbinden             |
| BC547 Emitter (E)          | **GND**        | Direct verbinden             |
| 10kΩ weerstand (van C)     | **3.3V**       | Pull-up naar 3.3V           |

**Let op:** GPIO16 zit aan de lange kant van de ESP32, ongeveer halverwege.

---

## 🔴 STAP 5: Verbind RJ12 P1 met BC547 en ESP32

### Massa Verbindingen:
| Van P1      | Naar        | Opmerking                |
|-------------|-------------|--------------------------|
| Pin 3 (GND) | ESP32 GND   | Alle GND samen!         |
| Pin 6 (GND) | ESP32 GND   | Alle GND samen!         |

### Data Verbinding:
| Van P1      | Naar              | Via          |
|-------------|-------------------|--------------|
| Pin 5 (Data)| BC547 Base (B)    | 2.2kΩ        |

### Pull-up Verbinding:
| Van P1           | Naar              | Via    |
|------------------|-------------------|--------|
| Pin 1 (+5V)      | Pin 2 (Data Req)  | 10kΩ   |

---

## 🔴 STAP 6: Voeding Aansluiten

### OPTIE A: Externe 5V Voeding (AANBEVOLEN)

| Van Voeding  | Naar ESP32 | Opmerking                |
|--------------|------------|--------------------------|
| 5V +         | **5V**     | Ook wel VIN genoemd      |
| GND -        | **GND**    | Samen met andere GND's   |

**BELANGRIJK:** Pin 2 Data Request moet nog steeds via 10kΩ naar P1 Pin 1 (+5V)!

### OPTIE B: Voeding via P1 Poort (met condensatoren)

```
P1 Pin 1 (+5V) ──┬── [4700µF] ──┬── [4700µF] ──┬── [4700µF] ──→ ESP32 5V
                 │               │               │
                GND             GND             GND
```

**Condensator Polariteit:**
- Lange poot (+) naar +5V
- Korte poot (-) naar GND

| Condensator | + Kant      | - Kant      |
|-------------|-------------|-------------|
| C1          | P1 Pin 1    | GND         |
| C2          | P1 Pin 1    | GND         |
| C3          | P1 Pin 1    | GND         |

---

## 🔴 STAP 7: Optionele SD Kaart Module

Indien je data logging wilt:

| SD Module Pin | ESP32 Pin  | Opmerking          |
|---------------|------------|--------------------|
| 3.3V          | **3.3V**   | Voeding            |
| GND           | **GND**    | Massa              |
| CS            | **GPIO5**  | Chip Select        |
| SCK           | **GPIO18** | Clock              |
| MOSI          | **GPIO23** | Master Out Slave In|
| MISO          | **GPIO19** | Master In Slave Out|

---

## 🔴 STAP 8: Optionele Status LEDs

| LED Kleur | ESP32 Pin  | Weerstand | Functie        |
|-----------|------------|-----------|----------------|
| Groen     | **GPIO4**  | 20kΩ      | Blinkie (actief)|
| Rood      | **GPIO2**  | 20kΩ      | SD Error       |
| Blauw     | **3.3V**   | 20kΩ      | Power indicator|

**Aansluiting:**
```
ESP32 Pin ──── [20kΩ] ──── LED (lange poot +) ──── LED (korte poot -) ──── GND
```

---

## ✅ Checklist Module 1 (Interface):

- [ ] BC547 weerstanden gesoldeerd (2.2kΩ aan Base, 10kΩ aan Collector)
- [ ] BC547 Collector aan ESP32 GPIO16
- [ ] BC547 Emitter aan ESP32 GND
- [ ] 10kΩ pull-up van Collector naar ESP32 3.3V
- [ ] RJ12 Pin 5 (Data) via 2.2kΩ naar BC547 Base
- [ ] RJ12 Pin 1 via 10kΩ naar Pin 2 (Data Request pull-up)
- [ ] RJ12 Pin 3 + Pin 6 (GND) naar ESP32 GND
- [ ] Voeding: ofwel externe 5V, ofwel P1 Pin 1 via 3x 4700µF naar ESP32 5V
- [ ] Optioneel: SD module aangesloten
- [ ] Optioneel: Status LEDs aangesloten

---

# 📦 MODULE 2: RELAIS ESP32 (bij het apparaat)

### Benodigde Componenten:
```
✓ 1x ESP32 DevKit (30 pins model)
✓ 1x 5V Relais Module (30A type)
✓ 1x Externe 5V voeding (minimaal 1A)
✓ Kabels voor verbindingen
```

---

## 🔵 STAP 1: Relais Module Pin Layout

### 5V Relais Module Pinnen:
```
┌───────────────────┐
│  5V RELAIS MODULE │
├───────────────────┤
│ DC+   (voeding +) │ ← 5V Voeding +
│ DC-   (voeding -) │ ← 5V Voeding - (GND)
│ IN    (signaal)   │ ← ESP32 GPIO16
├───────────────────┤
│ 220V CONTACTEN:   │
│ COM   (common)    │ ← 220V Fase IN (bruin)
│ N.O.  (norm open) │ ← 220V Fase NAAR apparaat
│ N.C.  (norm close)│ ← NIET GEBRUIKEN
└───────────────────┘
```

---

## 🔵 STAP 2: Voeding Verbindingen

### Externe 5V Voeding naar ESP32:

| Van 5V Voeding | Naar ESP32 | Draad kleur (tip) |
|----------------|------------|-------------------|
| 5V +           | **5V (VIN)**| Rood             |
| GND -          | **GND**    | Zwart             |

---

## 🔵 STAP 3: Voeding naar Relais Module

### Van 5V Voeding naar Relais:

| Van 5V Voeding | Naar Relais Module | Opmerking          |
|----------------|--------------------|--------------------|
| 5V +           | **DC+**            | Parallel met ESP32 |
| GND -          | **DC-**            | Parallel met ESP32 |

**Verbinding Schema:**
```
        5V Voeding
            │
      ┌─────┴─────┐
      │           │
   5V │        5V │
ESP32 │     Relais│
  GND │        DC-│
      │           │
      └─────┬─────┘
            │
          GND (gemeenschappelijk)
```

---

## 🔵 STAP 4: Stuursignaal Verbinding

### ESP32 naar Relais:

| Van ESP32   | Naar Relais Module | Type Verbinding    |
|-------------|--------------------|--------------------|
| **GPIO16**  | **IN**             | Direct (geen weerstand) |

**Let op:** GPIO16 staat aan de lange kant van de ESP32, ongeveer halverwege.

---

## 🔵 STAP 5: 220V Verbindingen (ELEKTRICIEN!)

### ⚠️ WAARSCHUWING: LEVENSGEVAARLIJK!
**Laat dit doen door een erkend elektricien!**

### 220V Aansluitingen:

| Van                    | Naar Relais | Naar Apparaat        |
|------------------------|-------------|----------------------|
| 220V Fase (bruin/zwart)| **COM**     | -                    |
| -                      | **N.O.**    | Apparaat Fase        |
| 220V Nul (blauw)       | -           | Direct naar apparaat |
| Aarde (geel/groen)     | -           | Direct naar apparaat |

### Visueel Schema:
```
METERKAST              RELAIS MODULE           APPARAAT
   │                      │                      │
   ├─ Fase (bruin) ──────►COM                   │
   │                      │                      │
   │                      N.O. ─────────────────►│ Fase
   │                      │                      │
   ├─ Nul (blauw) ────────┼──────────────────────►│ Nul
   │                      │                      │
   └─ Aarde (geel/groen)──┼──────────────────────►│ Aarde
```

**Werking:**
- Relais UIT: N.O. contact open → Apparaat UIT
- Relais AAN: N.O. contact sluit → Apparaat AAN

---

## ✅ Checklist Module 2 (Relais):

- [ ] Externe 5V voeding aangesloten op ESP32 (5V en GND)
- [ ] Externe 5V voeding aangesloten op Relais (DC+ en DC-)
- [ ] ESP32 GND en Relais DC- samen verbonden
- [ ] ESP32 GPIO16 naar Relais IN
- [ ] 220V Fase naar Relais COM (door elektricien)
- [ ] Relais N.O. naar Apparaat Fase (door elektricien)
- [ ] 220V Nul direct naar Apparaat (door elektricien)
- [ ] Aarde direct naar Apparaat (door elektricien)
- [ ] Relais getest met handmatige bediening (A/M = 1)

---

## 📊 Complete Pin Overzicht per ESP32

### Interface ESP32 (Module 1):
| ESP32 Pin | Verbinding                           |
|-----------|--------------------------------------|
| GPIO16    | BC547 Collector (P1 Data)            |
| GPIO5     | SD CS (optioneel)                    |
| GPIO18    | SD SCK (optioneel)                   |
| GPIO23    | SD MOSI (optioneel)                  |
| GPIO19    | SD MISO (optioneel)                  |
| GPIO4     | LED Blinkie (optioneel)              |
| GPIO2     | LED SD Error (optioneel)             |
| 3.3V      | 10kΩ pull-up + SD + LED Power        |
| 5V (VIN)  | Externe 5V OF P1 Pin 1 (via 3x 4700µF)|
| GND       | P1 Pin 3+6, BC547 Emitter, alle GND  |

### Relais ESP32 (Module 2):
| ESP32 Pin | Verbinding                  |
|-----------|-----------------------------|
| GPIO16    | Relais IN                   |
| 5V (VIN)  | Externe 5V voeding +        |
| GND       | Externe 5V voeding - + Relais DC- |

---

## 🧪 Test Procedure

### Test 1: Visuele Inspectie
- [ ] Geen kortsluitingen tussen 5V en GND
- [ ] Alle soldeerverbindingen glanzend en stevig
- [ ] Polariteit condensatoren correct (indien gebruikt)
- [ ] BC547 correct georiënteerd (vlakke kant)
- [ ] Geen losse draden

### Test 2: Multimeter Check (ESP32 NIET aangesloten!)
- [ ] Meet weerstand tussen 5V en GND: moet > 1kΩ zijn
- [ ] Meet 2.2kΩ weerstand: moet 2.2kΩ ± 10% zijn
- [ ] Meet 10kΩ weerstanden: moeten 10kΩ ± 10% zijn

### Test 3: Interface Module (zonder P1 eerst)
1. Sluit alleen externe 5V voeding aan (NIET P1 poort)
2. Controleer of ESP32 opstart (meestal blauwe LED)
3. Open Serial Monitor (115200 baud)
4. Controleer of je output ziet
5. Noteer MAC adres
6. Als OK: sluit P1 poort aan

### Test 4: Relais Module
1. Sluit externe 5V voeding aan
2. Upload software
3. Controleer Serial Monitor
4. Noteer MAC adres
5. Test handmatige bediening (zie software sectie)

---

## 🔧 Veelvoorkomende Soldeer Fouten

| Fout | Symptoom | Oplossing |
|------|----------|-----------|
| Koude soldeerverbinding | Relais werkt niet betrouwbaar | Opnieuw solderen met meer heat |
| BC547 verkeerd om | Geen P1 data | Controleer vlakke kant orientatie |
| Verkeerde weerstand waarde | Geen/vreemde data | Meet met multimeter |
| GND niet samen | Rare waardes | Alle GND's moeten verbonden zijn |
| Kortsluiting 5V-GND | ESP32 wordt heet | Controleer met multimeter |

---

## 📝 Soldeer Tips

### Algemene Tips:
1. **Temperatuur:** Stel soldeerbout in op 350°C
2. **Tin type:** Gebruik 60/40 of 63/37 loodhoudend (makkelijker voor beginners)
3. **Verwarm beide delen:** Pin EN draad/component
4. **Voeg tin toe aan verbinding:** Niet aan soldeerbout punt
5. **Glanzende verbinding:** Goede soldering is glanzend, niet mat/korrelig

### Volgorde voor Beste Resultaat:
1. Soldeer eerst weerstanden aan BC547 (kleine componenten)
2. Soldeer BC547 sub-assemblage aan draden
3. Soldeer alles aan ESP32 (kan niet meer los)
4. Test voor inbouwen in behuizing

### Tools die Handig Zijn:
- Derde hand / PCB houder
- Multimeter voor testen
- Desoldeerzuigpomp (voor fouten)
- Flux pen (voor betere tin flow)
- Losse oog / vergrootglas

---

## 📸 Bestanden in Repository

| Bestand | Beschrijving |
|---------|--------------|
| `Slimme_meter_esp32_V2.ino` | Software Interface Module |
| `Slimme_meter_esp32_relais_V2.ino` | Software Relais Module |
| `EENVOUDIG_RELAIS_SCHEMA.md` | Eenvoudig conceptueel schema |
| `SOLDEERDIAGRAM_RELAIS_INJECTIE.md` | Uitgebreid technisch document |
| `SOLDEERDIAGRAM_PINVERBINDINGEN.md` | Dit document (stap-voor-stap) |

---

## 🆘 Hulp Nodig?

### Debug Checklist:
1. Controleer alle verbindingen met multimeter
2. Upload eenvoudige blink sketch om ESP32 te testen
3. Test BC547 met simpele LED schakeling
4. Controleer Serial Monitor output (115200 baud)
5. Vraag hulp op Arduino / ESP32 forum

### Veiligheid:
- Bij twijfel over 220V: **STOP**
- Raadpleeg erkend elektricien
- Test eerst alles op 12V

---

**Succes met solderen!** 🔨⚡

*Live long and prosper!* 🖖

**thieu-b55 | Versie 2.0 | November 2024**
