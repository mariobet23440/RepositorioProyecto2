# Import standard python modules.
import sys
import time
import serial

# This example uses the MQTTClient instead of the REST client
from Adafruit_IO import MQTTClient
from Adafruit_IO import Client, Feed

# holds the count for the feed
run_count = 0

# Set to your Adafruit IO username and key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!
ADAFRUIT_IO_USERNAME = "mariobet04"
ADAFRUIT_IO_KEY = "aio_VhIs51L3AabBCEGHUqbkXZdhciiu"

# Set to the ID of the feed to subscribe to for updates.
FEED_ID_receive = 'Modo_TX'
FEED_ID_Send = 'Modo_RX'

FEEDS_RECEIVE = ['Modo_TX', 'Servomotor_X_TX', 'Servomotor_Y_TX','Motorreductor_X_TX','Motorreductor_Y_TX']


# Define "callback" functions which will be called when certain events
# happen (connected, disconnected, message arrived).
def connected(client):
    """Connected function will be called when the client is connected to
    Adafruit IO.This is a good place to subscribe to feed changes. The client
    parameter passed to this function is the Adafruit IO MQTT client so you
    can make calls against it easily.
    """
    print('Conectado a Adafruit IO. Suscribiéndose a los feeds...')
    for feed in FEEDS_RECEIVE:
        client.subscribe(feed)
        print(f'Subscrito al feed: {feed}')

def disconnected(client):
    """Disconnected function will be called when the client disconnects."""
    sys.exit(1)

def int_to_hexstr(value, length=2):
    # Convierte un int a string hex sin '0x', con padding a length caracteres (ej: '0A')
    return format(value, '0{}X'.format(length))

def message(client, feed_id, payload):
    """Message function will be called when a subscribed feed has a new value.
    The feed_id parameter identifies the feed, and the payload parameter has
    the new value.
    """

    print(f'Recibido mensaje de feed_id: "{feed_id}" con payload: "{payload}"')

    message  = ""

    if(feed_id == "Modo_TX"):
        # Activar Modo Manual
        if(payload == "MM"):
            message = "0GG1Z"
            print("Feed Modo_TX : Cambiando a modo Manual - " + message)
        
        # Activar Modo UART
        elif(payload == "UU"):
            message = "0HH1Z"
            print("Feed Modo_TX : Cambiando a modo UART - " + message)
        
        # Activar Modo EEPROM1
        elif(payload == "E1"):
            message = "0EE1Z"
            print("Feed Modo_TX : Cambiando a modo EEPROM1 - " + message)

        # Activar Modo EEPROM2
        elif(payload == "E2"):
            message = "0EE2Z"
            print("Feed Modo_TX : Cambiando a modo EEPROM2 - " + message)

        # Activar Modo EEPROM3
        elif(payload == "E3"):
            message = "0EE3Z"
            print("Feed Modo_TX : Cambiando a modo EEPROM3 - " + message)

        # Activar Modo EEPROM4
        elif(payload == "E4"):
            message = "0EE4Z"
            print("Feed Modo_TX : Cambiando a modo EEPROM4 - " + message)

        # Guardar en EEPROM1
        elif(payload == "G1"):
            message = "0FF1Z"
            print("Feed Modo_TX : Guardando posición en EEPROM1 - " + message)

        # Activar Modo EEPROM2
        elif(payload == "E2"):
            message = "0FF2Z"
            print("Feed Modo_TX : Guardando posición en EEPROM2 - " + message)

        # Activar Modo EEPROM3
        elif(payload == "E3"):
            message = "0FF3Z"
            print("Feed Modo_TX : Guardando posición en EEPROM3 - " + message)

        # Activar Modo EEPROM4
        elif(payload == "E4"):
            message = "0FF4Z"
            print("Feed Modo_TX : Guardando posición en EEPROM4 - " + message)

    elif(feed_id == "Motorreductor_X_TX"):
        try:
            ascii_char = chr(int(payload))
            message = "0AA" + ascii_char + "Z"
            print("Feed Motorreductor_X_TX : Cambiando posición de motorreductor - " + message)
        except ValueError:
            print("Payload inválido para Motorreductor_X_TX")

    elif(feed_id == "Motorreductor_Y_TX"):
        try:
            ascii_char = chr(int(payload))
            message = "0BB" + ascii_char + "Z"
            print("Feed Motorreductor_Y_TX : Cambiando posición de motorreductor - " + message)
        except ValueError:
            print("Payload inválido para Motorreductor_Y_TX")
    
    elif(feed_id == "Servomotor_X_TX"):
        try:
            ascii_char = chr(int(payload))
            message = "0CC" + ascii_char + "Z"
            print("Feed Servomotor_X_TX : Cambiando posición de servomotor - " + message)
        except ValueError:
            print("Payload inválido para Servomotor_X_TX")

    elif(feed_id == "Servomotor_Y_TX"):
        try:
            ascii_char = chr(int(payload))
            message = "0DD" + ascii_char + "Z"
            print("Feed Servomotor_Y_TX : Cambiando posición de servomotor - " + message)
        except ValueError:
            print("Payload inválido para Servomotor_Y_TX")

    if message:
        # Enviar caracteres
        miArduino.write(message.encode('utf-8'))

        # Publish or "send" message to corresponding feed
        print('Sending data back: {0}'.format(payload))
        client.publish(FEED_ID_Send, payload)


# Create an MQTT client instance.
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
miArduino = serial.Serial(port='COM4', baudrate=9600, timeout=0.1)

# Setup the callback functions defined above.
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message

# Connect to the Adafruit IO server.
client.connect()

# The first option is to run a thread in the background so you can continue
# doing things in your program.
client.loop_background()

while True:
    """
    # Uncomment the next 3 lines if you want to constantly send data
    # Adafruit IO is rate-limited for publishing
    # so we'll need a delay for calls to aio.send_data()
    """
    #run_count += 1
    #print('sending count: ', run_count)
    #client.publish(FEED_ID_Send, run_count)
    print('Running "main loop" ')
    time.sleep(10)