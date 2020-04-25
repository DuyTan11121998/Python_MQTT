import paho.mqtt.client as mqtt
import random, _thread, json
from datetime import datetime
import time
import serial
#====================================================
# MQTT Settings 
MQTT_Broker = "192.168.137.1"
MQTT_Port = 1883
Keep_Alive_Interval = 45
MQTT_Topic_Humidity = "Humidity"
MQTT_Topic_Temperature = "Temperature"


i=0
mgs = dict()
#====================================================
ser = serial.Serial (
	port		="/dev/ttyS0",
	baudrate	= 9600,
	parity		=serial.PARITY_NONE,
	stopbits	=serial.STOPBITS_ONE,
	bytesize	=serial.EIGHTBITS,
	timeout		=1)

def on_connect(client, userdata, rc):
	if rc != 0:
		pass
		print ("Unable to connect to MQTT Broker...")
	else:
		print ("Connected with MQTT Broker: " + str(MQTT_Broker))

def on_publish(client, userdata, mid):
	pass

def on_disconnect(client, userdata, rc):
	if rc !=0:
		pass

client = mqtt.Client()
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish
client.connect(MQTT_Broker, int(MQTT_Port), int(Keep_Alive_Interval))

def publish_To_Topic(topic, message):
	client.publish(topic,message)
	print ("Published: " + str(message) + " " + "on MQTT Topic: " + str(topic))
	print  (" ")

def read_uart():
	global i
	data = ser.read()
	data_left = ser.inWaiting()  # check for remaining byte
	data += ser.read(data_left)
	if (data) and (ord(data) == 254):
		while True:
			data = ser.read()
			data_left = ser.inWaiting()  # check for remaining byte
			data += ser.read(data_left)
			if data:
				data = ord(data)
				i = i + 1
				if i == 1:
					humid = data
				elif i == 2:
					humid = float(float(humid) + float(data) / 10)
				elif i == 3:
					temp = data
				elif i == 4:
					temp = float(float(temp) + float(data) / 10)
					#mgs= "temp: ",temp,",","humid: ",humid
					#print mgs
					i=0
					mgs['temp']=temp
					mgs['humid']=humid
					return 1

def thread_uart(thread,delay):
	while True:
		print ("thread is",thread)
		read_uart()
#			print (mgs)
		time.sleep(delay)

def publish_Sensor_Values_to_MQTT(threadname,delay):
	time.sleep(2)
	while(1):
		print("thread is ",threadname)
#		print (mgs['temp'],mgs['humid'])
		Sensor_data ={}
		Sensor_data['ID']= "DHT11"
		Sensor_data['Date'] = (datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f")
		Sensor_data['Humidity'] = mgs['humid']
		Sensor_data['Temperature'] = mgs['temp']
		Sensor_json_data = json.dumps(Sensor_data)
		publish_To_Topic(MQTT_Topic_Humidity, Sensor_json_data)
		time.sleep(delay)

try:
   _thread.start_new_thread( thread_uart, ("Read_Data", 0.1, ))
   _thread.start_new_thread( publish_Sensor_Values_to_MQTT, ("Publish", 10, ))
except:
   print ("Error: unable to start thread")

while 1:
   pass

