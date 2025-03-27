# Projectbeschrijving

Dit project betreft een op afstand bestuurbare trein via de Blynk-app. De trein is verbonden met een slagboom. Wanneer de ultrasone sensor bij de slagboom detecteert dat er een trein nadert, sluiten de slagbomen automatisch. Dit signaal wordt via MQTT naar de trein gestuurd, zodat deze kan vertragen en de spoorwegovergang veilig kan passeren.

Een extra functie is de LED-verlichting: witte LED’s aan de voorkant en rode aan de achterkant. Deze worden aangestuurd door een LDR in de trein. De trein stuurt de gemeten lichtsterkte via MQTT naar de Raspberry Pi, waardoor de spoorwegovergang indien nodig verlicht kan worden.

## Methodologie

Als eerste heb ik de LED’s geïnstalleerd. Deze heb ik parallel aangesloten, omdat de LED’s anders te veel spanning nodig zouden hebben. Dit ging vlot en gemakkelijk. Vervolgens onderzocht ik hoe ik zelf de Lego-treinmotor kon aansturen. Online vind je geen datasheets over deze motor, maar ik vond een website die de meeste Lego-motoren test en documenteert, wat erg handig bleek te zijn: [Philohome Motors](https://www.philohome.com/motors/motorcomp.htm).

Daarna heb ik de motor via een transistor aangesloten op een ESP32. Om de ESP32 te voeden, gebruikte ik een spanningsregelaar. Hoewel dit mogelijk overbodig was – online was dit zeer onduidelijk – deed ik dit voor de zekerheid. Vervolgens zette ik een kort stukje spoor op en voerde ik mijn eerste test uit.

Later heb ik de slagboom in elkaar gezet met een Raspberry Pi. Hierna heb ik de code geschreven, wat goed verliep: ik heb een aantal voorbeelden bij elkaar gevoegd en de meeste errors kunnen oplossen. Tot slot heb ik ook een LDR in de trein verwerkt en MQTT opgezet, zodat de slagboom en de trein met elkaar konden communiceren. Het afstellen van alle onderdelen samen was het moeilijkst; ik kon maar weinig in één keer realiseren, omdat er altijd wel iets niet werkte.

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

- #### Berekening voor de LM317:
![Afbeelding5](https://github.com/user-attachments/assets/de749cce-fd42-481f-9227-e17026dd67f0)
![Afbeelding6](https://github.com/user-attachments/assets/9024e3d5-a93e-41ae-a8d3-74ec9fe70265)

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

### Overige Weerstanden

- 1 × 10KΩ voor de LDR 
- 1 × 150Ω voor de transistor
- 1 × 4K7Ω voor de LM317
- 1 × 3K5Ω voor de LM317
- 2 × 220Ω voor de LED’s

# MQTT topics

esp32/trein/ldr: hiermee verstuurd de trein de rauwe data van de LDR naar de slagboom 
esp32/slagboom/lamp: hiermee wordt er naar de trein gestuurd of dat de lampen van de trein aan moeten
esp32/slagboom/sensor: hiermee wordt er verstuurd of dat de trein voorbij de sensor is gereden. 


## Blynk-app

![Afbeelding4](https://github.com/user-attachments/assets/f01737ed-76aa-479e-a961-ed1bea47fae9)

- **Linksboven:** Geeft de huidige snelheid weer.
- **Rechts:** Lampicoon dat aangeeft of de veilige modus actief is.
- **Slider:** Wordt gebruikt om de trein in beweging te zetten.
- **Stopknop:** Om de trein te stoppen.
- **Rechteronderste lamp:** Toont welke verlichtingsmodus actief is.
- **Linkeronderste lamp:** Geeft aan of de verlichting is ingeschakeld via de knop of automatisch via de LDR.
- Met de knoppen kunnen deze instellingen worden gewijzigd.

# Mogelijke uitbreidingen
Ik kan in plaats van een transistor een H-brug gebruiken, om de trein zowel vooruit als achteruit te laten rijden. Op dat moment had ik echter niet genoeg hardware en de benodigde onderdelen zouden niet op tijd aankomen, om dit ook nog te implementeren.

