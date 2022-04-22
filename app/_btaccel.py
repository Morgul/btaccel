import bluetooth

while (True):
	nearby_devices = bluetooth.discover_devices()

	for bdaddr in nearby_devices:
		name = bluetooth.lookup_name (bdaddr)
		service_list = bluetooth.find_service (address=bdaddr)
		print "Found Device: ", name, "with address: ", bdaddr, "with services: "

		for svc in service_list:
			print "     ", svc["name"], ":", svc["protocol"]
