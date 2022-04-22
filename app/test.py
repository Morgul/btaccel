data = "00FF\r\n0099\r\n\r\n\r\nFFFF\r\nFF3"
idx = 0
while (True):
	idx = data.find ('\r\n')
	if (idx == -1):
		break
	sendBuff = data[:idx].strip('\r\n')
	data = data[idx:].lstrip('\r\n')
	print sendBuff
print "data: ", data
