# Hier worden de lybraries geïmporteerd.
# Ik heb lgpio ivp Gpio zero gebruikt omdat deze library een aantal problemen met de servo’s en de ultrasone sensors oplossen.
import time
import paho.mqtt.client as mqtt
import lgpio
from gpiozero import Servo, Buzzer
from time import sleep

# WiFi & MQTT gegevens
MQTT_SERVER = "127.0.0.1"
MQTT_PORT = 1883
MQTT_USER = "maart"
MQTT_PASSWORD = "Five"
MQTT_CLIENT_ID = "raspberry_pi_slagboom"
MQTT_TOPIC_SENSOR = "esp32/slagboom/sensor"
MQTT_TOPIC_LAMP = "esp32/slagboom/lamp"
MQTT_TOPIC_LDR = "esp32/trein/ldr"

# Servo & sensor pins
SERVO_PIN1 = 17
SERVO_PIN2 = 16
TRIG_PIN1 = 5
ECHO_PIN1 = 18
TRIG_PIN2 = 7
ECHO_PIN2 = 12
LED_PIN1 = 21
BUZZER_PIN = 23

# GPIO-instellingen
# Dit is een beetje zoals de void setup: elke pin als output of input instellen
h = lgpio.gpiochip_open(0)
lgpio.gpio_claim_output(h, TRIG_PIN1)
lgpio.gpio_claim_input(h, ECHO_PIN1)
lgpio.gpio_claim_output(h, TRIG_PIN2)
lgpio.gpio_claim_input(h, ECHO_PIN2)
lgpio.gpio_claim_output(h, LED_PIN1)
servo1 = Servo(SERVO_PIN1)
servo2 = Servo(SERVO_PIN2)
buzzer = Buzzer(BUZZER_PIN)

# Ultrasone sensor instellingen
SOUND_SPEED = 0.034
Laatste_status_openen = False
Laatste_status_sluiten = True
interval = 0.5

# Hier worden de ultrasone sensors uitgelezen
def measure_distance(trig, echo):
    lgpio.gpio_write(h, trig, 0)
    time.sleep(0.1)
    lgpio.gpio_write(h, trig, 1)
    time.sleep(0.00001)
    lgpio.gpio_write(h, trig, 0)
    
    start_time = time.time()
    end_time = start_time
    
    while lgpio.gpio_read(h, echo) == 0:
        start_time = time.time()
        if start_time - end_time > 0.1:
            return float('inf')
    
    while lgpio.gpio_read(h, echo) == 1:
        end_time = time.time()
        if end_time - start_time > 0.1:
            return float('inf')
    
    duration = end_time - start_time
    distance = (duration * 34300) / 2
    return distance

# Hier worden de MQTT berichten gedecodeerd
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Verbonden met MQTT Broker")
        client.subscribe(MQTT_TOPIC_LDR)
    else:
        print(f"Verbindingsfout met code {rc}, herstart...")
        time.sleep(5)
        client.reconnect()

def on_message(client, userdata, msg):
    message = msg.payload.decode()
    print(f"Bericht ontvangen: {msg.topic} - {message}")
    
    if msg.topic == MQTT_TOPIC_LDR:
        try:
            value = int(message)
            if value > 2893 and value != 0:
                lgpio.gpio_write(h, LED_PIN1, 1)
                client.publish(MQTT_TOPIC_LAMP, "true", retain=True)
            else:
                lgpio.gpio_write(h, LED_PIN1, 0)
                client.publish(MQTT_TOPIC_LAMP, "false", retain=True)
        except ValueError:
            print("Ongeldige LDR waarde ontvangen")

# Dit is het “main” script dit wordt als eerste aangeroepen 
def main():
    # Hiermee wordt eerst alles goed gezet, vooraleer we verder gaan naar de rest van het script.
    global Laatste_status_sluiten, Laatste_status_openen
    client = mqtt.Client(client_id=MQTT_CLIENT_ID)
    client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_SERVER, MQTT_PORT, 60)
    client.loop_start()

    try:
        # Dit kan je vergelijken met een void loop om dit de simuleren gebruik ik een while true
        while True:
             # Als eerste worden de sensoren uitgelezen
            # De waardes van de sensoren worden geprint om te debuggen
            afstand_sluiten = measure_distance(TRIG_PIN1, ECHO_PIN1)
            afstand_openen = measure_distance(TRIG_PIN2, ECHO_PIN2)

            if afstand_sluiten == float('inf') or afstand_openen == float('inf'):
                print("Fout bij meten van afstand")
                continue

            print(f"Afstand Sluiten: {afstand_sluiten:.2f} cm | Afstand Openen: {afstand_openen:.2f} cm")

            Trein_Gezien = afstand_sluiten < 10
            Trein_Voorbij = afstand_openen < 10  # Trein is bij sensor 2
            Trein_Helemaal_Weg = afstand_openen > 15  # Trein is voorbij sensor 2
            # Als de trein gezien is bij sensor 1, dan wordt dit verstuurd met MQTT en worden de slagbomen gesloten.
            if Trein_Gezien != Laatste_status_sluiten:
                client.publish(MQTT_TOPIC_SENSOR, "true" if Trein_Gezien else "false", retain=True)
                Laatste_status_sluiten = Trein_Gezien

            if Trein_Gezien:
                buzzer.on()
                servo1.value = -1  # Slagboom sluiten
                servo2.value = -1

                # Wachten tot de trein voorbij sensor 2 is
                while True:
                    afstand_sluiten = measure_distance(TRIG_PIN1, ECHO_PIN1)
                    afstand_openen = measure_distance(TRIG_PIN2, ECHO_PIN2)

                    Trein_Voorbij = afstand_openen < 10

                    print(f"Wachten op trein bij sensor 2: {Trein_Voorbij}")

                    # Hier wordt er gewacht totdat de trein tot sensor 2 is gereden en dan stopt de buzzer met beepen
                    if Trein_Voorbij:
                        break  # De trein is voorbij sensor 1 en bereikt sensor 2

                    buzzer.on()
                    sleep(1)
                    buzzer.off()
                    sleep(1)

                # Wachten tot de trein echt helemaal weg is
                while True:
                    afstand_openen = measure_distance(TRIG_PIN2, ECHO_PIN2)
                    Trein_Helemaal_Weg = afstand_openen > 15

                    print(f"Wachten tot trein weg is bij sensor 2: {Trein_Helemaal_Weg}")

                    if Trein_Helemaal_Weg:
                        break  # Trein is echt weg

                    sleep(1)
            # Als de trein voorbij sensor 2 is gereden, dan pas gaat de slagboom open.
            if Trein_Helemaal_Weg:
                if Trein_Helemaal_Weg != Laatste_status_openen:
                    client.publish(MQTT_TOPIC_SENSOR, "false", retain=True)
                    Laatste_status_openen = Trein_Helemaal_Weg

                servo1.value = 1  # Slagboom openen
                servo2.value = 1
                buzzer.off()

            time.sleep(interval)

    except KeyboardInterrupt:
        print("Beëindigen...")
        lgpio.gpiochip_close(h)
        client.loop_stop()
        client.disconnect()
    except Exception as e:
        print(f"Fout opgetreden: {e}")
        lgpio.gpiochip_close(h)
        client.loop_stop()
        client.disconnect()


if __name__ == "__main__":
    main()
