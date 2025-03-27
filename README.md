# Projectbeschrijving

Dit project betreft een op afstand bestuurbare trein via de Blynk-app. De trein is verbonden met een slagboom. Wanneer de ultrasone sensor bij de slagboom detecteert dat er een trein nadert, sluiten de slagbomen automatisch. Dit signaal wordt via MQTT naar de trein gestuurd, zodat deze kan vertragen en de spoorwegovergang veilig kan passeren.

Een extra functie is de LED-verlichting: witte LED’s aan de voorkant en rode aan de achterkant. Deze worden aangestuurd door een LDR in de trein. De trein stuurt de gemeten lichtsterkte via MQTT naar de Raspberry Pi, waardoor de spoorwegovergang indien nodig verlicht kan worden.

## YouTube-video

https://youtu.be/Fh8W_xxydbA

## Benodigdheden

### Voor de trein

- 1 LEGO-trein aangedreven door een 9V-motor
- 1 ESP32
- 1 9V-batterij
- 1 BC547-transistor
- 1 LM317 (om de spanning stabiel op 5V te houden voor de ESP32)
- 1 LDR
- 3 witte LED’s
- 2 rode LED’s
- 1 0.1µF-condensator
- 1 1µF-condensator

### Weerstanden

- 1 × 10KΩ voor de LDR 
- 1 × 150Ω voor de transistor
- 1 × 4K7Ω voor de LM317
- 1 × 3K5Ω voor de LM317
- 2 × 220Ω voor de LED’s

**Installatieschema:**  
![Afbeelding1](https://github.com/user-attachments/assets/280502ff-5222-44b1-aa33-a41e6af3328c)

### Voor de slagboom

- 1 Raspberry Pi (5)
- 2 microservo’s
- 1 buzzer
- 1 LED
- 2 ultrasone sensoren
- 2 × 220Ω-weerstanden voor de LED’s

**Installatieschema:**  
![Afbeelding2](https://github.com/user-attachments/assets/97527deb-a451-4083-9abf-535014a7d9de)

### Algemeen

- Voldoende bekabeling
- Spoorweg

## Blynk-app

![Afbeelding4](https://github.com/user-attachments/assets/f01737ed-76aa-479e-a961-ed1bea47fae9)

- **Linksboven:** Geeft de huidige snelheid weer.
- **Rechts:** Lampicoon dat aangeeft of de veilige modus actief is.
- **Slider:** Wordt gebruikt om de trein in beweging te zetten.
- **Stopknop:** Om de trein te stoppen.
- **Rechteronderste lamp:** Toont welke verlichtingsmodus actief is.
- **Linkeronderste lamp:** Geeft aan of de verlichting is ingeschakeld via de knop of automatisch via de LDR.
