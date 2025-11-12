# Eenvoudig Relais Schema - Schakel bij Injectie

## 🎯 Wat doet dit systeem?

**Simpel:** Wanneer je zonnepanelen genoeg stroom terugleveren (injectie), gaat het relais AAN en schakelt een apparaat in.

---

## 📦 Wat heb je nodig?

### Module 1: Interface (Slimme meter uitlezer)
- 1x ESP32 DevKit (~€8)
- 1x BC547 transistor (~€0.10)
- 1x RJ12 kabel met connector (~€3)
- 3x Weerstand: 1x 2.2kΩ, 1x 10kΩ, 1x 10kΩ (~€0.30)
- **OPTIE A:** 3x 4700µF condensator (voeding via P1 poort)
- **OPTIE B:** Externe 5V voeding (~€5) ⭐ **AANBEVOLEN**

### Module 2: Relais (Schakelaar)
- 1x ESP32 DevKit (~€8)
- 1x 5V Relais module 30A (~€10)
  - Link: https://nl.aliexpress.com/item/4000185959463.html
  - **Kies de 5V uitvoering!**
- 1x Externe 5V voeding (~€5)

**Totaal:** ~€35-40 (zonder kabels/soldeermateriaal)

---

## 🔌 Module 1: Interface (bij de slimme meter)

### Eenvoudig Schema:

```
                    SLIMME METER P1 POORT
                    ┌──────────────────┐
                    │  RJ12 Connector  │
                    │  (6 pinnen)      │
                    └────────┬─────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
   Pin 1 (+5V)          Pin 5 (Data)         Pin 3+6 (GND)
        │                    │                    │
        │                    │                    │
        │              2.2kΩ │                    │
        │                ┌───┴───┐                │
        │                │       │                │
        │            ┌───┤  BC547├───┐            │
        │            │   │       │   │            │
        │            │   └───────┘   │            │
        │            │               │            │
        │       10kΩ │               │ (C)        │
        │       ┌────┴────┐          │            │
        │       │         │          │            │
        └───────┤5V   ESP32   GPIO16 ├────────────┘
                │         │
                │    GND  ├──────────────────────────┘
                └─────────┘
                   (E)

VOEDING OPTIE A (via P1 poort):
Pin 1 (+5V) ──┬── 4700µF ─┬── 4700µF ─┬── 4700µF ──→ ESP32 5V
              │           │           │
             GND         GND         GND

VOEDING OPTIE B (externe voeding) ⭐ AANBEVOLEN:
Externe 5V adapter ──→ ESP32 5V
                    └──→ ESP32 GND
(P1 Pin 2 Data Request moet WEL met P1 Pin 1 verbonden blijven via 10kΩ!)
```

### Aansluitingen Interface ESP32:

| Van                | Naar           | Via            |
|--------------------|----------------|----------------|
| P1 Pin 1 (+5V)     | ESP32 5V       | 3x 4700µF OF externe voeding |
| P1 Pin 2 (Data Req)| P1 Pin 1       | 10kΩ weerstand |
| P1 Pin 5 (Data)    | BC547 Base     | 2.2kΩ weerstand|
| BC547 Collector    | ESP32 GPIO16   | Direct         |
| BC547 Emitter      | ESP32 GND      | Direct         |
| ESP32 3.3V         | BC547 Collector| 10kΩ pull-up   |
| P1 Pin 3+6 (GND)   | ESP32 GND      | Direct         |

**Belangrijk:** Pin 2 (Data Request) moet altijd via 10kΩ naar +5V, ook bij externe voeding!

---

## 🔌 Module 2: Relais (bij het apparaat)

### Eenvoudig Schema:

```
    EXTERNE 5V VOEDING          ESP32 DevKit
         │                      ┌─────────┐
         ├──────────────────────┤ 5V      │
         │                      │         │
         │                      │ GPIO16 ─┼────┐
         │                      │         │    │
         └──────────────────────┤ GND     │    │
                                └─────────┘    │
                                               │
                                               │
                    5V RELAIS MODULE            │
                    ┌────────────────┐          │
        5V ─────────┤ DC+            │          │
                    │                │          │
       GND ─────────┤ DC-            │          │
                    │                │          │
  (van GPIO16) ─────┤ IN             │          │
                    │                │          │
   220V Fase ───────┤ COM            │
                    │                │
   Naar Apparaat ───┤ N.O. ●         │
                    │                │
   (niet gebruiken)─┤ N.C.           │
                    └────────────────┘

   220V Nul ────────────────────────→ Naar Apparaat


RELAIS WERKING:
• Normaal: Contact open, apparaat UIT
• Bij injectie: GPIO16 HIGH → Contact sluit → Apparaat AAN
```

