#!/usr/bin/python

# Read data from I2C sensors and send it to a remote UDP socket.

import json
import serial

from multiprocessing import Process
import time
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


import os, sys, inspect

cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile( inspect.currentframe() ))[0]))
if cmd_folder not in sys.path:
	sys.path.insert(0, cmd_folder)

cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"Adafruit-Raspberry-Pi-Python-Code")))
if cmd_subfolder not in sys.path:
	sys.path.insert(0, cmd_subfolder)

cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"Adafruit-Raspberry-Pi-Python-Code/Adafruit_BMP085")))
if cmd_subfolder not in sys.path:
	sys.path.insert(0, cmd_subfolder)

from Adafruit_BMP085 import BMP085

bmp = BMP085(0x77)


# I2C output files

def readI2C():
	itempf = open("data/itemperature.txt", "a")
	ipresf = open("data/ipressure.txt", "a")
	ialtif = open("data/ialtitude.txt", "a")

	x = 0
	while True:
		temp = bmp.readTemperature()
		pressure = bmp.readPressure()
		altitude = bmp.readAltitude()

		lastitemp = "%.2f" % temp
		lastipres = "%.2f" % (pressure / 100.0)
		lastialti = "%.2f" % altitude

		lastitemp = "Temperature: %.2f C" % temp
		lastipres = "Pressure:    %.2f hPa" % (pressure / 100.0)
		lastialti = "Altitude:    %.2f" % altitude

		itempf.write(str(time.time()) + " " + lastitemp + '\n')
		ipresf.write(str(time.time()) + " " + lastipres + '\n')
		ialtif.write(str(time.time()) + " " + lastialti + '\n')

		itempf.flush()
		ipresf.flush()
		ialtif.flush()

#		print "Temperature: %.2f C" % temp
#		print "Pressure:    %.2f hPa" % (pressure / 100.0)
#		print "Altitude:    %.2f" % altitude

		if x % 5 == 0:
			sockstr = "Temperature " + lastitemp + "\nPressure    " + lastipres + "\nAltitude    " + lastialti + "\n"
			sock.sendto(sockstr, ("192.168.10.1", 9930))
		x = x + 1

		time.sleep(1)

def readSerial():
	sensors = ["pressure","imux","temperature","light","capacitor","conductance"]
	files = {}
	for sensor in sensors:
		files[sensor] = open("data/" + sensor + ".txt", "a")

	genericdata = open("data/generic.txt", "a")

	ser = serial.Serial('/dev/ttyACM0', 115200)
	while True:
		x = ser.readline()
		try:
			f = json.loads(x)
			for sensor in sensors:
				if sensor in f:
					files[sensor].write(str(time.time()) + " " + x + '\n')
					files[sensor].flush()
			genericdata.write(str(time.time()) + " " + x + '\n')
			genericdata.flush()
		except:
			# corrupt json
			pass

if __name__ == '__main__':
	p = Process(target=readI2C, args=())
	p.start()
	q = Process(target=readSerial, args=())
	q.start()
