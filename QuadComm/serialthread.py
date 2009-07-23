#!/usr/bin/python

import serial
import threading

class SerialIOThread:
	def __init__(self, port, baud, rxcallback):
		self.serial = serial.Serial(port, baud, timeout=0.1)
				
		self.rxcallback = rxcallback
		self.rxstop = False
		self.rxthread = threading.Thread(target=self.RunRX, name="RX Thread")
		self.rxthread.daemon = True
		self.rxthread.start()

	def Write(self, str):
		self.serial.write(str)
	
	def Close(self):
		self.rxstop = True
		self.rxthread.join()
		self.serial.close()
		
	def RunRX(self):
		while True:
			if self.rxstop:
				return
				
			ch = self.serial.read()
			if ch != '':
				self.rxcallback(ch)
				