### Aansluitingen Relais ESP32:

| Van                | Naar           |
|--------------------|----------------|
| 5V Voeding +       | ESP32 5V       |
| 5V Voeding -       | ESP32 GND      |
| 5V Voeding +       | Relais DC+     |
| 5V Voeding -       | Relais DC-     |
| ESP32 GPIO16       | Relais IN      |
| 220V Fase (bruin)  | Relais COM     |
| Relais N.O.        | Apparaat Fase  |
| 220V Nul (blauw)   | Apparaat Nul   |

---

## 💻 Software Installatie

### Stap 1: Interface Module Programmeren

1. Download van GitHub: `Slimme_meter_esp32_V2.ino`
2. Open in Arduino IDE
3. Selecteer: Board: "ESP32 Dev Module"
4. Selecteer juiste COM poort
5. Upload naar ESP32
6. Open Serial Monitor (115200 baud)
7. **Noteer het MAC adres!** (bijvoorbeeld: `7c:87:ce:2e:0a:88`)

### Stap 2: Relais Module Programmeren

1. Download van GitHub: `Slimme_meter_esp32_relais_V2.ino`
2. Open in Arduino IDE
3. Upload naar tweede ESP32
4. **Noteer ook dit MAC adres!**

### Stap 3: MAC Adressen Instellen

**Via WiFi webinterface:**
1. Verbind met WiFi: `ESP32Energie_V2`
2. Wachtwoord: `ESP32pswd`
3. Ga naar: `192.168.4.1`
4. Klik: **Instellingen**
5. Scroll naar: **Ingeven MAC address**
6. Selecteer: **MAC address Relais 1**
7. Vul MAC adres van Relais module in
8. Klik: **OK**
9. ESP32 herstart automatisch

### Stap 4: Injectie Drempel Instellen

Op dezelfde webpagina (192.168.4.1):

**Voor automatisch schakelen bij injectie:**
- Scroll naar: **Relais schakelwaarden**
- Selecteer: **Relais 1**
- **KW:** `0.5` *(schakelt bij 500W injectie)*
- **Tijd:** `00:01` *(schakeling mogelijk vanaf 00:01 uur)*
- **A/M:** `A` *(Automatisch)*
- Klik: **OK**

**Wat betekent dit:**
- Vanaf 00:01 uur (= heel de dag)
- ALS injectie ≥ 500W
- DAN relais AAN
- Tot 00:00 uur (reset)

---

## 🧪 Testen

### Test 1: Interface Module
1. Sluit aan op P1 poort
2. Reset ESP32 (knop op board)
3. Wacht 30 seconden
4. Verbind WiFi: `ESP32Energie_V2` / `ESP32pswd`
5. Ga naar: `192.168.4.1`
6. Controleer of je "Injectie nu" ziet

### Test 2: Relais Module
1. Sluit aan op 5V voeding
2. Zet A/M op `1` (handmatig AAN)
3. Relais moet hoorbaar "klikken"
4. Zet A/M op `0` (handmatig UIT)
5. Relais moet weer klikken

### Test 3: Automatische Werking
1. Zet A/M terug op `A`
2. Zet KW op `0.1` (lage drempel voor testen)
3. Wacht op zonnige middag
4. Controleer op webpagina: "Relais 1: 1" (= aan)
5. 's Avonds moet relais uitgaan

---

## ⚡ VEILIGHEID - BELANGRIJK!

### ⚠️ 220V WAARSCHUWINGEN

**LEVENSGEVAARLIJK! Lees dit zorgvuldig:**

1. ❌ **Werk NOOIT aan 220V met spanning erop**
   - Schakel groep uit in meterkast
   - Controleer met spanningstester

2. ✅ **Laat 220V installatie doen door erkend elektricien**
   - Zelf solderen van ESP32 = OK
   - 220V aansluiten = NEEN (tenzij bevoegd)

3. ✅ **Gebruik IP-beschermde behuizing**
   - Relais in afgesloten kast
   - Nooit open 220V contacten

4. ✅ **Test eerst met 12V lamp**
   - Voordat je 220V aansluit
   - Controleer of relais schakelt

