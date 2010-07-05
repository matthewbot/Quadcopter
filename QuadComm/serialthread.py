#!/usr/bin/python

import cp2103
import threading
import time

class SerialIOThread:
	def __init__(self, port, baud, rxcallback):
		self.serial = cp2103.CP2103(port, baud, timeout=0.1)
				
		self.rxcallback = rxcallback
		self.rxstop = False
		self.rxthread = threading.Thread(target=self.RunRX, name="RX Thread")
		self.rxthread.daemon = True
		self.rxthread.start()

	def Write(self, str):
		self.serial.write(str)
	
	def Reset(self, program=False):
		self.serial.set_gpios(False, program)
		time.sleep(.1)
		self.serial.set_gpios(True, program)
		time.sleep(.3)
		self.serial.set_gpios(True)
	
	def Close(self):
		self.rxstop = True
		self.rxthread.join()
		self.serial.close()
		
	def RunRX(self):
		while True:
			if self.rxstop:
				return
				
			ch = self.serial.read(max(self.serial.inWaiting(), 1))
			if ch != '':
				self.rxcallback(ch)
				
