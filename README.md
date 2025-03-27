# PROJECT BESCHRIJVING
Het project is een trein die ik kan aangestuurd worden via de Blynk-app. De trein staat in verbinding met de slagboom. Wanneer de ultrasone sensor bij de slagboom detecteert dat er een trein voorbijrijdt, sluiten de slagbomen. Dit wordt gecommuniceerd met de trein via MQTT, zodat de trein kan vertragen en met een veilige snelheid de spoorwegovergang kan passeren.

Een leuke toevoeging is de LED-verlichting: witte LED’s aan de voorkant en rode aan de achterkant. Deze worden aangestuurd door een LDR in de trein. De trein stuurt de gemeten waarde via MQTT naar de Raspberry Pi, waardoor de spoorwegovergang ook verlicht kan worden.

# Wat heb je nodig voor dit projeckt?


Voor de trein:
1 lego trein aangedreven door een 9v motor
1 esp32
1 9V batterij
1 bc547 transistor
1 LM317 om de spanning stabiel op 5v te houden voor de esp
1 LDR
3 witte led’s
2 rode led’s
1 0.1µf capacitor
1 1µF capacitor


Instalatie schema:


![Afbeelding1](https://github.com/user-attachments/assets/280502ff-5222-44b1-aa33-a41e6af3328c)


Voor de slagboom
1 Rapsberry pi (5)
2 micro servo’s
1 buzzer
1 led
2 ultrasone sensors
2 220Ω weerstanden voor de led’s


instalatie schema:


![Afbeelding2](https://github.com/user-attachments/assets/97527deb-a451-4083-9abf-535014a7d9de)

Algemeen
Genoeg kabels
Spoorweg


Weerstanden:
1 10KΩ voor de LDR 
1 150Ω voor de transistor
1 4k7Ω voor de LM317
1 3k5Ω voor de LM317
2 220Ω voor de led’s 


# youtube video:
https://youtu.be/Fh8W_xxydbA
