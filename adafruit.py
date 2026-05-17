# Import standard python modules.
import sys
import time
import serial # Necesario para hablar con el Arduino

# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient

# --- CONFIGURACIÓN SERIAL ---
# Verifica el puerto COM en el Administrador de Dispositivos
PUERTO_SERIAL = 'COM3' 
BAUD_RATE = 9600

try:
    arduino = serial.Serial(PUERTO_SERIAL, BAUD_RATE, timeout=1)
    time.sleep(2) # Espera a que el Arduino reinicie
    print(f"Conectado al Arduino en {PUERTO_SERIAL}")
except:
    print("Error conectando al Arduino. Revisa el puerto COM.")
    sys.exit(1)

# Set to your Adafruit IO username and key.
ADAFRUIT_IO_USERNAME = "Lolo10"
ADAFRUIT_IO_KEY = "aio_LLTr67vKezBV6jhzSmoiMgR41mML"

# Set to the ID of the feed to subscribe to for updates.
FEED_ID_receive = 'Animatronic'

# Define "callback" functions
def connected(client):
    print('Subscribing to Feed {0}'.format(FEED_ID_receive))
    client.subscribe(FEED_ID_receive)
    print('Waiting for feed data...')

def disconnected(client):
    sys.exit(1)

def message(client, feed_id, payload):
    """
    Esta función se activa cuando presionas un botón en Adafruit.
    Enviamos el payload directamente al Arduino.
    """
    print('Feed {0} recibió: {1}'.format(feed_id, payload))
    
    # Agregamos \r\n para que tu función UART_leerLinea() en C sepa que terminó el comando
    comando = f"{payload}\r\n"
    
    try:
        arduino.write(comando.encode('ascii'))
        print(f"Enviado al Arduino: {payload}")
    except Exception as e:
        print(f"Error enviando al serial: {e}")


# Create an MQTT client instance.
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# Setup the callback functions defined above.
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message

# Connect to the Adafruit IO server.
client.connect()
client.loop_background()

# Main Loop
while True:
    print('Esperando comandos desde Adafruit IO...')
    time.sleep(5)