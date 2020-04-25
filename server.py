import paho.mqtt.client as mqtt
import json

# MQTT Settings
MQTT_Broker = "192.168.137.1"
MQTT_Port = 1883
Keep_Alive_Interval = 45
MQTT_Topic1 = "Temperature"
MQTT_Topic2 = "Humidity"

def on_connect(client, userdata, flags, rc):
    client.subscribe(MQTT_Topic1)
    client.subscribe(MQTT_Topic2)

#Save Data
def on_message(client, userdata, msg):
    print ("MQTT Data Received...")
    print ("MQTT Topic: " + msg.topic)
    data = msg.payload.decode("utf-8")
    print ("Data: ", data)
    data=json.loads(data)
#    print (data['Date'])

client = mqtt.Client()
client.on_message = on_message
client.on_connect = on_connect
client.connect(MQTT_Broker, int(MQTT_Port), int(Keep_Alive_Interval))

client.loop_forever()