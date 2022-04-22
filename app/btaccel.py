#!/usr/bin/python

import bluetooth

def linkConnect (addr):

	# Prepare to connect to the Linkmatik
	print "Preparing to connect to Linkmatik!\n"
	service = bluetooth.find_service (name = "Bluetooth Serial Port", address = addr)
	
	if (len (service) < 1):
		print "Initial try failing. Trying again.\n"
		service = bluetooth.find_service (name = "Bluetooth Serial Port", address = addr)
		if (len (service) < 1):
			print "Second try failed!.\n"
			return

	# Prepare variables
	btS0 = service[0]
	port = btS0["port"]
	host = btS0["host"]
	btS0Addr = (host, port)

	# Connect to the serial port on the Linkmatik
	print "Connecting to Linkmatik\n"
	sock.connect (btS0Addr)

	print "Entering Data Loop\n"

	data = "\r\n"

	while (data != None):

		# Recieve some Data!
		data = data + sock.recv(255)
		idx = 0
		while (True):
			idx = data.find ('\r\n')
			if (idx == -1):
				break
			msg = data[:idx].strip('\r\n')
			data = data[idx:].lstrip('\r\n')
			print msg

while (True):
	nearby_devices = bluetooth.discover_devices ()
	sock = bluetooth.BluetoothSocket (bluetooth.RFCOMM)

	for bdaddr in nearby_devices:
		name = bluetooth.lookup_name (bdaddr)
                service_list = bluetooth.find_service (address = bdaddr)
                print "Found Device: ", name, "with address: ", bdaddr, "with services: "

                for svc in service_list:
                        print "     ", svc["name"], ":", svc["protocol"]

		#Connect to Linkmatik
		if (bdaddr == "00:07:80:81:38:A1"):
			print "Found LinkMatic 2.0"
			linkConnect (bdaddr)
			break

		# Clear Variables
		name = None
		service_list = None
