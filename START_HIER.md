# 🚀 Start Hier - Relais Injectie Detectie

## Welk document heb ik nodig?

### 👉 Voor eenvoudige aan/uit schakelaar:
**→ Lees: [EENVOUDIG_RELAIS_SCHEMA.md](EENVOUDIG_RELAIS_SCHEMA.md)**

Dit is wat je wilt als je:
- Een apparaat AAN wilt schakelen bij injectie
- Geen geleidelijke regeling nodig hebt
- Het zo simpel mogelijk wilt houden

**Kosten: ~€35-40**

---

### 🔧 Voor geavanceerde PWM regeling:
**→ Lees: [SOLDEERDIAGRAM_RELAIS_INJECTIE.md](SOLDEERDIAGRAM_RELAIS_INJECTIE.md)**

Dit is wat je wilt als je:
- Een elektrische boiler hebt
- EXACT je overschot wilt gebruiken (geen Watt te veel of weinig)
- Geleidelijke regeling nodig hebt (PWM)

**Kosten: ~€50-60**

---

### 📚 Voor volledige project documentatie:
**→ Lees: [Slimme meter ESP32 V2.pdf](Slimme%20meter%20ESP32%20V2.pdf)**

Complete documentatie met:
- Alle 4 modules (Interface, Display, PWM, Relais)
- Data opslag op SD kaart
- Webinterface uitleg
- Alle geavanceerde functies

---

## ⚡ Snelstart (Eenvoudig Relais)

1. **Koop onderdelen:** (~€35)
   - 2x ESP32 DevKit
   - 1x 5V Relais module (30A)
   - 1x BC547 transistor
   - 1x RJ12 kabel
   - Weerstanden: 2.2kΩ, 2x 10kΩ
   - 2x 5V voeding

2. **Soldeer volgens schema:**
   - Zie: [EENVOUDIG_RELAIS_SCHEMA.md](EENVOUDIG_RELAIS_SCHEMA.md)

3. **Programmeer ESP32's:**
   - Interface: `Slimme_meter_esp32_V2.ino`
   - Relais: `Slimme_meter_esp32_relais_V2.ino`

4. **Configureer via WiFi:**
   - SSID: `ESP32Energie_V2`
   - Wachtwoord: `ESP32pswd`
   - IP: `192.168.4.1`

5. **Test het systeem!**

---

## ⚠️ Veiligheid Eerst!

- ❌ Werk NOOIT aan 220V met spanning erop
- ✅ Laat 220V aansluiting doen door elektricien
- ✅ Test eerst met 12V voordat je 220V aansluit

---

**Veel succes! 🎉**