5. ✅ **Controleer relais specificaties**
   - Minimaal 10A voor 220V
   - 30A aanbevolen voor zware apparaten
   - Controleer of geschikt voor AC (wisselstroom)

**Bij twijfel: STOP en vraag hulp aan elektricien!**

---

## 🔧 Problemen Oplossen

| Probleem | Oorzaak | Oplossing |
|----------|---------|-----------|
| Geen data van P1 poort | P1 niet geactiveerd | Activeer via Fluvius.be |
| ESP32 blijft resetten | Onvoldoende stroom | Gebruik externe 5V voeding |
| Relais schakelt niet | Verkeerd MAC adres | Controleer MAC adressen |
| Relais blijft aan | Reset om 00:00 werkt niet | Check klok van slimme meter |
| Geen WiFi verbinding | Te ver van meter | Gebruik WiFi versterker |

---

## 📊 Wat gebeurt er precies?

### In de code:

```
1. Interface leest P1 poort elke seconde
2. Berekent: injectie_nu_float (hoeveel teruglevering)
3. Vergelijkt met relais1_on (jouw ingestelde 0.5kW)
4. ALS injectie ≥ 0.5kW EN tijd OK
   → Stuurt via ESP-NOW: relais = TRUE
5. Relais module ontvangt bericht
   → GPIO16 = HIGH
   → Relais schakelt
```

### Visueel:

```
☀️ ZONNIG WEER
├─ 10:00u → 0.3kW injectie → Relais UIT (< 0.5kW)
├─ 12:00u → 1.2kW injectie → Relais AAN (≥ 0.5kW) ✅
├─ 15:00u → 0.8kW injectie → Relais BLIJFT AAN
├─ 18:00u → 0.0kW injectie → Relais BLIJFT AAN
└─ 00:00u → Reset         → Relais UIT

🌙 VOLGENDE DAG
└─ Cyclus begint opnieuw
```

**LET OP:** Relais blijft AAN tot middernacht, ook als injectie stopt!

---

## 🎛️ Instellingen Voorbeelden

### Voorbeeld 1: Schakelen vanaf 1kW injectie, hele dag
```
KW: 1.0
Tijd: 00:01
A/M: A
```

### Voorbeeld 2: Alleen tussen 10:00 en 16:00
```
KW: 0.5
Tijd: 10:00
A/M: A

+ Extra relais instellingen aanpassen voor uitschakeling
(of handmatig A/M op 0 zetten om 16:00)
```

### Voorbeeld 3: Handmatig AAN (bijv. voor testen)
```
A/M: 1
(KW en Tijd worden genegeerd)
```

### Voorbeeld 4: Handmatig UIT
```
A/M: 0
```

---

## 📸 Welke Apparaten Zijn Geschikt?

### ✅ GOED (resistieve belasting):
- Elektrische kachel
- Elektrische boiler (zonder thermostaat)
- Gloeilampen
- Verwarmingselementen

### ⚠️ MOGELIJK (met juiste relais):
- Wasmachine (grote inschakelstroom)
- Vaatwasser
- Droger

### ❌ NIET AANBEVOLEN:
- Koelkasten (constante koeling nodig)
- Computers (data verlies)
- Medische apparaten

---

## 🚀 Uitbreidingen Later

Als je wilt upgraden kan je toevoegen:
- **Display module** → Lokale weergave
- **Tweede relais** → Tweede apparaat
- **PWM module** → Boiler met geleidelijke regeling
- **SD kaart** → Data opslag (al in Interface code)

Maar voor nu: **dit is alles wat je nodig hebt!**

---

## 📝 Checklist Installatie

- [ ] Interface ESP32 geprogrammeerd
- [ ] Relais ESP32 geprogrammeerd
- [ ] Beide MAC adressen genoteerd
- [ ] MAC adres ingevuld in Interface
- [ ] P1 poort aangesloten (met externe voeding)
- [ ] Relais module getest (handmatig)
- [ ] WiFi verbinding getest (192.168.4.1)
- [ ] Injectie drempel ingesteld (KW + Tijd)
- [ ] 220V aangesloten door elektricien
- [ ] Systeem getest bij zonnig weer
- [ ] Behuizing gemonteerd (waterdicht)

---

**Veel succes met je slimme energie-schakelaar!** ⚡☀️

*Live long and prosper! 🖖*

**Vragen? Check de originele PDF in de repo of vraag op het forum!**
