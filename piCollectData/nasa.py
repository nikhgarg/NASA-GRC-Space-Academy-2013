#!/usr/bin/python

# Read data from I2C sensors and send it to a remote UDP socket.

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

itempf = open("data/itemperature.txt", "a")
ipresf = open("data/ipressure.txt", "a")
ialtif = open("data/ialtitude.txt", "a")

lastitemp = ""
lastipres = ""
lastialti = ""

def readI2C():
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

#		print "Temperature: %.2f C" % temp
#		print "Pressure:    %.2f hPa" % (pressure / 100.0)
#		print "Altitude:    %.2f" % altitude

		if x % 5 == 0:
			str = "Temperature " + lastitemp + "\nPressure    " + lastipres + "\nAltitude    " + lastialti + "\n"
			sock.sendto(str, ("192.168.10.223", 9930))
		x = x + 1

		time.sleep(1)


if __name__ == '__main__':
	p = Process(target=readI2C, args=())
	p.start()
